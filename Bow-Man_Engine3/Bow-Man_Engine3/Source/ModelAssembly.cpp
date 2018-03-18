#include "stdafx.h"
#include "ModelAssembly.h"

#include "Renderer.h"
#include "Picking.h"
#include "CFBXLoader.h"
#include "UtilityFunction.h"

#include "Mesh.h"
#include "Model.h"
#include "ModelHomo.h"

CModelAssembly::CModelAssembly(ID3D11Device * pDevice, char * pChar) {
	using FBX_LOADER::CFBXLoader;

	CFBXLoader fbxLoader;
	fbxLoader.LoadFBX(pChar, CFBXLoader::eAXIS_SYSTEM::eAXIS_DIRECTX);

	for (int i = 0; i<fbxLoader.GetNodesCount(); ++i) {
		FBX_LOADER::FBX_MESH_NODE meshNode = fbxLoader.GetNode(i);

		if (!meshNode.m_positionArray.size()) continue;

		float fMaxX = FLT_MIN, fMaxY = FLT_MIN, fMaxZ = FLT_MIN, fMinX = FLT_MAX, fMinY = FLT_MAX, fMinZ = FLT_MAX;
		MeshData meshData;

		for (FbxVector4 vPos : meshNode.m_positionArray) {
			XMFLOAT3 vPosition(vPos[0], vPos[1], vPos[2]);
			meshData.m_vvPosition.push_back(vPosition);

			//AABB
			if (vPosition.x > fMaxX) fMaxX = vPosition.x;
			if (vPosition.y > fMaxY) fMaxY = vPosition.y;
			if (vPosition.z > fMaxZ) fMaxZ = vPosition.z;
			if (vPosition.x < fMinX) fMinX = vPosition.x;
			if (vPosition.y < fMinY) fMinY = vPosition.y;
			if (vPosition.z < fMinZ) fMinZ = vPosition.z;
		}
		for (int i = 0; i<meshNode.m_positionArray.size(); ++i) {
			meshData.m_vvColor.push_back(XMFLOAT4(rand() % 255 / 255.0f, rand() % 255 / 255.0f, rand() % 255 / 255.0f, 1));
		}
		for (FbxVector2 vTex : meshNode.m_texcoordArray) {
			XMFLOAT2 vTexCoord(vTex[0], -vTex[1]);
			meshData.m_vvTexCoord1.push_back(vTexCoord);
		}
		for (FbxVector4 vNorm : meshNode.m_normalArray) {
			XMFLOAT3 vNormal(vNorm[0], vNorm[1], vNorm[2]);
			meshData.m_vvNormal.push_back(vNormal);
		}

		float fSizeX = abs((fMaxX - fMinX) / 2);
		float fSizeY = abs((fMaxY - fMinY) / 2);
		float fSizeZ = abs((fMaxZ - fMinZ) / 2);
		meshData.m_AABB = BoundingBox(XMFLOAT3((fMinX + fMaxX) / 2, (fMinY + fMaxY) / 2, (fMinZ + fMaxZ) / 2), XMFLOAT3(fSizeX, fSizeY, fSizeZ));
		//	meshData.m_AABB = BoundingBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(10.0f,10.0f,10.0f));
		BoundingBox::CreateMerged(m_AABB, m_AABB, meshData.m_AABB);

		CModel* pModel = new CModel;
		pModel->SetMesh(new CMesh(pDevice, meshData));
		pModel->SetTexture(Texture(L"default"));
		pModel->SetMaterial(Material(L"default"));
		AddModel(pModel);

		meshData.Clear();
		fMaxX = fMaxY = fMaxZ = -FLT_MAX;
		fMinX = fMinY = fMinZ = +FLT_MAX;
	}
	CreateModelHomo();
}

CUiObject * CModelAssembly::CreateUI(const POINT & vMousePos, UINT element, bool bBodyFlag) {
	CUiObject *pUiObject = UiManager->CreateUi(L"ModelAssembly", element, bBodyFlag);
	pUiObject->SetSizeX(200);
	for (CModel* pModelData : m_vpModel) {
		CUiObject *pChild = new CUiObject;
		pChild->SetSize(0, 20);
		pChild->SetData(pModelData);
		pChild->m_uCoordAttri = UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE;
		pChild->SetRenderFunc(RenderModelData);
		pChild->SetMouseInputFunc(MouseInputModel);
		pUiObject->AddChild(pChild);
	}
	pUiObject->SetPosition(vMousePos.x, vMousePos.y);

	return pUiObject;
}

