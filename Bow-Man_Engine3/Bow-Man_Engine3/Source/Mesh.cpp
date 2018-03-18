#include "stdafx.h"
//ResourceData -> Mesh 순서 중요
//#include "ResourceData.h"
#include "HeightMap.h"
#include "Picking.h"
#include "Mesh.h"
#include "UtilityFunction.h"

void CMeshBuffer::AddVertexBuffer(VertexBuffer * pVertexBuffer) {
	m_vpVertexBuffer.push_back(pVertexBuffer->m_pVertexBuffer);
	m_vnStride.push_back(pVertexBuffer->m_nStride);
	m_vnOffset.push_back(pVertexBuffer->m_nOffset);
	m_uInputLayout |= pVertexBuffer->m_uInputLayout;
}

void CMeshBuffer::Render(ID3D11DeviceContext * pDeviceContext) {
	pDeviceContext->IASetPrimitiveTopology(m_PrimitiveTopology);
	pDeviceContext->IASetVertexBuffers(0, m_vpVertexBuffer.size(), m_vpVertexBuffer.data(), m_vnStride.data(), m_vnOffset.data());

	if (m_pIndexBuffer) {
		pDeviceContext->IASetIndexBuffer(m_pIndexBuffer->m_pIndexBuffer, m_pIndexBuffer->m_dxgiIndexFormat, m_pIndexBuffer->m_nIndexOffset);
		pDeviceContext->DrawIndexed(m_nIndex, 0, 0);
	}
	else
		pDeviceContext->Draw(m_nPosition, 0);
}