void CModelAssembly::CreateModelHomo() {
	ReleaseModelHomo();

	for (CModel *pModel : m_vpModel) {
		pModel->CalModelID();

		bool bFlag = false;
		for (CModelHomo* pModelHomo : m_vpModelHomo) {
			if (pModelHomo->GetRendererType() == pModel->GetRendererType()) {
				pModelHomo->AddModel(pModel);
				bFlag = true;
				break;
			}
		}
		//새로운 Render Type인 경우, 새로운 ModelHomo를 생성한다
		if (bFlag == false) {
			CModelHomo* pModelHomo = new CModelHomo;
			pModelHomo->AddModel(pModel);
			pModelHomo->SetRenderer(pModel->GetRendererType());
			m_vpModelHomo.push_back(pModelHomo);
		}
	}
}

void CModelAssembly::ReleaseModelHomo() {
	for (CModelHomo *pModelHomo : m_vpModelHomo) {
		pModelHomo->Release();
	}
	m_vpModelHomo.clear();
}

void CModelAssembly::LoadObjFile(ID3D11Device * pDevice, WCHAR * pwChar, float fScale, UINT element)
{
	float fMaxX, fMaxY, fMaxZ, fMinX, fMinY, fMinZ;
	fMaxX = fMaxY = fMaxZ = -FLT_MAX;
	fMinX = fMinY = fMinZ = +FLT_MAX;

	std::vector<XMFLOAT3>	vvTempPosition;
	std::vector<XMFLOAT3>	vvTempNormal;
	std::vector<XMFLOAT2>	vvTempTexCoord;
	std::vector<XMFLOAT4>	vvTempColor;

	bool bFlag = true;
	UINT vModelDataPivot = -1;
	UINT idxIndex = 0;
	MeshData meshData;
	char cIgnore;

	std::ifstream fRead(pwChar);
	if (fRead.fail()) {
		MyMessageBox(L"Failed To Read \n", pwChar);
		exit(-1);
	}

	std::string sData;
	while (!fRead.eof()) {
		std::getline(fRead, sData);

		if (sData.substr(0, 2) == "v ") {
			bFlag = true;

			std::istringstream sVector(sData.substr(2));
			float x, y, z;
			sVector >> x >> y >> z;

			x *= -fScale;
			y *= fScale;
			z *= fScale;
			vvTempPosition.push_back(XMFLOAT3(x, y, z));
		}
		else if (sData.substr(0, 2) == "vt") {
			std::istringstream sTexCoord(sData.substr(2));
			float u, v;
			sTexCoord >> u >> v;
			vvTempTexCoord.push_back(XMFLOAT2(u, -v));
		}
		else if (sData.substr(0, 2) == "vn") {
			std::istringstream sNormal(sData.substr(2));
			float x, y, z;
			sNormal >> x >> y >> z;
			vvTempNormal.push_back(XMFLOAT3(x, y, z));
		}
		else if (sData.substr(0, 2) == "f ") {
			goto WARP;
			while (std::getline(fRead, sData)) {
				if (sData.substr(0, 2) == "f ") {
				WARP:
					std::vector<UINT> vPositionIndex;
					std::vector<UINT> vTexCoordIndex;
					std::vector<UINT> vNormalIndex;
					UINT v, t, n;

					std::istringstream sFace(sData.substr(2));
					while (sFace >> v >> cIgnore >> t >> cIgnore >> n) {
						vPositionIndex.push_back(--v);
						vTexCoordIndex.push_back(--t);
						vNormalIndex.push_back(--n);

						(fMaxX < vvTempPosition[v].x) ? fMaxX = vvTempPosition[v].x : NULL;
						(fMaxY < vvTempPosition[v].y) ? fMaxY = vvTempPosition[v].y : NULL;
						(fMaxZ < vvTempPosition[v].z) ? fMaxZ = vvTempPosition[v].z : NULL;
						(fMinX > vvTempPosition[v].x) ? fMinX = vvTempPosition[v].x : NULL;
						(fMinY > vvTempPosition[v].y) ? fMinY = vvTempPosition[v].y : NULL;
						(fMinZ > vvTempPosition[v].z) ? fMinZ = vvTempPosition[v].z : NULL;
					}

					meshData.m_vIndex.push_back(idxIndex++);
					meshData.m_vIndex.push_back(idxIndex++);
					meshData.m_vIndex.push_back(idxIndex++);

					meshData.m_vvPosition.push_back(vvTempPosition[vPositionIndex[1]]);
					meshData.m_vvPosition.push_back(vvTempPosition[vPositionIndex[0]]);
					meshData.m_vvPosition.push_back(vvTempPosition[vPositionIndex[2]]);

					meshData.m_vvTexCoord1.push_back(vvTempTexCoord[vTexCoordIndex[1]]);
					meshData.m_vvTexCoord1.push_back(vvTempTexCoord[vTexCoordIndex[0]]);
					meshData.m_vvTexCoord1.push_back(vvTempTexCoord[vTexCoordIndex[2]]);

					XMFLOAT3 vNormal;
					XMStoreFloat3(&vNormal, XMVector3Normalize(XMLoadFloat3(&vvTempNormal[vNormalIndex[1]])));
					vNormal.x *= -1;
					meshData.m_vvNormal.push_back(vNormal);
					XMStoreFloat3(&vNormal, XMVector3Normalize(XMLoadFloat3(&vvTempNormal[vNormalIndex[0]])));
					vNormal.x *= -1;
					meshData.m_vvNormal.push_back(vNormal);
					XMStoreFloat3(&vNormal, XMVector3Normalize(XMLoadFloat3(&vvTempNormal[vNormalIndex[2]])));
					vNormal.x *= -1;
					meshData.m_vvNormal.push_back(vNormal);

					if (vPositionIndex.size() == 4) {
						meshData.m_vIndex.push_back(idxIndex++);
						meshData.m_vIndex.push_back(idxIndex++);
						meshData.m_vIndex.push_back(idxIndex++);

						meshData.m_vvPosition.push_back(vvTempPosition[vPositionIndex[2]]);
						meshData.m_vvPosition.push_back(vvTempPosition[vPositionIndex[0]]);
						meshData.m_vvPosition.push_back(vvTempPosition[vPositionIndex[3]]);

						meshData.m_vvTexCoord1.push_back(vvTempTexCoord[vTexCoordIndex[2]]);
						meshData.m_vvTexCoord1.push_back(vvTempTexCoord[vTexCoordIndex[0]]);
						meshData.m_vvTexCoord1.push_back(vvTempTexCoord[vTexCoordIndex[3]]);

						XMStoreFloat3(&vNormal, XMVector3Normalize(XMLoadFloat3(&vvTempNormal[vNormalIndex[2]])));
						vNormal.x *= -1;
						meshData.m_vvNormal.push_back(vNormal);
						XMStoreFloat3(&vNormal, XMVector3Normalize(XMLoadFloat3(&vvTempNormal[vNormalIndex[0]])));
						vNormal.x *= -1;
						meshData.m_vvNormal.push_back(vNormal);
						XMStoreFloat3(&vNormal, XMVector3Normalize(XMLoadFloat3(&vvTempNormal[vNormalIndex[3]])));
						vNormal.x *= -1;
						meshData.m_vvNormal.push_back(vNormal);
					}
					//	else if (vPositionIndex.size() >= 5)
					//		MyMessageBox(L"정점이 5개 이상");
				}
				else if (sData.substr(0, 2) == "s ");
				else
					break;
			}
			for (int i = 0; i<meshData.m_vvPosition.size(); ++i)
				meshData.m_vvColor.push_back(XMFLOAT4(rand() % 255 / 255.0f, rand() % 255 / 255.0f, rand() % 255 / 255.0f, 1));

			//meshData.m_vvNormal.clear();
			//for (int i = 0; i<meshData.m_vvPosition.size() ; i+=3) {
			//	XMVECTOR p1 = XMLoadFloat3(&meshData.m_vvPosition[i]);
			//	XMVECTOR p2 = XMLoadFloat3(&meshData.m_vvPosition[i+1]);
			//	XMVECTOR p3 = XMLoadFloat3(&meshData.m_vvPosition[i+2]);

			//	XMVECTOR v1 = p1 - p2;
			//	XMVECTOR v2 = p1 - p3;

			//	XMVECTOR n = XMVector3Normalize(XMVector3Cross(v1, v2));
			//	XMFLOAT3 f3N;
			//	XMStoreFloat3(&f3N, n);
			//	meshData.m_vvNormal.push_back(f3N);
			//	meshData.m_vvNormal.push_back(f3N);
			//	meshData.m_vvNormal.push_back(f3N);
			//}

			float fSizeX = abs((fMaxX - fMinX) / 2);
			float fSizeY = abs((fMaxY - fMinY) / 2);
			float fSizeZ = abs((fMaxZ - fMinZ) / 2);
			meshData.m_AABB = BoundingBox(XMFLOAT3((fMinX + fMaxX) / 2, (fMinY + fMaxY) / 2, (fMinZ + fMaxZ) / 2), XMFLOAT3(fSizeX, fSizeY, fSizeZ));
			BoundingBox::CreateMerged(m_AABB, m_AABB, meshData.m_AABB);

			CModel* pModel = new CModel;
			pModel->SetMesh(new CMesh(pDevice, meshData));
			pModel->SetTexture(Texture(L"default"));
			pModel->SetMaterial(Material(L"default"));
			AddModel(pModel);

			idxIndex = 0;
			meshData.Clear();
			fMaxX = fMaxY = fMaxZ = -FLT_MAX;
			fMinX = fMinY = fMinZ = +FLT_MAX;
		}
	}
	fRead.close();
}

void TriangulateRecursive(FbxScene *pScene, FbxNode* pNode, std::wstring wsTab)
{
	FbxNodeAttribute* pNodeAttribute = pNode->GetNodeAttribute();

    if (pNodeAttribute){
	//	std::wcout << wsTab << pNode->GetName() << " " << pNodeAttribute->GetAttributeType() << endl;

		FbxNodeAttribute::EType eNodeAttribute = pNodeAttribute->GetAttributeType();
        if (eNodeAttribute == FbxNodeAttribute::eMesh ||
            eNodeAttribute == FbxNodeAttribute::eNurbs ||
            eNodeAttribute == FbxNodeAttribute::ePatch ||
            eNodeAttribute == FbxNodeAttribute::eNurbsSurface)
        {

			FbxGeometryConverter lConverter(pNode->GetFbxManager());
#if 0
            lConverter.TriangulateInPlace(pNode);
#endif
			lConverter.Triangulate( pScene, true );
        }
    }
//	else
//		std::wcout << wsTab << pNode->GetName() << endl;

	const int lChildCount = pNode->GetChildCount();
    for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
    {
	//	wsTab += _T("|-");
        TriangulateRecursive(pScene, pNode->GetChild(lChildIndex), wsTab);
    }
}


void SetupNode(std::vector<MeshData>& vMeshData, FbxNode* pNode, std::string parentName, float fScale )
{
	if(!pNode) return ;

//	FBX_MESH_NODE meshNode;
//	meshNode.name = pNode->GetName();
//	meshNode.parentName = parentName;
//	ZeroMemory( &meshNode.elements, sizeof(MESH_ELEMENTS) );

	FbxMesh* pFbxMesh = pNode->GetMesh();	

	MeshData meshData;

	if(pFbxMesh)
	{
		const int nVertex = pFbxMesh->GetControlPointsCount();

		if (nVertex > 0)
		{
			int lPolygonCount = pFbxMesh->GetPolygonCount();

			FbxVector4 pos, nor;
	
		//	meshNode->elements.numPosition = 1;
		//	meshNode->elements.numNormal = 1;

			unsigned int indx = 0;
	
			for(int i=0;i<lPolygonCount;i++)
			{
				int lPolygonsize = pFbxMesh->GetPolygonSize(i);

				for(int pol=0;pol<lPolygonsize;pol++)
				{
					int index = pFbxMesh->GetPolygonVertex(i, pol);
					meshData.m_vIndex.push_back(indx++);

					pos = pFbxMesh->GetControlPointAt(index);
					meshData.m_vvPosition.push_back(XMFLOAT3{
						-static_cast<float>(pos.mData[X]) * fScale, 
						static_cast<float>(pos.mData[Y]) * fScale, 
						static_cast<float>(pos.mData[Z]) * fScale
					});

					pFbxMesh->GetPolygonVertexNormal(i,pol,nor);
					meshData.m_vvNormal.push_back(XMFLOAT3{ 
						-static_cast<float>(nor.mData[X]), 
						static_cast<float>(nor.mData[Y]), 
						static_cast<float>(nor.mData[Z])
					});
				}
				std::swap(meshData.m_vIndex[indx-1], meshData.m_vIndex[indx-2]);
			}

			FbxStringList	uvsetName;
			pFbxMesh->GetUVSetNames(uvsetName);
			int numUVSet = uvsetName.GetCount();
			//meshNode->elements.numUVSet = numUVSet;

			bool unmapped = false;

			for(int uv=0;uv<numUVSet;uv++)
			{
			//	meshNode->uvsetID[uvsetName.GetStringAt(uv)] = uv;
				for(int i=0;i<lPolygonCount;i++)
				{
					int lPolygonsize = pFbxMesh->GetPolygonSize(i);

					for(int pol=0;pol<lPolygonsize;pol++)
					{
						FbxString name = uvsetName.GetStringAt(uv);
						FbxVector2 texCoord;
						pFbxMesh->GetPolygonVertexUV( i, pol, name, texCoord, unmapped);
						meshData.m_vvTexCoord1.push_back(XMFLOAT2{
							static_cast<float>(texCoord.mData[X]),
							-static_cast<float>(texCoord.mData[Y]) 
						});
					}
				}
			}

			vMeshData.push_back(meshData);
		}
	}

	//const int lMaterialCount = pNode->GetMaterialCount();
	//for(int i=0;i<lMaterialCount;i++)
	//{
	//	FbxSurfaceMaterial* mat = pNode->GetMaterial(i);
	//	if(!mat)
	//		continue;

	//	FBX_MATERIAL_NODE destMat;
	//	CopyMatrialData(mat, &destMat);

	//	meshNode.m_materialArray.push_back(destMat);
	//}

	//
	//ComputeNodeMatrix(pNode, &meshNode);

	const int lCount = pNode->GetChildCount();
	for (int i = 0; i < lCount; i++)
	{
		SetupNode(vMeshData, pNode->GetChild(i), "null", fScale);
	}
}