//------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
ID3D11Buffer * CMesh::CreateBuffer(ID3D11Device * pDevice, T & vData, UINT nBindFlags, D3D11_USAGE usage, UINT nCPUAccessFlags)
{
	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	BufferDesc.Usage = usage;
	BufferDesc.ByteWidth = sizeof(vData.front()) * vData.size();
	BufferDesc.BindFlags = nBindFlags;
	BufferDesc.CPUAccessFlags = nCPUAccessFlags;
	D3D11_SUBRESOURCE_DATA BufferData;
	ZeroMemory(&BufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	BufferData.pSysMem = vData.data();

	ID3D11Buffer *pBuffer = NULL;
	pDevice->CreateBuffer(&BufferDesc, &BufferData, &pBuffer);
	return(pBuffer);
}

template<typename T>
void CMesh::AddVertexBuffer(ID3D11Device * pDevice, T & vData, EVertexElement element) {
	if (vData.size() == 0) return;
	m_uInputLayout |= static_cast<UINT>(element);

	VertexBuffer *pVertexBuffer = new VertexBuffer;
	pVertexBuffer->m_uInputLayout |= static_cast<UINT>(element);
	pVertexBuffer->m_pVertexBuffer = CreateBuffer(pDevice, vData, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	pVertexBuffer->m_nStride = sizeof(vData.front());
	pVertexBuffer->m_nOffset = 0;

	m_vpVertexBuffer.push_back(pVertexBuffer);
}


void CMesh::Render(ID3D11DeviceContext * pDeviceContext) {
	pDeviceContext->IASetPrimitiveTopology(m_PrimitiveTopology);

	int idx = 0;
	for (VertexBuffer* pVertexBuffer : m_vpVertexBuffer)
		pDeviceContext->IASetVertexBuffers(idx++, 1, &pVertexBuffer->m_pVertexBuffer, &pVertexBuffer->m_nStride, &pVertexBuffer->m_nOffset);

	if (m_pIndexBuffer) {
		pDeviceContext->IASetIndexBuffer(m_pIndexBuffer->m_pIndexBuffer, m_pIndexBuffer->m_dxgiIndexFormat, m_pIndexBuffer->m_nIndexOffset);
		pDeviceContext->DrawIndexed(m_vIndex.size(), 0, 0);
	}
	else
		pDeviceContext->Draw(m_vvPosition.size(), 0);
}

void CMesh::CreateMesh(ID3D11Device * pDevice, const MeshData & meshData) {
	m_vvPosition = meshData.m_vvPosition;
	m_vIndex = meshData.m_vIndex;

	//	m_PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	//	m_PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	AddVertexBuffer(pDevice, m_vvPosition, EVertexElement::POSITION);
	AddVertexBuffer(pDevice, meshData.m_vvColor, EVertexElement::COLOR);
	AddVertexBuffer(pDevice, meshData.m_vvTexCoord1, EVertexElement::TEXCOORD1);
	AddVertexBuffer(pDevice, meshData.m_vvTexCoord2, EVertexElement::TEXCOORD2);
	AddVertexBuffer(pDevice, meshData.m_vvTangent, EVertexElement::TANGENT);
	AddVertexBuffer(pDevice, meshData.m_vvNormal, EVertexElement::NORMAL);
	SetIndexBuffer(pDevice, m_vIndex);
	m_AABB = meshData.m_AABB;
}

void CMesh::CreateTBN(MeshData & meshData) {
	meshData.m_vvNormal.clear();

	XMFLOAT4X4 f4x4IdentityMatirx;
	XMStoreFloat4x4(&f4x4IdentityMatirx, XMMatrixIdentity());
	UINT nVertexPerPrimitive = 0;

	switch (m_PrimitiveTopology) {
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		nVertexPerPrimitive = 3;
		for (int i = 0; i<meshData.m_vvPosition.size(); i += nVertexPerPrimitive) {
			XMFLOAT3 pos1 = meshData.m_vvPosition[i];
			XMFLOAT3 pos2 = meshData.m_vvPosition[i + 1];
			XMFLOAT3 pos3 = meshData.m_vvPosition[i + 2];

			XMFLOAT2 tex1 = meshData.m_vvTexCoord1[i];
			XMFLOAT2 tex2 = meshData.m_vvTexCoord1[i + 1];
			XMFLOAT2 tex3 = meshData.m_vvTexCoord1[i + 2];

			//모델의 3차원 벡터
			XMFLOAT3 posVec1;
			posVec1.x = pos2.x - pos1.x;
			posVec1.y = pos2.y - pos1.y;
			posVec1.z = pos2.z - pos1.z;
			XMFLOAT3 posVec2;
			posVec2.x = pos3.x - pos1.x;
			posVec2.y = pos3.y - pos1.y;
			posVec2.z = pos3.z - pos1.z;

			//모델의 2차원 텍스쳐 벡터
			XMFLOAT2 texVec1;
			texVec1.x = tex2.x - tex1.x;
			texVec1.y = tex2.y - tex1.y;
			XMFLOAT2 texVec2;
			texVec2.x = tex3.x - tex1.x;
			texVec2.y = tex3.y - tex1.y;

			XMMATRIX mtxPosMatrix;
			XMFLOAT4X4 f4x4PosMatrix = f4x4IdentityMatirx;
			f4x4PosMatrix._11 = posVec1.x;		f4x4PosMatrix._12 = posVec1.y;		f4x4PosMatrix._13 = posVec1.z;
			f4x4PosMatrix._21 = posVec2.x;		f4x4PosMatrix._22 = posVec2.y;		f4x4PosMatrix._23 = posVec2.z;
			mtxPosMatrix = XMLoadFloat4x4(&f4x4PosMatrix);


			XMMATRIX mtxTexMatrix;
			XMFLOAT4X4 f4x4TexMatrix = f4x4IdentityMatirx;;
			f4x4TexMatrix._11 = texVec1.x;		f4x4TexMatrix._12 = texVec1.y;
			f4x4TexMatrix._21 = texVec2.x;		f4x4TexMatrix._22 = texVec2.y;
			mtxTexMatrix = XMLoadFloat4x4(&f4x4TexMatrix);

			//모델3차원벡터행렬 = 모델2차원벡터행렬 * f4x4TBN
			//역행렬(모델2차원벡터행렬) = 모델3차원벡터행렬 = 역행렬(모델2차원벡터행렬) * f4x4TBN
			//f4x4TBN = 역행렬(모델2차원벡터행렬) = 모델3차원벡터행렬

			//f4x4TBN을 구하기 위한 역행렬
			//	D3DXMatrixInverse(&f4x4TexMatrix, NULL, &f4x4TexMatrix);
			float D = (f4x4TexMatrix._11 * f4x4TexMatrix._22) - (f4x4TexMatrix._12 * f4x4TexMatrix._21);
			f4x4TexMatrix._12 *= -1;
			f4x4TexMatrix._21 *= -1;
			std::swap(f4x4TexMatrix._11, f4x4TexMatrix._22);
			mtxTexMatrix /= D;

			XMMATRIX mtxTBN = mtxTexMatrix * mtxPosMatrix;
			XMFLOAT4X4 f4x4TBN;
			XMStoreFloat4x4(&f4x4TBN, mtxTBN);

			XMVECTOR vTangent = XMLoadFloat3(&XMFLOAT3(f4x4TBN._11, f4x4TBN._12, f4x4TBN._13));
			vTangent = XMVector3Normalize(vTangent);
			XMFLOAT3 f3Tangent;
			XMStoreFloat3(&f3Tangent, vTangent);

			XMVECTOR vBitangent = XMLoadFloat3(&XMFLOAT3(f4x4TBN._21, f4x4TBN._22, f4x4TBN._23));
			vBitangent = XMVector3Normalize(vBitangent);
			XMFLOAT3 f3Bitangent;
			XMStoreFloat3(&f3Bitangent, vBitangent);

			XMVECTOR vNormal = XMVector3Normalize(XMVector3Cross(vTangent, vBitangent));
			XMFLOAT3 f3Normal;
			XMStoreFloat3(&f3Normal, vNormal);

			//m_vvTangent.push_back(f3Tangent);		m_vvTangent.push_back(f3Tangent);		m_vvTangent.push_back(f3Tangent);
			//m_vvBinormal.push_back(f3Bitangent);	m_vvBinormal.push_back(f3Bitangent);	m_vvBinormal.push_back(f3Bitangent);
			for (int j = 0; j < nVertexPerPrimitive; ++j) {
				meshData.m_vvTangent.push_back(f3Tangent);
				meshData.m_vvNormal.push_back(f3Normal);
			}
		}
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST:
		//for(int i=0 ; i<m_nVertices ; i+=4){
		//	XMVECTOR pos1 = m_vvPosition[i];
		//	//이부분이 조금 독특하다.
		//	XMVECTOR pos2 = m_vvPosition[i+2];
		//	XMVECTOR pos3 = m_vvPosition[i+1];

		//	XMVECTOR tex1 = m_vvTexCoord1[i];
		//	XMVECTOR tex2 = m_vvTexCoord1[i+1];
		//	XMVECTOR tex3 = m_vvTexCoord1[i+2];

		//	//모델의 3차원 벡터
		//	XMVECTOR posVec1 = pos2 - pos1;
		//	XMVECTOR posVec2 = pos3 - pos1;

		//	//모델의 2차원 텍스쳐 벡터
		//	XMVECTOR texVec1 = tex2 - tex1;
		//	XMVECTOR texVec2 = tex3 - tex1;

		//	XMMATRIX posMatrix = f4x4IdentityMatirx;
		//	posMatrix._11 = posVec1.x;		posMatrix._12 = posVec1.y;		posMatrix._13 = posVec1.z;
		//	posMatrix._21 = posVec2.x;		posMatrix._22 = posVec2.y;		posMatrix._23 = posVec2.z;

		//	XMMATRIX texMatrix = f4x4IdentityMatirx;;
		//	texMatrix._11 = texVec1.x;		texMatrix._12 = texVec1.y;
		//	texMatrix._21 = texVec2.x;		texMatrix._22 = texVec2.y;

		//	float D = (texMatrix._11 * texMatrix._22) - (texMatrix._12 * texMatrix._21);
		//	texMatrix._12 *= -1;
		//	texMatrix._21 *= -1;
		//	swap(texMatrix._11, texMatrix._22);
		//	texMatrix /= D;

		//	XMMATRIX TBN = texMatrix * posMatrix;

		//	XMVECTOR tangent(TBN._11,TBN._12,TBN._13);
		//	D3DXVec3Normalize(&tangent,&tangent);
		//	XMVECTOR bitangent(TBN._21,TBN._22,TBN._23);
		//	D3DXVec3Normalize(&bitangent,&bitangent);
		//	XMVECTOR normal;
		//	D3DXVec3Cross(&normal,&tangent,&bitangent);
		//	D3DXVec3Normalize(&bitangent,&bitangent);

		//	m_vvTangent.push_back(tangent);		m_vvTangent.push_back(tangent);		m_vvTangent.push_back(tangent);		m_vvTangent.push_back(tangent);
		//	m_vvBinormal.push_back(bitangent);	m_vvBinormal.push_back(bitangent);	m_vvBinormal.push_back(bitangent);	m_vvBinormal.push_back(bitangent);
		//	m_vvNormal.push_back(normal);		m_vvNormal.push_back(normal);		m_vvNormal.push_back(normal);		m_vvNormal.push_back(normal);
		//}
		break;
	default:
		break;
	}
}

void CMesh::CreateAABB()
{
	float fMaxX, fMaxY, fMaxZ, fMinX, fMinY, fMinZ;
	fMaxX = fMaxY = fMaxZ = -FLT_MAX;
	fMinX = fMinY = fMinZ = +FLT_MAX;

	for (const XMFLOAT3& f3Position : m_vvPosition) {		
		(fMaxX < f3Position.x) ? fMaxX = f3Position.x : NULL;
		(fMaxY < f3Position.y) ? fMaxY = f3Position.y : NULL;
		(fMaxZ < f3Position.z) ? fMaxZ = f3Position.z : NULL;
		(fMinX > f3Position.x) ? fMinX = f3Position.x : NULL;
		(fMinY > f3Position.y) ? fMinY = f3Position.y : NULL;
		(fMinZ > f3Position.z) ? fMinZ = f3Position.z : NULL;
	}
	float fSizeX = abs((fMaxX - fMinX) / 2);
	float fSizeY = abs((fMaxY - fMinY) / 2);
	float fSizeZ = abs((fMaxZ - fMinZ) / 2);
	m_AABB = BoundingBox(XMFLOAT3((fMinX + fMaxX) / 2, (fMinY + fMaxY) / 2, (fMinZ + fMaxZ) / 2), XMFLOAT3(fSizeX, fSizeY, fSizeZ));
}

void CMesh::SetIndexBuffer(ID3D11Device * pDevice, std::vector<UINT> vData) {
	if (vData.size() == 0) return;
	m_pIndexBuffer = new IndexBuffer;
	m_pIndexBuffer->m_pIndexBuffer = CreateBuffer(pDevice, vData, D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
}

bool CMesh::MeshPicking(const XMVECTOR & vRayPosition,const XMVECTOR & vRayDirection, PickingData &pickingData){
//※만약 Collision Mesh가 있다면 그것을 사용할 수 있도록 한다

	bool bCollided = false;
	float fTempDistance;
	UINT nVertex = m_vvPosition.size();
	UINT nIndex = m_vIndex.size();
	float fResultDistance;

	//모델 좌표계 AABB 광선 충돌
	if (!m_AABB.Intersects(vRayPosition, vRayDirection, fResultDistance))
		return false;

	int nPrimitives;
	switch(m_PrimitiveTopology){
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
	case D3D11_PRIMITIVE_TOPOLOGY_LINELIST:
		nPrimitives = nVertex / 3;	break;
	default:
		nPrimitives = nVertex - 2;	break;
	}

#define WITH_INDEX		0
#define NO_INDEX		1

	char pickType = NO_INDEX;
	if (nIndex > 0) {
		pickType = WITH_INDEX;
		nPrimitives = (m_PrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (nIndex / 3) : (nIndex - 2);
	}

	XMVECTOR v[3];
	int index= 0;
	int nFlag = 1;
	for(int i = 0; i < nPrimitives; i++){
		switch (pickType) {
		case WITH_INDEX:
			if (D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP == m_PrimitiveTopology) {
				if (nFlag == 1) {
					v[0] = XMLoadFloat3(&m_vvPosition[m_vIndex[index]]);
					v[1] = XMLoadFloat3(&m_vvPosition[m_vIndex[index + 1]]);
					v[2] = XMLoadFloat3(&m_vvPosition[m_vIndex[index + 2]]);
				}
				else {
					v[0] = XMLoadFloat3(&m_vvPosition[m_vIndex[index]]);
					v[1] = XMLoadFloat3(&m_vvPosition[m_vIndex[index + 2]]);
					v[2] = XMLoadFloat3(&m_vvPosition[m_vIndex[index + 1]]);
				}
				nFlag *= -1;
				index++;
			}
			else {
				v[0] = XMLoadFloat3(&m_vvPosition[m_vIndex[index++]]);
				v[1] = XMLoadFloat3(&m_vvPosition[m_vIndex[index++]]);
				v[2] = XMLoadFloat3(&m_vvPosition[m_vIndex[index++]]);
			}
			break;
		case NO_INDEX:
			if (D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP == m_PrimitiveTopology) {
				if (nFlag == 1) {
					v[0] = XMLoadFloat3(&m_vvPosition[m_vIndex[i    ]]);
					v[1] = XMLoadFloat3(&m_vvPosition[m_vIndex[i + 1]]);
					v[2] = XMLoadFloat3(&m_vvPosition[m_vIndex[i + 2]]);
				}
				else {
					v[0] = XMLoadFloat3(&m_vvPosition[m_vIndex[i    ]]);
					v[1] = XMLoadFloat3(&m_vvPosition[m_vIndex[i + 2]]);
					v[2] = XMLoadFloat3(&m_vvPosition[m_vIndex[i + 1]]);
				}
				nFlag *= -1;
			}
			else {
				v[0] = XMLoadFloat3(&m_vvPosition[index++]);
				v[1] = XMLoadFloat3(&m_vvPosition[index++]);
				v[2] = XMLoadFloat3(&m_vvPosition[index++]);
			}
			break;
		}
		//Prevent Picking From Inside Of The Object
		XMVECTOR vOrthogonal = XMVector3Cross(v[1]-v[0],v[2]-v[0]);

		float fDotResult = 0;
		XMStoreFloat(&fDotResult,XMVector3Dot(vOrthogonal,vRayDirection));
		if(fDotResult > 0) continue;

		if(DirectX::TriangleTests::Intersects(vRayPosition,vRayDirection,v[0],v[1],v[2],fResultDistance)){
			if(pickingData.m_fNearestDistance > fResultDistance){
				pickingData.m_fNearestDistance = fResultDistance;
				bCollided = true;
			//	static int num;
			//	cout << "??" << num++ << endl;
			}
		}
	}

	return bCollided;
}

//아무 값도 넣지 않으면 최상위 옵션이 부여된다

CMeshBuffer * CMesh::GetMeshBuffer(UINT uInputLayout) {
	CMeshBuffer* pMeshBuffer = nullptr;

	UINT result = uInputLayout & m_uInputLayout;
	if (result == uInputLayout) {
		pMeshBuffer = m_mpMeshBuffer[uInputLayout];

		if (!pMeshBuffer) {
			pMeshBuffer = new CMeshBuffer(m_PrimitiveTopology, m_vvPosition.size(), m_vIndex.size());

			for (VertexBuffer* pVertexBuffer : m_vpVertexBuffer) {
				if (uInputLayout & pVertexBuffer->m_uInputLayout)
					pMeshBuffer->AddVertexBuffer(pVertexBuffer);
			}

			if (m_pIndexBuffer)
				pMeshBuffer->SetIndexBuffer(m_pIndexBuffer);
			m_mpMeshBuffer[uInputLayout] = pMeshBuffer;
		}
	}
	else {
		auto end = std::end(gInputLayoutList);
		for (auto pivot = std::begin(gInputLayoutList); pivot != end; ++pivot) {
			UINT result = *pivot & m_uInputLayout;
			if (result == *pivot)
				return GetMeshBuffer(*pivot);
		}
	}
	return pMeshBuffer;
}


//---------------------------------------------------------------------------------------------------
CSectorMesh::CSectorMesh(ID3D11Device* pDevice, float fWidth, float fHeight, float fDepth, int iSectorNum, bool bFlat, XMCOLOR vColor){
	int pointNum = iSectorNum + 1;
	float fx = fWidth*0.5f, fy = fHeight * 0.5f, fz = fDepth*0.5f;
	float leafLength = fWidth/iSectorNum;
		
	UINT nVertex = 0;
	int index = 0;

	switch(bFlat){
	case true:
		nVertex = pointNum * 4;
		for(int i=0 ; i<nVertex ; ++i) m_vvPosition.push_back(XMFLOAT3(0, 0, 0));

		for(int i=0 ; i<pointNum ; ++i){
			m_vvPosition[index++] = XMFLOAT3(-fx, 0, leafLength*i-fz);
			m_vvPosition[index++] = XMFLOAT3(+fx, 0, leafLength*i-fz);

			m_vvPosition[index++] = XMFLOAT3(leafLength*i-fx, 0, -fz);
			m_vvPosition[index++] = XMFLOAT3(leafLength*i-fx, 0, +fz);
		}

		for(int i=0 ; i<nVertex ; ++i) m_vIndex.push_back(i);
		break;
	case false:
		nVertex = pointNum * pointNum * 2;
		for(int i=0 ; i<nVertex ; ++i) m_vvPosition.push_back(XMFLOAT3(0, 0, 0));

		for(int i=0 ; i<pointNum ; ++i){
			for(int j=0 ; j<pointNum ; ++j){
			
				if(i!=pointNum-1 && j!=pointNum-1){
					m_vvPosition[(i*pointNum) + j] = XMFLOAT3(leafLength*j-fx, 0, leafLength*i-fz);
					m_vvPosition[(i*pointNum) + j + nVertex/2] = XMFLOAT3(leafLength*j-fx, fy, leafLength*i-fz);
				}
				else if(j != pointNum-1){
					m_vvPosition[(i*pointNum) + j] = XMFLOAT3(leafLength*j-fx, 0, leafLength*i-fz-4);
					m_vvPosition[(i*pointNum) + j + nVertex/2] = XMFLOAT3(leafLength*j-fx, fy, leafLength*i-fz-4);
				}
				else if(i != pointNum-1){
					m_vvPosition[(i*pointNum) + j] = XMFLOAT3(leafLength*j-fx-4, 0, leafLength*i-fz);
					m_vvPosition[(i*pointNum) + j + nVertex/2] = XMFLOAT3(leafLength*j-fx-4, fy, leafLength*i-fz);
				}
				else{
					m_vvPosition[(i*pointNum) + j] = XMFLOAT3(leafLength*j-fx-4, 0, leafLength*i-fz-4);
					m_vvPosition[(i*pointNum) + j + nVertex/2] = XMFLOAT3(leafLength*j-fx-4, fy, leafLength*i-fz-4);
				}
			}
		}

		for(int i=0 ; i<nVertex/2 ; ++i){
			m_vIndex.push_back(i);									m_vIndex.push_back(i+nVertex/2);

			if(i < pointNum){
				m_vIndex.push_back(i);								m_vIndex.push_back(i + (pointNum*(pointNum-1)));
				m_vIndex.push_back(i+(pointNum*pointNum));			m_vIndex.push_back(i + (pointNum*(pointNum-1))+(pointNum*pointNum));
			}
		
			if(i % pointNum == 0){
				m_vIndex.push_back(i);								m_vIndex.push_back(i+(pointNum-1));
				m_vIndex.push_back(i+(pointNum*pointNum));			m_vIndex.push_back(i+(pointNum*pointNum)+(pointNum-1));
			}
		}
		break;
	}

	std::vector<XMFLOAT4> vvColor;
	for (int i = 0; i < m_vvPosition.size(); i++){
		vvColor.push_back(XMFLOAT4(RandomBetween(0.3, 10), RandomBetween(0.3, 10), RandomBetween(0.3, 10), 1));
	//	vvColor.push_back(XMFLOAT4(0, 0, 3, 1));
	}

	m_PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	AddVertexBuffer(pDevice, m_vvPosition, EVertexElement::POSITION);
	AddVertexBuffer(pDevice, vvColor, EVertexElement::COLOR);
	SetIndexBuffer(pDevice, m_vIndex);

	m_AABB = BoundingBox(XMFLOAT3(0, 0, 0), XMFLOAT3(fx, fy, fz));
}

CSectorMesh::CSectorMesh(ID3D11Device * pDevice, float fWidth, float fHeight, float fDepth, int iSectorNum, CHeightMap * pHeightMap)
{
	UINT nMeshSidePointNum = pHeightMap->GetHeightMapWidth();
	float fTextureCoordSize = 1;
	float fDotDistant = fWidth/(nMeshSidePointNum);

	float fMinHeight = +FLT_MAX,fMaxHeight = -FLT_MAX;

	MeshData meshData;

	//다음 Index 정점을 통하여 삼각형을 만들어야 하기 때문에, -1된 값이 max값이 된다
	for (int x = 0; x<nMeshSidePointNum ; ++x) {
		for (int z = 0; z < nMeshSidePointNum ; ++z) {
			float fPosX = (x * fDotDistant) - (fWidth / 2);
			float fPosZ = (z * fDotDistant) - (fDepth / 2);
			float fPosY = pHeightMap->GetHeight(fPosX, fPosZ);

			if (fHeight < fMinHeight) fMinHeight = fHeight;
			if (fHeight > fMaxHeight) fMaxHeight = fHeight;

			meshData.m_vvPosition.push_back(XMFLOAT3(fPosX, fPosY, fPosZ));
		}
	}

	UINT idxIndex1 = 0;
	UINT save1 = 0;
	UINT idxIndex2 = 0;
	UINT save2 = 0;
	for (int x = 0; x<iSectorNum+1; ++x) {
		for (int z = 0; z < nMeshSidePointNum-1; ++z) {
			meshData.m_vIndex.push_back(idxIndex1);
			idxIndex1 += 1;
			meshData.m_vIndex.push_back(idxIndex1);

			meshData.m_vIndex.push_back(idxIndex2);
			idxIndex2 += nMeshSidePointNum;
			meshData.m_vIndex.push_back(idxIndex2);
		}
		save1 += (nMeshSidePointNum)*(nMeshSidePointNum / iSectorNum);
		idxIndex1 = save1;
		save2 += nMeshSidePointNum/iSectorNum;
		idxIndex2 = save2;
	}

	UINT idxBeg = meshData.m_vvPosition.size();
	UINT s = idxBeg;
	for (int x = 0; x<nMeshSidePointNum ; x+=static_cast<int>(nMeshSidePointNum/iSectorNum)) {
		for (int z = 0; z < nMeshSidePointNum ; z+=static_cast<int>(nMeshSidePointNum/iSectorNum)) {
			float fPosX = (x * fDotDistant) - (fWidth / 2);
			float fPosZ = (z * fDotDistant) - (fDepth / 2);
			float fPosY = 0;

			if (fHeight < fMinHeight) fMinHeight = fHeight;
			if (fHeight > fMaxHeight) fMaxHeight = fHeight;

			meshData.m_vvPosition.push_back(XMFLOAT3(fPosX, fPosY, fPosZ));
		}
	}

	for (int i = 0; i<iSectorNum+1 ; ++i) {
		meshData.m_vIndex.push_back(i+idxBeg);
		meshData.m_vIndex.push_back(i+idxBeg + (iSectorNum+1)*iSectorNum);
		meshData.m_vIndex.push_back(i*(iSectorNum+1) + idxBeg);
		meshData.m_vIndex.push_back(i*(iSectorNum+1) + iSectorNum + idxBeg);
	}

	idxBeg = meshData.m_vvPosition.size();
	for (int x = 0; x<nMeshSidePointNum ; x+=static_cast<int>(nMeshSidePointNum/iSectorNum)) {
		for (int z = 0; z < nMeshSidePointNum ; z+=static_cast<int>(nMeshSidePointNum/iSectorNum)) {
			float fPosX = (x * fDotDistant) - (fWidth / 2);
			float fPosZ = (z * fDotDistant) - (fDepth / 2);
			float fPosY = fHeight;

			if (fHeight < fMinHeight) fMinHeight = fHeight;
			if (fHeight > fMaxHeight) fMaxHeight = fHeight;

			meshData.m_vvPosition.push_back(XMFLOAT3(fPosX, fPosY, fPosZ));
		}
	}

	for (int i = 0; i<iSectorNum+1 ; ++i) {
		meshData.m_vIndex.push_back(i+idxBeg);
		meshData.m_vIndex.push_back(i+idxBeg + (iSectorNum+1)*iSectorNum);
		meshData.m_vIndex.push_back(i*(iSectorNum+1) + idxBeg);
		meshData.m_vIndex.push_back(i*(iSectorNum+1) + iSectorNum + idxBeg);
	}

	for (int i = 0; i<iSectorNum+1 ;++i) {
		for (int j = 0; j<iSectorNum+1; ++j) {
			meshData.m_vIndex.push_back((i * (iSectorNum+1)) + j + s);
			meshData.m_vIndex.push_back((i * (iSectorNum+1)) + ((iSectorNum+1)*(iSectorNum+1)) + j + s);
		}
	}

	meshData.m_AABB = BoundingBox(XMFLOAT3(0, (fMinHeight+fMaxHeight)/2, 0), XMFLOAT3(fWidth/2, (abs(fMinHeight)+abs(fMaxHeight))/2, fDepth/2));

	for (int i = 0; i < meshData.m_vvPosition.size(); i++) 
		meshData.m_vvColor.push_back(XMFLOAT4(RandomBetween(0.3,6), RandomBetween(0.3, 6), RandomBetween(0.3, 6), 1));
	
	m_PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	CreateMesh(pDevice, meshData);
}

CSquareMesh::CSquareMesh(ID3D11Device * pDevice,float fWidth,float fHeight,UINT element)
{
	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = 0;

	std::vector<XMFLOAT3> vvNormal;
	std::vector<XMFLOAT4> vvColor;
	std::vector<XMFLOAT2> vvTexCoord;

	if (element & EMeshElement::INDEX) {
		m_vvPosition.push_back(XMFLOAT3(-fx, +fy, -fz));	m_vvPosition.push_back(XMFLOAT3(+fx, +fy, +fz));
		m_vvPosition.push_back(XMFLOAT3(+fx, -fy, -fz));	m_vvPosition.push_back(XMFLOAT3(-fx, -fy, -fz));	
		
		vvTexCoord.push_back(XMFLOAT2(1, 0));	vvTexCoord.push_back(XMFLOAT2(0, 0));
		vvTexCoord.push_back(XMFLOAT2(0, 1));	vvTexCoord.push_back(XMFLOAT2(1, 1));

		m_vIndex.push_back(1); m_vIndex.push_back(0); m_vIndex.push_back(2); m_vIndex.push_back(3);

		if (element & EMeshElement::BOTH_SIDE) {
			m_vIndex.push_back(2); m_vIndex.push_back(0); m_vIndex.push_back(1);
		}
	}
	else {
		//FRONT
		m_vvPosition.push_back(XMFLOAT3(-fx, +fy, +fz));	m_vvPosition.push_back(XMFLOAT3(-fx, -fy, +fz));	m_vvPosition.push_back(XMFLOAT3(+fx, -fy, +fz));
		m_vvPosition.push_back(XMFLOAT3(-fx, +fy, +fz));	m_vvPosition.push_back(XMFLOAT3(+fx, -fy, +fz));	m_vvPosition.push_back(XMFLOAT3(+fx, +fy, +fz));

		if (element & EMeshElement::BOTH_SIDE) {

		}

		vvTexCoord.push_back(XMFLOAT2(1, 0));	vvTexCoord.push_back(XMFLOAT2(1, 1));	vvTexCoord.push_back(XMFLOAT2(0, 1));
		vvTexCoord.push_back(XMFLOAT2(1, 0));	vvTexCoord.push_back(XMFLOAT2(0, 1));	vvTexCoord.push_back(XMFLOAT2(0, 0));
	}

	if(element & LOOK_UP){
		XMMATRIX mtx = XMMatrixRotationAxis(XMVECTOR{1, 0, 0, 0}, XMConvertToRadians(-90));

		for(int i=0 ; i<m_vvPosition.size() ; ++i){
			XMVECTOR vecPos = XMLoadFloat3(&m_vvPosition[i]);
			vecPos = XMVector3TransformCoord(vecPos, mtx);
			XMStoreFloat3(&m_vvPosition[i], vecPos);
		}
		fz = fy;
		fy = 0;
	}

	if(element & LOOK_UP)
		for (int i = 0; i < m_vvPosition.size(); i++) vvNormal.push_back(XMFLOAT3(0, 1, 0));
	else
		for (int i = 0; i < m_vvPosition.size(); i++) vvNormal.push_back(XMFLOAT3(0, 0, 1));

	for (int i = 0; i < m_vvPosition.size(); i++)
		vvColor.push_back(XMFLOAT4(rand() % 255 / 255.0f, rand() % 255 / 255.0f, rand() % 255 / 255.0f, 1)); // + RANDOM_COLOR;
	
	if (element & EMeshElement::INDEX)		//Index & default triangle
		m_PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	else									//no Index & default triangle
		m_PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	AddVertexBuffer(pDevice, m_vvPosition, EVertexElement::POSITION);
	AddVertexBuffer(pDevice, vvColor, EVertexElement::COLOR);
	AddVertexBuffer(pDevice, vvTexCoord, EVertexElement::TEXCOORD1);
	AddVertexBuffer(pDevice, vvNormal, EVertexElement::NORMAL);
	SetIndexBuffer(pDevice, m_vIndex);

	m_AABB = BoundingBox(XMFLOAT3(0, 0, 0), XMFLOAT3(fx, fy, fz));
}

CWarpSquareMesh::CWarpSquareMesh(ID3D11Device * pDevice,float fWidth,float fHeight, UINT uSplit,UINT element)
{
	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = 0;

	std::vector<XMFLOAT3> vvNormal;
	std::vector<XMFLOAT4> vvColor;
	std::vector<XMFLOAT2> vvTexCoord;

	if (element & EMeshElement::INDEX) {
		m_vvPosition.push_back(XMFLOAT3(-fx, +fy, -fz));	m_vvPosition.push_back(XMFLOAT3(+fx, +fy, +fz));
		m_vvPosition.push_back(XMFLOAT3(+fx, -fy, -fz));	m_vvPosition.push_back(XMFLOAT3(-fx, -fy, -fz));	
		
		vvTexCoord.push_back(XMFLOAT2(uSplit, 0));	vvTexCoord.push_back(XMFLOAT2(0, 0));
		vvTexCoord.push_back(XMFLOAT2(0, uSplit));	vvTexCoord.push_back(XMFLOAT2(uSplit, uSplit));

		m_vIndex.push_back(1); m_vIndex.push_back(0); m_vIndex.push_back(2); m_vIndex.push_back(3);

		if (element & EMeshElement::BOTH_SIDE) {
			m_vIndex.push_back(2); m_vIndex.push_back(0); m_vIndex.push_back(1);
		}
	}
	else {
		//FRONT
		m_vvPosition.push_back(XMFLOAT3(-fx, +fy, +fz));	m_vvPosition.push_back(XMFLOAT3(-fx, -fy, +fz));	m_vvPosition.push_back(XMFLOAT3(+fx, -fy, +fz));
		m_vvPosition.push_back(XMFLOAT3(-fx, +fy, +fz));	m_vvPosition.push_back(XMFLOAT3(+fx, -fy, +fz));	m_vvPosition.push_back(XMFLOAT3(+fx, +fy, +fz));

		if (element & EMeshElement::BOTH_SIDE) {

		}

		vvTexCoord.push_back(XMFLOAT2(1, 0));	vvTexCoord.push_back(XMFLOAT2(1, 1));	vvTexCoord.push_back(XMFLOAT2(0, 1));
		vvTexCoord.push_back(XMFLOAT2(1, 0));	vvTexCoord.push_back(XMFLOAT2(0, 1));	vvTexCoord.push_back(XMFLOAT2(0, 0));
	}

	if(element & LOOK_UP){
		XMMATRIX mtx = XMMatrixRotationAxis(XMVECTOR{1, 0, 0, 0}, XMConvertToRadians(-90));

		for(int i=0 ; i<m_vvPosition.size() ; ++i){
			XMVECTOR vecPos = XMLoadFloat3(&m_vvPosition[i]);
			vecPos = XMVector3TransformCoord(vecPos, mtx);
			XMStoreFloat3(&m_vvPosition[i], vecPos);
		}
		fz = fy;
		fy = 0;
	}

	if(element & LOOK_UP)
		for (int i = 0; i < m_vvPosition.size(); i++) vvNormal.push_back(XMFLOAT3(0, 1, 0));
	else
		for (int i = 0; i < m_vvPosition.size(); i++) vvNormal.push_back(XMFLOAT3(0, 0, 1));

	for (int i = 0; i < m_vvPosition.size(); i++)
		vvColor.push_back(XMFLOAT4(rand() % 255 / 255.0f, rand() % 255 / 255.0f, rand() % 255 / 255.0f, 1)); // + RANDOM_COLOR;
	
	if (element & EMeshElement::INDEX)		//Index & default triangle
		m_PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	else									//no Index & default triangle
		m_PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	AddVertexBuffer(pDevice, m_vvPosition, EVertexElement::POSITION);
	AddVertexBuffer(pDevice, vvColor, EVertexElement::COLOR);
	AddVertexBuffer(pDevice, vvTexCoord, EVertexElement::TEXCOORD1);
	AddVertexBuffer(pDevice, vvNormal, EVertexElement::NORMAL);
	SetIndexBuffer(pDevice, m_vIndex);

	m_AABB = BoundingBox(XMFLOAT3(0, 0, 0), XMFLOAT3(fx, fy, fz));
}


CCubeMesh::CCubeMesh(ID3D11Device * pDevice,float fWidth,float fHeight,float fDepth,UINT element)
{
	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	MeshData meshData;
	
	if (element & EMeshElement::INDEX) {
		meshData.m_vvPosition.push_back(XMFLOAT3(-fx, +fy, -fz));
		meshData.m_vvPosition.push_back(XMFLOAT3(+fx, +fy, -fz));
		meshData.m_vvPosition.push_back(XMFLOAT3(+fx, +fy, +fz));
		meshData.m_vvPosition.push_back(XMFLOAT3(-fx, +fy, +fz));
		meshData.m_vvPosition.push_back(XMFLOAT3(-fx, -fy, -fz));
		meshData.m_vvPosition.push_back(XMFLOAT3(+fx, -fy, -fz));
		meshData.m_vvPosition.push_back(XMFLOAT3(+fx, -fy, +fz));
		meshData.m_vvPosition.push_back(XMFLOAT3(-fx, -fy, +fz));

		meshData.m_vvColor.push_back(XMFLOAT4(0,1,1,1));
		meshData.m_vvColor.push_back(XMFLOAT4(1,1,1,1));
		meshData.m_vvColor.push_back(XMFLOAT4(1,0,1,1));
		meshData.m_vvColor.push_back(XMFLOAT4(0,0,1,1));

		meshData.m_vvColor.push_back(XMFLOAT4(0,1,0,1)); 
		meshData.m_vvColor.push_back(XMFLOAT4(1,1,0,1));
		meshData.m_vvColor.push_back(XMFLOAT4(1,0,0,1));
		meshData.m_vvColor.push_back(XMFLOAT4(0,0,0,1));

		meshData.m_vIndex.push_back(5); //5,6,4 - cw
		meshData.m_vIndex.push_back(6); //6,4,7 - ccw
		meshData.m_vIndex.push_back(4); //4,7,0 - cw
		meshData.m_vIndex.push_back(7); //7,0,3 - ccw
		meshData.m_vIndex.push_back(0); //0,3,1 - cw
		meshData.m_vIndex.push_back(3); //3,1,2 - ccw
		meshData.m_vIndex.push_back(1); //1,2,3 - cw 
		meshData.m_vIndex.push_back(2); //2,3,7 - ccw
		meshData.m_vIndex.push_back(3); //3,7,2 - cw - Degenerated Index
		meshData.m_vIndex.push_back(7); //7,2,6 - ccw
		meshData.m_vIndex.push_back(2); //2,6,1 - cw
		meshData.m_vIndex.push_back(6); //6,1,5 - ccw
		meshData.m_vIndex.push_back(1); //1,5,0 - cw
		meshData.m_vIndex.push_back(5); //5,0,4 - ccw
		meshData.m_vIndex.push_back(0);
		meshData.m_vIndex.push_back(4);

		meshData.m_vvTexCoord1.push_back(XMFLOAT2(0.25f, 0.3333f));	
		meshData.m_vvTexCoord1.push_back(XMFLOAT2(0.25f, 0.6666f));	
		meshData.m_vvTexCoord1.push_back(XMFLOAT2(0.50f, 0.6666f));	
		meshData.m_vvTexCoord1.push_back(XMFLOAT2(0.50f, 0.3333f));
		meshData.m_vvTexCoord1.push_back(XMFLOAT2(0, 0.3333f));
		meshData.m_vvTexCoord1.push_back(XMFLOAT2(0, 0.6666f));
		meshData.m_vvTexCoord1.push_back(XMFLOAT2(0.75f, 0));
		meshData.m_vvTexCoord1.push_back(XMFLOAT2(0.75f, 0));
	}
	else {
		//if(element & MESH_ELEMENT::TESSELLATION){	//테셀레이션인 경우
		//	//BACK
		//	m_vvPosition.push_back(XMFLOAT3(-fx, +fy, -fz));		m_vvPosition.push_back(XMFLOAT3(+fx, +fy, -fz));		m_vvPosition.push_back(XMFLOAT3(-fx, -fy, -fz));		m_vvPosition.push_back(XMFLOAT3(+fx, -fy, -fz));	
		//	//RIGNT
		//	m_vvPosition.push_back(XMFLOAT3(+fx, +fy, -fz));		m_vvPosition.push_back(XMFLOAT3(+fx, +fy, +fz));		m_vvPosition.push_back(XMFLOAT3(+fx, -fy, -fz));		m_vvPosition.push_back(XMFLOAT3(+fx, -fy, +fz));	
		//	//FRONT
		//	m_vvPosition.push_back(XMFLOAT3(+fx, +fy, +fz));		m_vvPosition.push_back(XMFLOAT3(-fx, +fy, +fz));		m_vvPosition.push_back(XMFLOAT3(+fx, -fy, +fz));		m_vvPosition.push_back(XMFLOAT3(-fx, -fy, +fz));
		//	//LEFT
		//	m_vvPosition.push_back(XMFLOAT3(-fx, +fy, +fz));		m_vvPosition.push_back(XMFLOAT3(-fx, +fy, -fz));		m_vvPosition.push_back(XMFLOAT3(-fx, -fy, +fz));		m_vvPosition.push_back(XMFLOAT3(-fx, -fy, -fz));		
		//	//TOP
		//	m_vvPosition.push_back(XMFLOAT3(-fx, +fy, +fz));		m_vvPosition.push_back(XMFLOAT3(+fx, +fy, +fz));		m_vvPosition.push_back(XMFLOAT3(-fx, +fy, -fz));		m_vvPosition.push_back(XMFLOAT3(+fx, +fy, -fz));		
		//	//BOTTOM
		//	m_vvPosition.push_back(XMFLOAT3(-fx, -fy, -fz));		m_vvPosition.push_back(XMFLOAT3(+fx, -fy, -fz));		m_vvPosition.push_back(XMFLOAT3(-fx, -fy, +fz));		m_vvPosition.push_back(XMFLOAT3(+fx, -fy, +fz));		
		//	

		//	if (element & MESH_ELEMENT::TEXCOORD1) {
		//		vvTexCoord.push_back(XMFLOAT2(0, 0));		vvTexCoord.push_back(XMFLOAT2(1, 0));		vvTexCoord.push_back(XMFLOAT2(0, 1));		vvTexCoord.push_back(XMFLOAT2(1, 1));
		//	
		//		vvTexCoord.push_back(XMFLOAT2(0, 0));		vvTexCoord.push_back(XMFLOAT2(1, 0));		vvTexCoord.push_back(XMFLOAT2(0, 1));		vvTexCoord.push_back(XMFLOAT2(1, 1));
		//	
		//		vvTexCoord.push_back(XMFLOAT2(0, 0));		vvTexCoord.push_back(XMFLOAT2(1, 0));		vvTexCoord.push_back(XMFLOAT2(0, 1));		vvTexCoord.push_back(XMFLOAT2(1, 1));
		//	
		//		vvTexCoord.push_back(XMFLOAT2(0, 0));		vvTexCoord.push_back(XMFLOAT2(1, 0));		vvTexCoord.push_back(XMFLOAT2(0, 1));		vvTexCoord.push_back(XMFLOAT2(1, 1));		
		//	
		//		vvTexCoord.push_back(XMFLOAT2(0, 0));		vvTexCoord.push_back(XMFLOAT2(1, 0));		vvTexCoord.push_back(XMFLOAT2(0, 1));		vvTexCoord.push_back(XMFLOAT2(1, 1));

		//		vvTexCoord.push_back(XMFLOAT2(0, 0));		vvTexCoord.push_back(XMFLOAT2(1, 0));		vvTexCoord.push_back(XMFLOAT2(0, 1));		vvTexCoord.push_back(XMFLOAT2(1, 1));
		//	}
		//}

		//BACK
		meshData.m_vvPosition.push_back(XMFLOAT3(+fx, +fy, -fz));		meshData.m_vvPosition.push_back(XMFLOAT3(+fx, -fy, -fz));		meshData.m_vvPosition.push_back(XMFLOAT3(-fx, -fy, -fz));
		meshData.m_vvPosition.push_back(XMFLOAT3(+fx, +fy, -fz));		meshData.m_vvPosition.push_back(XMFLOAT3(-fx, -fy, -fz));		meshData.m_vvPosition.push_back(XMFLOAT3(-fx, +fy, -fz));
		//RIGNT
		meshData.m_vvPosition.push_back(XMFLOAT3(+fx, +fy, +fz));		meshData.m_vvPosition.push_back(XMFLOAT3(+fx, -fy, +fz));		meshData.m_vvPosition.push_back(XMFLOAT3(+fx, -fy, -fz));
		meshData.m_vvPosition.push_back(XMFLOAT3(+fx, +fy, +fz));		meshData.m_vvPosition.push_back(XMFLOAT3(+fx, -fy, -fz));		meshData.m_vvPosition.push_back(XMFLOAT3(+fx, +fy, -fz));
		//FRONT
		meshData.m_vvPosition.push_back(XMFLOAT3(-fx, +fy, +fz));		meshData.m_vvPosition.push_back(XMFLOAT3(-fx, -fy, +fz));		meshData.m_vvPosition.push_back(XMFLOAT3(+fx, -fy, +fz));
		meshData.m_vvPosition.push_back(XMFLOAT3(-fx, +fy, +fz));		meshData.m_vvPosition.push_back(XMFLOAT3(+fx, -fy, +fz));		meshData.m_vvPosition.push_back(XMFLOAT3(+fx, +fy, +fz));
		//LEFT
		meshData.m_vvPosition.push_back(XMFLOAT3(-fx, +fy, -fz));		meshData.m_vvPosition.push_back(XMFLOAT3(-fx, -fy, -fz));		meshData.m_vvPosition.push_back(XMFLOAT3(-fx, -fy, +fz));
		meshData.m_vvPosition.push_back(XMFLOAT3(-fx, +fy, -fz));		meshData.m_vvPosition.push_back(XMFLOAT3(-fx, -fy, +fz));		meshData.m_vvPosition.push_back(XMFLOAT3(-fx, +fy, +fz));
		//TOP
		meshData.m_vvPosition.push_back(XMFLOAT3(-fx, +fy, -fz));		meshData.m_vvPosition.push_back(XMFLOAT3(-fx, +fy, +fz));		meshData.m_vvPosition.push_back(XMFLOAT3(+fx, +fy, +fz));
		meshData.m_vvPosition.push_back(XMFLOAT3(-fx, +fy, -fz));		meshData.m_vvPosition.push_back(XMFLOAT3(+fx, +fy, +fz));		meshData.m_vvPosition.push_back(XMFLOAT3(+fx, +fy, -fz));
		//BOTTOM
		meshData.m_vvPosition.push_back(XMFLOAT3(-fx, -fy, +fz));		meshData.m_vvPosition.push_back(XMFLOAT3(-fx, -fy, -fz));		meshData.m_vvPosition.push_back(XMFLOAT3(+fx, -fy, -fz));
		meshData.m_vvPosition.push_back(XMFLOAT3(-fx, -fy, +fz));		meshData.m_vvPosition.push_back(XMFLOAT3(+fx, -fy, -fz));		meshData.m_vvPosition.push_back(XMFLOAT3(+fx, -fy, +fz));

		for (int i = 0; i < m_vvPosition.size(); i++) 
			meshData.m_vvColor.push_back(XMFLOAT4(rand() % 255 / 255.0f, rand() % 255 / 255.0f, rand() % 255 / 255.0f, 1)); // + RANDOM_COLOR;

		meshData.m_vvTexCoord1.push_back(XMFLOAT2(1, 0));			meshData.m_vvTexCoord1.push_back(XMFLOAT2(1, 1));			meshData.m_vvTexCoord1.push_back(XMFLOAT2(0, 1));
		meshData.m_vvTexCoord1.push_back(XMFLOAT2(1, 0));			meshData.m_vvTexCoord1.push_back(XMFLOAT2(0, 1));			meshData.m_vvTexCoord1.push_back(XMFLOAT2(0, 0));
																																
		meshData.m_vvTexCoord1.push_back(XMFLOAT2(1, 0));			meshData.m_vvTexCoord1.push_back(XMFLOAT2(1, 1));			meshData.m_vvTexCoord1.push_back(XMFLOAT2(0, 1));
		meshData.m_vvTexCoord1.push_back(XMFLOAT2(1, 0));			meshData.m_vvTexCoord1.push_back(XMFLOAT2(0, 1));			meshData.m_vvTexCoord1.push_back(XMFLOAT2(0, 0));
																																
		meshData.m_vvTexCoord1.push_back(XMFLOAT2(1, 0));			meshData.m_vvTexCoord1.push_back(XMFLOAT2(1, 1));			meshData.m_vvTexCoord1.push_back(XMFLOAT2(0, 1));
		meshData.m_vvTexCoord1.push_back(XMFLOAT2(1, 0));			meshData.m_vvTexCoord1.push_back(XMFLOAT2(0, 1));			meshData.m_vvTexCoord1.push_back(XMFLOAT2(0, 0));
																																
		meshData.m_vvTexCoord1.push_back(XMFLOAT2(1, 0));			meshData.m_vvTexCoord1.push_back(XMFLOAT2(1, 1));			meshData.m_vvTexCoord1.push_back(XMFLOAT2(0, 1));
		meshData.m_vvTexCoord1.push_back(XMFLOAT2(1, 0));			meshData.m_vvTexCoord1.push_back(XMFLOAT2(0, 1));			meshData.m_vvTexCoord1.push_back(XMFLOAT2(0, 0));
																																
		meshData.m_vvTexCoord1.push_back(XMFLOAT2(0, 1));			meshData.m_vvTexCoord1.push_back(XMFLOAT2(0, 0));			meshData.m_vvTexCoord1.push_back(XMFLOAT2(1, 0));
		meshData.m_vvTexCoord1.push_back(XMFLOAT2(0, 1));			meshData.m_vvTexCoord1.push_back(XMFLOAT2(1, 0));			meshData.m_vvTexCoord1.push_back(XMFLOAT2(1, 1));
																																
		meshData.m_vvTexCoord1.push_back(XMFLOAT2(1, 0));			meshData.m_vvTexCoord1.push_back(XMFLOAT2(1, 1));			meshData.m_vvTexCoord1.push_back(XMFLOAT2(0, 1));
		meshData.m_vvTexCoord1.push_back(XMFLOAT2(1, 0));			meshData.m_vvTexCoord1.push_back(XMFLOAT2(0, 1));			meshData.m_vvTexCoord1.push_back(XMFLOAT2(0, 0));
		
		meshData.m_vvNormal.push_back(XMFLOAT3(+0, +0, -1));		meshData.m_vvNormal.push_back(XMFLOAT3(+0, +0, -1));		meshData.m_vvNormal.push_back(XMFLOAT3(+0, +0, -1));
		meshData.m_vvNormal.push_back(XMFLOAT3(+0, +0, -1));		meshData.m_vvNormal.push_back(XMFLOAT3(+0, +0, -1));		meshData.m_vvNormal.push_back(XMFLOAT3(+0, +0, -1));
																																
		meshData.m_vvNormal.push_back(XMFLOAT3(+1, +0, +0));		meshData.m_vvNormal.push_back(XMFLOAT3(+1, +0, +0));		meshData.m_vvNormal.push_back(XMFLOAT3(+1, +0, +0));
		meshData.m_vvNormal.push_back(XMFLOAT3(+1, +0, +0));		meshData.m_vvNormal.push_back(XMFLOAT3(+1, +0, +0));		meshData.m_vvNormal.push_back(XMFLOAT3(+1, +0, +0));
																																
		meshData.m_vvNormal.push_back(XMFLOAT3(+0, +0, +1));		meshData.m_vvNormal.push_back(XMFLOAT3(+0, +0, +1));		meshData.m_vvNormal.push_back(XMFLOAT3(+0, +0, +1));
		meshData.m_vvNormal.push_back(XMFLOAT3(+0, +0, +1));		meshData.m_vvNormal.push_back(XMFLOAT3(+0, +0, +1));		meshData.m_vvNormal.push_back(XMFLOAT3(+0, +0, +1));
																																
		meshData.m_vvNormal.push_back(XMFLOAT3(-1, +0, +0));		meshData.m_vvNormal.push_back(XMFLOAT3(-1, +0, +0));		meshData.m_vvNormal.push_back(XMFLOAT3(-1, +0, +0));
		meshData.m_vvNormal.push_back(XMFLOAT3(-1, +0, +0));		meshData.m_vvNormal.push_back(XMFLOAT3(-1, +0, +0));		meshData.m_vvNormal.push_back(XMFLOAT3(-1, +0, +0));
																																
		meshData.m_vvNormal.push_back(XMFLOAT3(+0, +1, +0));		meshData.m_vvNormal.push_back(XMFLOAT3(+0, +1, +0));		meshData.m_vvNormal.push_back(XMFLOAT3(+0, +1, +0));
		meshData.m_vvNormal.push_back(XMFLOAT3(+0, +1, +0));		meshData.m_vvNormal.push_back(XMFLOAT3(+0, +1, +0));		meshData.m_vvNormal.push_back(XMFLOAT3(+0, +1, +0));
																																
		meshData.m_vvNormal.push_back(XMFLOAT3(+0, -1, +0));		meshData.m_vvNormal.push_back(XMFLOAT3(+0, -1, +0));		meshData.m_vvNormal.push_back(XMFLOAT3(+0, -1, +0));
		meshData.m_vvNormal.push_back(XMFLOAT3(+0, -1, +0));		meshData.m_vvNormal.push_back(XMFLOAT3(+0, -1, +0));		meshData.m_vvNormal.push_back(XMFLOAT3(+0, -1, +0));
	}

	meshData.m_AABB = BoundingBox(XMFLOAT3(0, 0, 0), XMFLOAT3(fx, fy, fz));

	if (element & EMeshElement::INDEX)		//Index & default triangle
		m_PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	else									//no Index & default triangle
		m_PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	CreateTBN(meshData);
	CreateMesh(pDevice, meshData);
}

CCrossBillboardMesh::CCrossBillboardMesh(ID3D11Device * pDevice, float fWidth, float fHeight, float fDepth, UINT element)
{
	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	std::vector<XMFLOAT3> vvNormal;
	std::vector<XMFLOAT2> vvTexCoord;
	std::vector<XMFLOAT4> vvColor;

	//BACK
	m_vvPosition.push_back(XMFLOAT3(+fx, +fy, -0));		m_vvPosition.push_back(XMFLOAT3(+fx, -fy, -0));		m_vvPosition.push_back(XMFLOAT3(-fx, -fy, -0));
	m_vvPosition.push_back(XMFLOAT3(+fx, +fy, -0));		m_vvPosition.push_back(XMFLOAT3(-fx, -fy, -0));		m_vvPosition.push_back(XMFLOAT3(-fx, +fy, -0));
	//RIGNT
	m_vvPosition.push_back(XMFLOAT3(+0, +fy, +fz));		m_vvPosition.push_back(XMFLOAT3(+0, -fy, +fz));		m_vvPosition.push_back(XMFLOAT3(+0, -fy, -fz));
	m_vvPosition.push_back(XMFLOAT3(+0, +fy, +fz));		m_vvPosition.push_back(XMFLOAT3(+0, -fy, -fz));		m_vvPosition.push_back(XMFLOAT3(+0, +fy, -fz));
	//FRONT
	m_vvPosition.push_back(XMFLOAT3(-fx, +fy, +0));		m_vvPosition.push_back(XMFLOAT3(-fx, -fy, +0));		m_vvPosition.push_back(XMFLOAT3(+fx, -fy, +0));
	m_vvPosition.push_back(XMFLOAT3(-fx, +fy, +0));		m_vvPosition.push_back(XMFLOAT3(+fx, -fy, +0));		m_vvPosition.push_back(XMFLOAT3(+fx, +fy, +0));
	//LEFT
	m_vvPosition.push_back(XMFLOAT3(-0, +fy, -fz));		m_vvPosition.push_back(XMFLOAT3(-0, -fy, -fz));		m_vvPosition.push_back(XMFLOAT3(-0, -fy, +fz));
	m_vvPosition.push_back(XMFLOAT3(-0, +fy, -fz));		m_vvPosition.push_back(XMFLOAT3(-0, -fy, +fz));		m_vvPosition.push_back(XMFLOAT3(-0, +fy, +fz));
	////TOP
	//m_vvPosition.push_back(XMFLOAT3(-fx, +fy, -fz));		m_vvPosition.push_back(XMFLOAT3(-fx, +fy, +fz));		m_vvPosition.push_back(XMFLOAT3(+fx, +fy, +fz));
	//m_vvPosition.push_back(XMFLOAT3(-fx, +fy, -fz));		m_vvPosition.push_back(XMFLOAT3(+fx, +fy, +fz));		m_vvPosition.push_back(XMFLOAT3(+fx, +fy, -fz));
	////BOTTOM
	//m_vvPosition.push_back(XMFLOAT3(-fx, -fy, +fz));		m_vvPosition.push_back(XMFLOAT3(-fx, -fy, -fz));		m_vvPosition.push_back(XMFLOAT3(+fx, -fy, -fz));
	//m_vvPosition.push_back(XMFLOAT3(-fx, -fy, +fz));		m_vvPosition.push_back(XMFLOAT3(+fx, -fy, -fz));		m_vvPosition.push_back(XMFLOAT3(+fx, -fy, +fz));

	for (int i = 0; i < m_vvPosition.size(); i++)
		vvColor.push_back(XMFLOAT4(rand() % 255 / 255.0f, rand() % 255 / 255.0f, rand() % 255 / 255.0f, 1)); // + RANDOM_COLOR;

	vvTexCoord.push_back(XMFLOAT2(1, 0));		vvTexCoord.push_back(XMFLOAT2(1, 1));		vvTexCoord.push_back(XMFLOAT2(0, 1));
	vvTexCoord.push_back(XMFLOAT2(1, 0));		vvTexCoord.push_back(XMFLOAT2(0, 1));		vvTexCoord.push_back(XMFLOAT2(0, 0));

	vvTexCoord.push_back(XMFLOAT2(1, 0));		vvTexCoord.push_back(XMFLOAT2(1, 1));		vvTexCoord.push_back(XMFLOAT2(0, 1));
	vvTexCoord.push_back(XMFLOAT2(1, 0));		vvTexCoord.push_back(XMFLOAT2(0, 1));		vvTexCoord.push_back(XMFLOAT2(0, 0));

	vvTexCoord.push_back(XMFLOAT2(1, 0));		vvTexCoord.push_back(XMFLOAT2(1, 1));		vvTexCoord.push_back(XMFLOAT2(0, 1));
	vvTexCoord.push_back(XMFLOAT2(1, 0));		vvTexCoord.push_back(XMFLOAT2(0, 1));		vvTexCoord.push_back(XMFLOAT2(0, 0));

	vvTexCoord.push_back(XMFLOAT2(1, 0));		vvTexCoord.push_back(XMFLOAT2(1, 1));		vvTexCoord.push_back(XMFLOAT2(0, 1));
	vvTexCoord.push_back(XMFLOAT2(1, 0));		vvTexCoord.push_back(XMFLOAT2(0, 1));		vvTexCoord.push_back(XMFLOAT2(0, 0));

	//vvTexCoord.push_back(XMFLOAT2(0, 1));		vvTexCoord.push_back(XMFLOAT2(0, 0));		vvTexCoord.push_back(XMFLOAT2(1, 0));
	//vvTexCoord.push_back(XMFLOAT2(0, 1));		vvTexCoord.push_back(XMFLOAT2(1, 0));		vvTexCoord.push_back(XMFLOAT2(1, 1));

	//vvTexCoord.push_back(XMFLOAT2(1, 0));		vvTexCoord.push_back(XMFLOAT2(1, 1));		vvTexCoord.push_back(XMFLOAT2(0, 1));
	//vvTexCoord.push_back(XMFLOAT2(1, 0));		vvTexCoord.push_back(XMFLOAT2(0, 1));		vvTexCoord.push_back(XMFLOAT2(0, 0));

	vvNormal.push_back(XMFLOAT3(+0, +0, -1));		vvNormal.push_back(XMFLOAT3(+0, +0, -1));		vvNormal.push_back(XMFLOAT3(+0, +0, -1));
	vvNormal.push_back(XMFLOAT3(+0, +0, -1));		vvNormal.push_back(XMFLOAT3(+0, +0, -1));		vvNormal.push_back(XMFLOAT3(+0, +0, -1));

	vvNormal.push_back(XMFLOAT3(+1, +0, +0));		vvNormal.push_back(XMFLOAT3(+1, +0, +0));		vvNormal.push_back(XMFLOAT3(+1, +0, +0));
	vvNormal.push_back(XMFLOAT3(+1, +0, +0));		vvNormal.push_back(XMFLOAT3(+1, +0, +0));		vvNormal.push_back(XMFLOAT3(+1, +0, +0));

	vvNormal.push_back(XMFLOAT3(+0, +0, +1));		vvNormal.push_back(XMFLOAT3(+0, +0, +1));		vvNormal.push_back(XMFLOAT3(+0, +0, +1));
	vvNormal.push_back(XMFLOAT3(+0, +0, +1));		vvNormal.push_back(XMFLOAT3(+0, +0, +1));		vvNormal.push_back(XMFLOAT3(+0, +0, +1));

	vvNormal.push_back(XMFLOAT3(-1, +0, +0));		vvNormal.push_back(XMFLOAT3(-1, +0, +0));		vvNormal.push_back(XMFLOAT3(-1, +0, +0));
	vvNormal.push_back(XMFLOAT3(-1, +0, +0));		vvNormal.push_back(XMFLOAT3(-1, +0, +0));		vvNormal.push_back(XMFLOAT3(-1, +0, +0));

	//vvNormal.push_back(XMFLOAT3(+0, +1, +0));		vvNormal.push_back(XMFLOAT3(+0, +1, +0));		vvNormal.push_back(XMFLOAT3(+0, +1, +0));
	//vvNormal.push_back(XMFLOAT3(+0, +1, +0));		vvNormal.push_back(XMFLOAT3(+0, +1, +0));		vvNormal.push_back(XMFLOAT3(+0, +1, +0));

	//vvNormal.push_back(XMFLOAT3(+0, -1, +0));		vvNormal.push_back(XMFLOAT3(+0, -1, +0));		vvNormal.push_back(XMFLOAT3(+0, -1, +0));
	//vvNormal.push_back(XMFLOAT3(+0, -1, +0));		vvNormal.push_back(XMFLOAT3(+0, -1, +0));		vvNormal.push_back(XMFLOAT3(+0, -1, +0));

	m_PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	AddVertexBuffer(pDevice, m_vvPosition, EVertexElement::POSITION);
	AddVertexBuffer(pDevice, vvColor, EVertexElement::COLOR);
	AddVertexBuffer(pDevice, vvTexCoord, EVertexElement::TEXCOORD1);
	AddVertexBuffer(pDevice, vvNormal, EVertexElement::NORMAL);
	SetIndexBuffer(pDevice, m_vIndex);

	m_AABB = BoundingBox(XMFLOAT3(0, 0, 0), XMFLOAT3(fx, fy, fz));
}

CSkyboxMesh::CSkyboxMesh(ID3D11Device * pDevice, float fSizeX, float fSizeY, float fSizeZ) : CMesh() {
	float fx = fSizeX * 0.5f;
	float fy = fSizeY * 0.5f;
	float fz = fSizeZ * 0.5f;

	std::vector<XMFLOAT2> vvTexCoord;
	std::vector<XMFLOAT4> vvColor;

	//BACK
	m_vvPosition.push_back(XMFLOAT3(+fx, -fy, -fz));		m_vvPosition.push_back(XMFLOAT3(+fx, +fy, -fz));		m_vvPosition.push_back(XMFLOAT3(-fx, -fy, -fz));
	m_vvPosition.push_back(XMFLOAT3(-fx, -fy, -fz));		m_vvPosition.push_back(XMFLOAT3(+fx, +fy, -fz));		m_vvPosition.push_back(XMFLOAT3(-fx, +fy, -fz));
	//RIGNT
	m_vvPosition.push_back(XMFLOAT3(+fx, -fy, +fz));		m_vvPosition.push_back(XMFLOAT3(+fx, +fy, +fz));		m_vvPosition.push_back(XMFLOAT3(+fx, -fy, -fz));
	m_vvPosition.push_back(XMFLOAT3(+fx, -fy, -fz));		m_vvPosition.push_back(XMFLOAT3(+fx, +fy, +fz));		m_vvPosition.push_back(XMFLOAT3(+fx, +fy, -fz));
	//FRONT
	m_vvPosition.push_back(XMFLOAT3(-fx, -fy, +fz));		m_vvPosition.push_back(XMFLOAT3(-fx, +fy, +fz));		m_vvPosition.push_back(XMFLOAT3(+fx, -fy, +fz));
	m_vvPosition.push_back(XMFLOAT3(+fx, -fy, +fz));		m_vvPosition.push_back(XMFLOAT3(-fx, +fy, +fz));		m_vvPosition.push_back(XMFLOAT3(+fx, +fy, +fz));
	//LEFT
	m_vvPosition.push_back(XMFLOAT3(-fx, -fy, -fz));		m_vvPosition.push_back(XMFLOAT3(-fx, +fy, -fz));		m_vvPosition.push_back(XMFLOAT3(-fx, -fy, +fz));
	m_vvPosition.push_back(XMFLOAT3(-fx, -fy, +fz));		m_vvPosition.push_back(XMFLOAT3(-fx, +fy, -fz));		m_vvPosition.push_back(XMFLOAT3(-fx, +fy, +fz));
	//TOP
	m_vvPosition.push_back(XMFLOAT3(-fx, +fy, +fz));		m_vvPosition.push_back(XMFLOAT3(-fx, +fy, -fz));		m_vvPosition.push_back(XMFLOAT3(+fx, +fy, +fz));
	m_vvPosition.push_back(XMFLOAT3(+fx, +fy, +fz));		m_vvPosition.push_back(XMFLOAT3(-fx, +fy, -fz));		m_vvPosition.push_back(XMFLOAT3(+fx, +fy, -fz));
	//BOTTOM
	m_vvPosition.push_back(XMFLOAT3(-fx, -fy, -fz));		m_vvPosition.push_back(XMFLOAT3(-fx, -fy, +fz));		m_vvPosition.push_back(XMFLOAT3(+fx, -fy, -fz));
	m_vvPosition.push_back(XMFLOAT3(+fx, -fy, -fz));		m_vvPosition.push_back(XMFLOAT3(-fx, -fy, +fz));		m_vvPosition.push_back(XMFLOAT3(+fx, -fy, +fz));

	float fAmount = 0.0003;
	vvTexCoord.push_back(XMFLOAT2(0.75f, 2.0f / 3.0f - fAmount));		vvTexCoord.push_back(XMFLOAT2(0.75f, 1.0f / 3.0f + fAmount));		vvTexCoord.push_back(XMFLOAT2(1.0f, 2.0f / 3.0f - fAmount));
	vvTexCoord.push_back(XMFLOAT2(1.0f, 2.0f / 3.0f - fAmount));		vvTexCoord.push_back(XMFLOAT2(0.75f, 1.0f / 3.0f + fAmount));		vvTexCoord.push_back(XMFLOAT2(1.0f, 1.0f / 3.0f + fAmount));

	vvTexCoord.push_back(XMFLOAT2(0.5f, 2.0f / 3.0f - fAmount));		vvTexCoord.push_back(XMFLOAT2(0.5f, 1.0f / 3.0f + fAmount));		vvTexCoord.push_back(XMFLOAT2(0.75f, 2.0f / 3.0f - fAmount));
	vvTexCoord.push_back(XMFLOAT2(0.75f, 2.0f / 3.0f - fAmount));		vvTexCoord.push_back(XMFLOAT2(0.5f, 1.0f / 3.0f + fAmount));		vvTexCoord.push_back(XMFLOAT2(0.75f, 1.0f / 3.0f + fAmount));

	vvTexCoord.push_back(XMFLOAT2(0.25f, 2.0f / 3.0f));					vvTexCoord.push_back(XMFLOAT2(0.25f, 1.0f / 3.0f));					vvTexCoord.push_back(XMFLOAT2(0.5f, 2.0f / 3.0f));
	vvTexCoord.push_back(XMFLOAT2(0.5f, 2.0f / 3.0f));					vvTexCoord.push_back(XMFLOAT2(0.25f, 1.0f / 3.0f));					vvTexCoord.push_back(XMFLOAT2(0.5f, 1.0f / 3.0f));

	vvTexCoord.push_back(XMFLOAT2(0.0f, 2.0f / 3.0f - fAmount));		vvTexCoord.push_back(XMFLOAT2(0.0f, 1.0f / 3.0f + fAmount));		vvTexCoord.push_back(XMFLOAT2(0.25f, 2.0f / 3.0f - fAmount));
	vvTexCoord.push_back(XMFLOAT2(0.25f, 2.0f / 3.0f - fAmount));		vvTexCoord.push_back(XMFLOAT2(0.0f, 1.0f / 3.0f + fAmount));		vvTexCoord.push_back(XMFLOAT2(0.25f, 1.0f / 3.0f + fAmount));

	vvTexCoord.push_back(XMFLOAT2(0.25f + fAmount, 1.0f / 3.0f));		vvTexCoord.push_back(XMFLOAT2(0.25f + fAmount, 0.0f));				vvTexCoord.push_back(XMFLOAT2(0.5f - fAmount, 1.0f / 3.0f));
	vvTexCoord.push_back(XMFLOAT2(0.5f - fAmount, 1.0f / 3.0f));		vvTexCoord.push_back(XMFLOAT2(0.25f + fAmount, 0.0f));				vvTexCoord.push_back(XMFLOAT2(0.5f - fAmount, 0.0f));

	vvTexCoord.push_back(XMFLOAT2(0.25f + fAmount, 1.0f));				vvTexCoord.push_back(XMFLOAT2(0.25f + fAmount, 2.0f / 3.0f));		vvTexCoord.push_back(XMFLOAT2(0.5f - fAmount, 1.0f));
	vvTexCoord.push_back(XMFLOAT2(0.5f - fAmount, 1.0f));				vvTexCoord.push_back(XMFLOAT2(0.25f + fAmount, 2.0f / 3.0f));		vvTexCoord.push_back(XMFLOAT2(0.5f - fAmount, 2.0f / 3.0f));

	m_PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	AddVertexBuffer(pDevice, m_vvPosition, EVertexElement::POSITION);
	AddVertexBuffer(pDevice, vvColor, EVertexElement::COLOR);
	AddVertexBuffer(pDevice, vvTexCoord, EVertexElement::TEXCOORD1);
//	AddVertexBuffer(pDevice, vvNormal, EVertexElement::NORMAL);
//	SetIndexBuffer(pDevice, m_vIndex);

	m_AABB = BoundingBox(XMFLOAT3(0, 0, 0), XMFLOAT3(fx, fy, fz));
}

CCTerrainMesh::CCTerrainMesh(ID3D11Device * pDevice, int indexX, int indexZ, float fPivotXPos, float fPivotZPos, int fVertexDistant, int nMeshColumn, XMFLOAT3 f3Scale, void * pContext)
{
//	m_f3Scale = f3Scale;
//	CHeightMap *g_pHeightMap = (CHeightMap *)pContext;
//
//	float fInitZPos = fPivotZPos;
//
//	XMFLOAT3 v1;
//	XMFLOAT3 v2;
//	XMFLOAT3 v3;
//
//	int texCoordX = 1;
//	int texCoordZ = 1;
//
//	float fMinHeight = +FLT_MAX,fMaxHeight = -FLT_MAX;
//	float fHeight = 0;
//
//	float fTextureCoordSize = 1;
//
//
//	int index = 0;
//	for(int x=indexX ; x<nMeshColumn+indexX ; fPivotXPos+=fVertexDistant,++x){
//		for(int z=indexZ ; z<nMeshColumn+indexZ ; fPivotZPos+=fVertexDistant,++z){
//			//		ㅣ\  ㅣ
//			//		ㅣ \ ㅣ
//			//		ㅣ  \ㅣ
//
//			fHeight = GetHeight(x,z,pContext);
//			v1 = XMFLOAT3(fPivotXPos,fHeight,fPivotZPos);
//			if(fHeight < fMinHeight) fMinHeight = fHeight;
//			if(fHeight > fMaxHeight) fMaxHeight = fHeight;
//
//			fHeight = GetHeight(x,z+1,pContext);
//			v2 = XMFLOAT3(fPivotXPos,fHeight,fPivotZPos+fVertexDistant);
//			if(fHeight < fMinHeight) fMinHeight = fHeight;
//			if(fHeight > fMaxHeight) fMaxHeight = fHeight;
//
//			fHeight = GetHeight(x+1,z,pContext);
//			v3 = XMFLOAT3(fPivotXPos+fVertexDistant,fHeight,fPivotZPos);
//			if(fHeight < fMinHeight) fMinHeight = fHeight;
//			if(fHeight > fMaxHeight) fMaxHeight = fHeight;
//
//			m_vvPosition.push_back(v1);	index++;
//			m_vvPosition.push_back(v2);	index++;
//			m_vvPosition.push_back(v3);	index++;
//
//			v1 = m_vvPosition[index-2] - m_vvPosition[index-3];
//			v2 = m_vvPosition[index-1] - m_vvPosition[index-3];
//			D3DXVec3Cross(&v1,&v1,&v2);
//			D3DXVec3Normalize(&v1,&v1);
//			m_vvNormal.push_back(v1);
//			m_vvNormal.push_back(v1);
//			m_vvNormal.push_back(v1);
//
//			m_vvTexCoord1.push_back(XMFLOAT(texCoordX-fTextureCoordSize,texCoordZ-fTextureCoordSize));
//			m_vvTexCoord1.push_back(XMFLOAT(texCoordX-fTextureCoordSize,texCoordZ));
//			m_vvTexCoord1.push_back(XMFLOAT(texCoordX,texCoordZ-fTextureCoordSize));
//
//
//			fHeight = GetHeight(x+1,z+1,pContext);
//			v1 = XMFLOAT3(fPivotXPos+fVertexDistant,fHeight,fPivotZPos+fVertexDistant);
//			if(fHeight < fMinHeight) fMinHeight = fHeight;
//			if(fHeight > fMaxHeight) fMaxHeight = fHeight;
//
//			fHeight = GetHeight(x+1,z,pContext);
//			v2 = XMFLOAT3(fPivotXPos+fVertexDistant,fHeight,fPivotZPos);
//			if(fHeight < fMinHeight) fMinHeight = fHeight;
//			if(fHeight > fMaxHeight) fMaxHeight = fHeight;
//
//			fHeight = GetHeight(x,z+1,pContext);
//			v3 = XMFLOAT3(fPivotXPos,fHeight,fPivotZPos+fVertexDistant);
//			if(fHeight < fMinHeight) fMinHeight = fHeight;
//			if(fHeight > fMaxHeight) fMaxHeight = fHeight;
//
//			m_vvPosition.push_back(v1);	index++;
//			m_vvPosition.push_back(v2);	index++;
//			m_vvPosition.push_back(v3);	index++;
//
//			v1 = m_vvPosition[index-2] - m_vvPosition[index-3];
//			v2 = m_vvPosition[index-1] - m_vvPosition[index-3];
//			D3DXVec3Cross(&v1,&v1,&v2);
//			D3DXVec3Normalize(&v1,&v1);
//			m_vvNormal.push_back(v1);
//			m_vvNormal.push_back(v1);
//			m_vvNormal.push_back(v1);
//
//			m_vvTexCoord1.push_back(XMFLOAT(texCoordX,texCoordZ));
//			m_vvTexCoord1.push_back(XMFLOAT(texCoordX,texCoordZ-fTextureCoordSize));
//			m_vvTexCoord1.push_back(XMFLOAT(texCoordX-fTextureCoordSize,texCoordZ));
//			texCoordZ+=fTextureCoordSize;
//		}
//		texCoordX+=fTextureCoordSize;
//		fPivotZPos = fInitZPos;
//	}
//
//	if(m_vvPosition.size() == 0)
//		return;
//
//	m_PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//	m_nVertices = m_vvPosition.size();
//
//	AssembleToVertexBuffer(pDevice);
//
//	m_AABB = AABB(XMFLOAT3(m_vvPosition[0].x,fMinHeight,m_vvPosition[0].z),XMFLOAT3(XMFLOAT3(m_vvPosition[0].x + fVertexDistant*nMeshColumn,fMaxHeight,m_vvPosition[0].z + fVertexDistant*nMeshColumn)));
}

CCTerrainMesh::CCTerrainMesh(ID3D11Device * pDevice, float fSizeX, float fSizeZ, CHeightMap * pHeightMap)
{
	MeshData meshData;

	float fMinHeight = +FLT_MAX,fMaxHeight = -FLT_MAX;
	
	float fPosX;
	float fPosZ;
	float fHeight;
	XMFLOAT3 v1;
	XMFLOAT3 v2;
	XMFLOAT3 v3;
	XMVECTOR vec1;
	XMVECTOR vec2;
	XMVECTOR norm;

	float fTextureCoordSize = 1;
	float fDotDistant = fSizeX/257;
	float fTexDistant = 1.0f ;

	for (int x = 0; x<257-1 ; ++x) {
		for (int z = 0; z < 257-1; ++z) {
			fPosX = (x * fDotDistant) - (fSizeX / 2);
			fPosZ = (z * fDotDistant) - (fSizeZ / 2);
			fHeight = pHeightMap->GetHeight(fPosX, fPosZ);
			if (fHeight < fMinHeight) fMinHeight = fHeight;
			if (fHeight > fMaxHeight) fMaxHeight = fHeight;
			v1 = XMFLOAT3(fPosX, fHeight, fPosZ);

			fPosX = (x * fDotDistant) - (fSizeX / 2);
			fPosZ = ((z + 1) * fDotDistant) - (fSizeZ / 2);
			fHeight = pHeightMap->GetHeight(fPosX, fPosZ);
			if (fHeight < fMinHeight) fMinHeight = fHeight;
			if (fHeight > fMaxHeight) fMaxHeight = fHeight;
			v2 = XMFLOAT3(fPosX, fHeight, fPosZ);

			fPosX = ((x + 1) * fDotDistant) - (fSizeX / 2);
			fPosZ = (z * fDotDistant) - (fSizeZ / 2);
			fHeight = pHeightMap->GetHeight(fPosX, fPosZ);
			if (fHeight < fMinHeight) fMinHeight = fHeight;
			if (fHeight > fMaxHeight) fMaxHeight = fHeight;
			v3 = XMFLOAT3(fPosX, fHeight, fPosZ);

			meshData.m_vvPosition.push_back(v1);
			meshData.m_vvPosition.push_back(v2);
			meshData.m_vvPosition.push_back(v3);

			vec1 = XMVECTOR{v1.x-v2.x, v1.y-v2.y, v1.z-v2.z, 1};
			vec2 = XMVECTOR{v1.x-v3.x, v1.y-v3.y, v1.z-v3.z, 1};
			norm = XMVector3Cross(vec1, vec2);
			norm = XMVector3Normalize(norm);
			XMStoreFloat3(&v1, norm);
			meshData.m_vvNormal.push_back(v1);
			meshData.m_vvNormal.push_back(v1);
			meshData.m_vvNormal.push_back(v1);

			meshData.m_vvTexCoord1.push_back(XMFLOAT2(x*fTexDistant, z*fTexDistant));
			meshData.m_vvTexCoord1.push_back(XMFLOAT2(x*fTexDistant, (z+1)*fTexDistant));
			meshData.m_vvTexCoord1.push_back(XMFLOAT2((x+1)*fTexDistant, z*fTexDistant));

			fPosX = ((x+1) * fDotDistant) - (fSizeX/2);
			fPosZ = ((z+1) * fDotDistant) - (fSizeZ/2);
			fHeight = pHeightMap->GetHeight(fPosX, fPosZ);
			if(fHeight < fMinHeight) fMinHeight = fHeight;
			if(fHeight > fMaxHeight) fMaxHeight = fHeight;
			v1 = XMFLOAT3(fPosX, fHeight, fPosZ);

			fPosX = ((x+1) * fDotDistant) - (fSizeX/2);
			fPosZ = (z * fDotDistant) - (fSizeZ/2);
			fHeight = pHeightMap->GetHeight(fPosX, fPosZ);
			if(fHeight < fMinHeight) fMinHeight = fHeight;
			if(fHeight > fMaxHeight) fMaxHeight = fHeight;
			v2 = XMFLOAT3(fPosX, fHeight, fPosZ);

			fPosX = (x * fDotDistant) - (fSizeX/2);
			fPosZ = ((z+1) * fDotDistant) - (fSizeZ/2);
			fHeight = pHeightMap->GetHeight(fPosX, fPosZ);
			if(fHeight < fMinHeight) fMinHeight = fHeight;
			if(fHeight > fMaxHeight) fMaxHeight = fHeight;
			v3 = XMFLOAT3(fPosX, fHeight, fPosZ);

			meshData.m_vvPosition.push_back(v1);
			meshData.m_vvPosition.push_back(v2);
			meshData.m_vvPosition.push_back(v3);

			vec1 = XMVECTOR{v1.x-v2.x, v1.y-v2.y, v1.z-v2.z, 1};
			vec2 = XMVECTOR{v1.x-v3.x, v1.y-v3.y, v1.z-v3.z, 1};
			norm = XMVector3Cross(vec1, vec2);
			norm = XMVector3Normalize(norm);
			XMStoreFloat3(&v1, norm);
			meshData.m_vvNormal.push_back(v1);
			meshData.m_vvNormal.push_back(v1);
			meshData.m_vvNormal.push_back(v1);

			meshData.m_vvTexCoord1.push_back(XMFLOAT2((x+1)*fTexDistant, (z+1)*fTexDistant));
			meshData.m_vvTexCoord1.push_back(XMFLOAT2((x+1)*fTexDistant, z*fTexDistant));
			meshData.m_vvTexCoord1.push_back(XMFLOAT2(x*fTexDistant, (z+1)*fTexDistant));
		}
	}

	for (int i = 0; i < meshData.m_vvPosition.size(); i++) 
		meshData.m_vvColor.push_back(XMFLOAT4(rand() % 255 / 255.0f, rand() % 255 / 255.0f, rand() % 255 / 255.0f, 1)); // + RANDOM_COLOR;

	m_PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//	m_AABB = AABB(XMFLOAT3(m_vvPosition[0].x,fMinHeight,m_vvPosition[0].z),XMFLOAT3(XMFLOAT3(m_vvPosition[0].x + fVertexDistant*nMeshColumn,fMaxHeight,m_vvPosition[0].z + fVertexDistant*nMeshColumn)));
	meshData.m_AABB = BoundingBox(XMFLOAT3(0, (fMinHeight+fMaxHeight)/2, 0), XMFLOAT3(fSizeX/2, (abs(fMinHeight)+abs(fMaxHeight))/2, fSizeZ/2));
	CreateMesh(pDevice, meshData);

//	float fHeight = 0;
//	float fInitZPos = fPivotZPos;
	//XMFLOAT3 v1;
	//XMFLOAT3 v2;
	//XMFLOAT3 v3;
	//
	//int texCoordX = 1;
	//int texCoordZ = 1;
	//
	//int index = 0;
	//for(int x=indexX ; x<nMeshColumn+indexX ; fPivotXPos+=fVertexDistant,++x){
	//	for(int z=indexZ ; z<nMeshColumn+indexZ ; fPivotZPos+=fVertexDistant,++z){
	//		//		ㅣ\  ㅣ
	//		//		ㅣ \ ㅣ
	//		//		ㅣ  \ㅣ
	//
	//		fHeight = GetHeight(x,z,pContext);
	//		v1 = XMFLOAT3(fPivotXPos,fHeight,fPivotZPos);
	//		if(fHeight < fMinHeight) fMinHeight = fHeight;
	//		if(fHeight > fMaxHeight) fMaxHeight = fHeight;
	//
	//		fHeight = GetHeight(x,z+1,pContext);
	//		v2 = XMFLOAT3(fPivotXPos,fHeight,fPivotZPos+fVertexDistant);
	//		if(fHeight < fMinHeight) fMinHeight = fHeight;
	//		if(fHeight > fMaxHeight) fMaxHeight = fHeight;
	//
	//		fHeight = GetHeight(x+1,z,pContext);
	//		v3 = XMFLOAT3(fPivotXPos+fVertexDistant,fHeight,fPivotZPos);
	//		if(fHeight < fMinHeight) fMinHeight = fHeight;
	//		if(fHeight > fMaxHeight) fMaxHeight = fHeight;
	//
	//		meshData.m_vvPosition.push_back(v1);	index++;
	//		meshData.m_vvPosition.push_back(v2);	index++;
	//		meshData.m_vvPosition.push_back(v3);	index++;
	//
	//		v1 = meshData.m_vvPosition[index-2] - meshData.m_vvPosition[index-3];
	//		v2 = meshData.m_vvPosition[index-1] - meshData.m_vvPosition[index-3];
	//		D3DXVec3Cross(&v1,&v1,&v2);
	//		D3DXVec3Normalize(&v1,&v1);
	//		meshData.m_vvNormal.push_back(v1);
	//		meshData.m_vvNormal.push_back(v1);
	//		meshData.m_vvNormal.push_back(v1);
	//
	//		meshData.m_vvTexCoord1.push_back(XMFLOAT(texCoordX-fTextureCoordSize,texCoordZ-fTextureCoordSize));
	//		meshData.m_vvTexCoord1.push_back(XMFLOAT(texCoordX-fTextureCoordSize,texCoordZ));
	//		meshData.m_vvTexCoord1.push_back(XMFLOAT(texCoordX,texCoordZ-fTextureCoordSize));
	//
	//
	//		fHeight = GetHeight(x+1,z+1,pContext);
	//		v1 = XMFLOAT3(fPivotXPos+fVertexDistant,fHeight,fPivotZPos+fVertexDistant);
	//		if(fHeight < fMinHeight) fMinHeight = fHeight;
	//		if(fHeight > fMaxHeight) fMaxHeight = fHeight;
	//
	//		fHeight = GetHeight(x+1,z,pContext);
	//		v2 = XMFLOAT3(fPivotXPos+fVertexDistant,fHeight,fPivotZPos);
	//		if(fHeight < fMinHeight) fMinHeight = fHeight;
	//		if(fHeight > fMaxHeight) fMaxHeight = fHeight;
	//
	//		fHeight = GetHeight(x,z+1,pContext);
	//		v3 = XMFLOAT3(fPivotXPos,fHeight,fPivotZPos+fVertexDistant);
	//		if(fHeight < fMinHeight) fMinHeight = fHeight;
	//		if(fHeight > fMaxHeight) fMaxHeight = fHeight;
	//
	//		m_vvPosition.push_back(v1);	index++;
	//		m_vvPosition.push_back(v2);	index++;
	//		m_vvPosition.push_back(v3);	index++;
	//
	//		v1 = m_vvPosition[index-2] - m_vvPosition[index-3];
	//		v2 = m_vvPosition[index-1] - m_vvPosition[index-3];
	//		D3DXVec3Cross(&v1,&v1,&v2);
	//		D3DXVec3Normalize(&v1,&v1);
	//		meshData.m_vvNormal.push_back(v1);
	//		meshData.m_vvNormal.push_back(v1);
	//		meshData.m_vvNormal.push_back(v1);
	//
	//		meshData.m_vvTexCoord1.push_back(XMFLOAT(texCoordX,texCoordZ));
	//		meshData.m_vvTexCoord1.push_back(XMFLOAT(texCoordX,texCoordZ-fTextureCoordSize));
	//		meshData.m_vvTexCoord1.push_back(XMFLOAT(texCoordX-fTextureCoordSize,texCoordZ));
	//		texCoordZ+=fTextureCoordSize;
	//	}
	//	texCoordX+=fTextureCoordSize;
	//	fPivotZPos = fInitZPos;
	//}
	//
	//if(m_vvPosition.size() == 0)
	//	return;
	//
	//m_vvPosition = meshData.m_vvPosition;
	//m_vIndex = meshData.m_vIndex;
	
//	m_AABB = AABB(XMFLOAT3(m_vvPosition[0].x,fMinHeight,m_vvPosition[0].z),XMFLOAT3(XMFLOAT3(m_vvPosition[0].x + fVertexDistant*nMeshColumn,fMaxHeight,m_vvPosition[0].z + fVertexDistant*nMeshColumn)));
}

CCTerrainMesh::CCTerrainMesh(ID3D11Device * pDevice, int xStart, int zStart, int nWidth, int nLength, CHeightMap * pHeightMap) {
	MeshData meshData;
	m_f3Scale = pHeightMap->GetScale();

	UINT nRowPos = pHeightMap->GetHeightMapWidth();
	UINT nColumnPos = pHeightMap->GetHeightMapHeight();
	float fPrimitiveSizeX = static_cast<float>((pHeightMap->GetHeightMapWidth()-1))*m_f3Scale.x / (nRowPos-1);
	float fPrimitiveSizeZ = static_cast<float>((pHeightMap->GetHeightMapHeight()-1))*m_f3Scale.z / (nColumnPos-1);
	float fTexSizeU = 1.0f/(nRowPos-1);
	float fTexSizeV = 1.0f/(nColumnPos-1);

	int cxHeightMap = pHeightMap->GetHeightMapWidth();
	int czHeightMap = pHeightMap->GetHeightMapHeight();

	float fHeight = 0.0f, fMinHeight = +FLT_MAX, fMaxHeight = -FLT_MAX;
	for (int z = zStart; z < (zStart + nLength); z++)
	{
		for (int x = xStart; x < (xStart + nWidth); x++)
		{
			fHeight = pHeightMap->GetHeight(x*m_f3Scale.x-(g_fWorldSize/2), z*m_f3Scale.z-(g_fWorldSize/2));
			meshData.m_vvPosition.push_back(XMFLOAT3((x*fPrimitiveSizeX)-(g_fWorldSize/2), fHeight, (z*fPrimitiveSizeZ)-(g_fWorldSize/2)));
		//	meshData.m_vvNormal.push_back(pHeightMap->GetNormal((x*m_f3Scale.x)-(g_fWorldSize/2), (z*m_f3Scale.z)-(g_fWorldSize/2)));
			meshData.m_vvTexCoord1.push_back(XMFLOAT2(x*fTexSizeU, z*fTexSizeV));
			meshData.m_vvTexCoord2.push_back(XMFLOAT2(x, z));
			if (fHeight < fMinHeight) fMinHeight = fHeight;
			if (fHeight > fMaxHeight) fMaxHeight = fHeight;
		}
	}
	
	for (int z = 0; z < nLength - 1; z++)
	{
		if ((z % 2) == 0)
		{
			//홀수 번째 줄이므로(z = 0, 2, 4, ...) 인덱스의 나열 순서는 왼쪽에서 오른쪽 방향이다.
			for (int x = 0; x < nWidth; x++)
			{
				//첫 번째 줄을 제외하고 줄이 바뀔 때마다(x == 0) 첫 번째 인덱스를 추가한다.
				if ((x == 0) && (z > 0)) meshData.m_vIndex.push_back((UINT)(x + (z * nWidth)));
				//아래, 위의 순서로 인덱스를 추가한다.
				meshData.m_vIndex.push_back((UINT)(x + (z * nWidth)));
				meshData.m_vIndex.push_back((UINT)((x + (z * nWidth)) + nWidth));
			}
		}
		else
		{
			//짝수 번째 줄이므로(z = 1, 3, 5, ...) 인덱스의 나열 순서는 오른쪽에서 왼쪽 방향이다.
			for (int x = nWidth - 1; x >= 0; x--)
			{
				//줄이 바뀔 때마다(x == (nWidth-1)) 첫 번째 인덱스를 추가한다.
				if (x == (nWidth - 1)) meshData.m_vIndex.push_back((UINT)(x + (z * nWidth)));
				//아래, 위의 순서로 인덱스를 추가한다.
				meshData.m_vIndex.push_back((UINT)(x + (z * nWidth)));
				meshData.m_vIndex.push_back((UINT)((x + (z * nWidth)) + nWidth));
			}
		}
	}

	for (int i = 0; i < meshData.m_vvPosition.size(); i++) 
		meshData.m_vvColor.push_back(XMFLOAT4(rand() % 255 / 255.0f, rand() % 255 / 255.0f, rand() % 255 / 255.0f, 1)); // + RANDOM_COLOR;

	m_PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
//	m_AABB = AABB(XMFLOAT3(m_vvPosition[0].x,fMinHeight,m_vvPosition[0].z),XMFLOAT3(XMFLOAT3(m_vvPosition[0].x + fVertexDistant*nMeshColumn,fMaxHeight,m_vvPosition[0].z + fVertexDistant*nMeshColumn)));
	CreateMesh(pDevice, meshData);
	CreateAABB();
}

CCTerrainMesh::CCTerrainMesh(ID3D11Device * pDevice, CHeightMap * pHeightMap)
{
	MeshData meshData;
	XMFLOAT3 vScale = pHeightMap->GetScale();

	UINT nCellPerPatch = 64;
	UINT nRowPos = (pHeightMap->GetHeightMapWidth()-1)/nCellPerPatch * 20 + 1;
	UINT nColumnPos = (pHeightMap->GetHeightMapHeight()-1)/nCellPerPatch * 20 + 1;
//	UINT nRowPos = 37;
//	UINT nColumnPos = 37;
//	float fPatchSizeX = g_fWorldSize/(nRowPos-1);
//	float fPatchSizeZ = g_fWorldSize/(nColumnPos-1);
	float fPatchSizeX = static_cast<float>((pHeightMap->GetHeightMapWidth()-1))*vScale.x / (nRowPos-1);
	float fPatchSizeZ = static_cast<float>((pHeightMap->GetHeightMapHeight()-1))*vScale.z / (nColumnPos-1);
	float fTexSizeU = 1.0f/(nRowPos-1);
	float fTexSizeV = 1.0f/(nColumnPos-1);

	int xPos = 0;
	int zPos = 0;
	for (int z = 0; z < nColumnPos; z++)
	{
		for (int x = 0; x < nRowPos; x++)
		{
			meshData.m_vvPosition.push_back(XMFLOAT3(x*fPatchSizeX-(g_fWorldSize/2), 0, z*fPatchSizeZ-(g_fWorldSize/2)));
			meshData.m_vvTexCoord1.push_back(XMFLOAT2(x*fTexSizeU, 1-(z*fTexSizeV)));
			meshData.m_vvTexCoord2.push_back(XMFLOAT2(x*4, z*4));
		}
	}

	UINT idx = 0;
	for (int i = 0; i < nColumnPos-1; ++i) {
		for (int j = 0; j < nRowPos-1; ++j) {
			meshData.m_vIndex.push_back(idx);
			meshData.m_vIndex.push_back(idx+1);
			meshData.m_vIndex.push_back(idx+nRowPos);
			meshData.m_vIndex.push_back(idx+nRowPos+1);
			idx++;
		}
		idx++;
	}

	for (int i = 0; i < meshData.m_vvPosition.size(); i++) 
		meshData.m_vvColor.push_back(XMFLOAT4(rand() % 255 / 255.0f, rand() % 255 / 255.0f, rand() % 255 / 255.0f, 1)); // + RANDOM_COLOR;

	float fMinHeight = pHeightMap->GetMinHeight();
	float fMaxHeight = pHeightMap->GetMaxHeight();
	m_PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;
	meshData.m_AABB = BoundingBox(XMFLOAT3(0, (fMinHeight+fMaxHeight)/2, 0), XMFLOAT3((g_fWorldSize/2), (abs(fMinHeight)+abs(fMaxHeight))/2, (g_fWorldSize/2)));
	CreateMesh(pDevice, meshData);	
}

CCTerrainMesh::CCTerrainMesh(ID3D11Device * pDevice, int xStart, int zStart, int xEnd, int zEnd, int num, CHeightMap * pHeightMap)
{
	float fMinHeight = pHeightMap->GetMinHeight();
	float fMaxHeight = pHeightMap->GetMaxHeight();
	float fMaxX, fMaxZ, fMinX, fMinZ;
	fMaxX = fMaxZ = -FLT_MAX;
	fMinX = fMinZ = +FLT_MAX;

	MeshData meshData;
	XMFLOAT3 vScale = pHeightMap->GetScale();

	UINT nCellPerPatch = 64;
//	UINT nRowPos = (pHeightMap->GetHeightMapWidth()-1)/nCellPerPatch + 1;
//	UINT nColumnPos = (pHeightMap->GetHeightMapHeight()-1)/nCellPerPatch + 1;
	UINT nBegRowPos = (xStart)/nCellPerPatch;
	UINT nBegColumnPos = (zStart)/nCellPerPatch;
	UINT nEndRowPos = (xEnd)/nCellPerPatch;
	UINT nEndColumnPos = (zEnd)/nCellPerPatch;
	UINT nRowPos = nEndRowPos - nBegRowPos;
	UINT nColumnPos = nEndColumnPos - nBegColumnPos;
	float fPatchSizeX = static_cast<float>(xEnd - xStart)*vScale.x / (nRowPos-1);
	float fPatchSizeZ = static_cast<float>(zEnd - zStart)*vScale.z / (nColumnPos-1);
	float fTexSizeU = 1.0f/((pHeightMap->GetHeightMapWidth()-1)/static_cast<float>(nCellPerPatch));
	float fTexSizeV = 1.0f/((pHeightMap->GetHeightMapHeight()-1)/static_cast<float>(nCellPerPatch));

	int xPos = 0;
	int zPos = 0;
	for (int z = nBegColumnPos; z < nColumnPos+nBegColumnPos; z++)
	{
		for (int x = nBegRowPos; x < nRowPos+nBegRowPos; x++)
		{
			XMFLOAT3 f3Position(x*fPatchSizeX, 0, z*fPatchSizeZ);
			meshData.m_vvPosition.push_back(f3Position);
			(fMaxX < f3Position.x) ? fMaxX = f3Position.x : NULL;
			(fMaxZ < f3Position.z) ? fMaxZ = f3Position.z : NULL;
			(fMinX > f3Position.x) ? fMinX = f3Position.x : NULL;
			(fMinZ > f3Position.z) ? fMinZ = f3Position.z : NULL;

			meshData.m_vvTexCoord1.push_back(XMFLOAT2(x*fTexSizeU, 1-(z*fTexSizeV)));
			meshData.m_vvTexCoord2.push_back(XMFLOAT2(x*4, z*4));
		}
	}

	UINT idx = 0;
	for (int i = 0; i < nColumnPos-1; ++i) {
		for (int j = 0; j < nRowPos-1; ++j) {
			meshData.m_vIndex.push_back(idx);
			meshData.m_vIndex.push_back(idx+1);
			meshData.m_vIndex.push_back(idx+nRowPos);
			meshData.m_vIndex.push_back(idx+nRowPos+1);
			idx++;
		}
		idx++;
	}

	for (int i = 0; i < meshData.m_vvPosition.size(); i++) 
		meshData.m_vvColor.push_back(XMFLOAT4(rand() % 255 / 255.0f, rand() % 255 / 255.0f, rand() % 255 / 255.0f, 1)); // + RANDOM_COLOR;

	m_PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;

	float fSizeX = abs((fMaxX - fMinX) / 2);
	float fSizeY = (abs(fMinHeight) + abs(fMaxHeight)) / 2;
	float fSizeZ = abs((fMaxZ - fMinZ) / 2);
	m_AABB = BoundingBox(XMFLOAT3((fMinX + fMaxX) / 2, (fMinHeight+fMaxHeight)/2, (fMinZ + fMaxZ) / 2), XMFLOAT3(fSizeX, fSizeY, fSizeZ));
	CreateMesh(pDevice, meshData);	
}