void CModelAssembly::LoadFbxFile(ID3D11Device * pDevice, WCHAR * pwChar, float fScale, UINT element) 
{
	char pChar[256];
	wcstombs(pChar, pwChar, lstrlen(pwChar)+1);
	
	static FbxManager* pFbxSdkManager = nullptr;
	if (pFbxSdkManager == nullptr) {
		pFbxSdkManager = FbxManager::Create();

		FbxIOSettings* pIOsettings = FbxIOSettings::Create(pFbxSdkManager, IOSROOT);
		pFbxSdkManager->SetIOSettings(pIOsettings);
		//	pIOsettings->Destroy();
	}

	FbxImporter* pImporter = FbxImporter::Create(pFbxSdkManager, "");
	FbxScene* pFbxScene = FbxScene::Create(pFbxSdkManager, "My Scene");

	if (!pImporter->Initialize(pChar, -1, pFbxSdkManager->GetIOSettings())) {
		MyMessageBox(__FUNCTION__, "\n", pChar, L"\n파일을 불러들이지 못했습니다");
		exit(-1);
	}
	if (!pImporter->Import(pFbxScene)) {
		MyMessageBox(__FUNCTION__, "\n", pChar, L"\nScene으로 Inport 실패");
		exit(-1);
	}

	FbxNode* pFbxRootNode = pFbxScene->GetRootNode();
	int nNodeChild = pFbxRootNode->GetChildCount();

	if (!pFbxRootNode) return;

	//Scene -> Root Node -> Nodes

	TriangulateRecursive(pFbxScene, pFbxRootNode, _T(""));

	std::vector<MeshData> vMeshData;
	SetupNode(vMeshData, pFbxScene->GetRootNode(), "null", fScale);

	for (const MeshData& meshData : vMeshData) {
		CModel* pModel = new CModel;
		pModel->SetMesh(new CMesh(pDevice, meshData));
		pModel->SetTexture(Texture(L"default"));
		pModel->SetMaterial(Material(L"default"));
		AddModel(pModel);
	}
}

void CModelAssembly::LoadModel(ID3D11Device * pDevice, WCHAR * pwChar, float fScale, UINT element) {
	std::wstring wsName;
	std::wstring wsFormat;
	GetFileNameFormat(pwChar, wsName, wsFormat);

	if (wsFormat == _T("obj"))		LoadObjFile(pDevice, pwChar, fScale, element);
	else if (wsFormat == _T("fbx"))	LoadFbxFile(pDevice, pwChar, fScale, element);

	CreateModelHomo();
}

void CModelAssembly::AddModel(CModel * pModelData) {
	if (!pModelData) return;
	m_vpModel.push_back(pModelData);
	pModelData->SetModelAssembly(this);
	m_AABB.CreateMerged(m_AABB, pModelData->GetMesh()->GetAABB(), m_AABB);
}

void CModelAssembly::AddToRenderer(const XMFLOAT4X4 & mtxWorld) {
	for (CModelHomo* pModelHomo : m_vpModelHomo)
		pModelHomo->AddToRenderData(mtxWorld);
}

void CModelAssembly::Render(ID3D11DeviceContext * pDeviceContext) {
	for (CModel *pModelData : m_vpModel) {
		pModelData->Render2(pDeviceContext);
	}
}

bool CModelAssembly::ModelAssemblyPicking(const XMVECTOR & vRayPosition, const XMVECTOR & vRayDirection, PickingData& pickData) {
	bool bFlag = false;

	for (CModel* pModelData : m_vpModel) {
		if (pModelData->ModelPicking(vRayPosition, vRayDirection, pickData)) {
			pickData.m_pModelAssembly = this;
			bFlag = true;
		}
	}
	return bFlag;
}