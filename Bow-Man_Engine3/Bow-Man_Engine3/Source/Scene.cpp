#include "stdafx.h"
#include "Scene.h"
#include "D2DFramework.h"
#include "UiObject.h"
#include "InputFramework.h"

#include "Light.h"
#include "ChatDataBase.h"
#include "Renderer.h"
#include "CFBXLoader.h"
#include "HeightMap.h"

#include "Texture.h"
#include "Shader.h"

#include "ConstantBuffer.h"

#include "Picking.h"
#include "Mesh.h"
#include "Model.h"
#include "ModelAssembly.h"

#include "RenderState.h"

#include "Object.h"
#include "Camera.h"
#include "Player.h"

CCameraObject *pCamera;
CPlayerObject *pPlayer;

CObject *pLight;

/*
	--참고사항--
	Map Data를 저장하는 기능이 개발되지 않아, Hard Code로 객체가 배치되었음
*/
void CScene::CreateScene(ID3D11Device * pDevice,ID3D11DeviceContext * pDeviceContext){
	CreateTerrain(pDevice, pDeviceContext);

	CModelAssembly *pModelAssembly = nullptr;
	CModel *pModel = nullptr;
	CObject *pObject = nullptr;
	XMFLOAT3 vPos;

	pModelAssembly = new CModelAssembly;
		pModel = new CModel;
		pModel->SetMesh(new CCubeMesh(pDevice, 0.3, 0.3, 0.3, 0));
		pModel->SetTexture(Texture(L"default"));
		pModel->SetMaterial(Material(L"default"));
		pModel->SetDptstnclState(EDpthStnclState::NONE);
		pModel->SetRstrzrState(ERstrzrState::WIRE_NONE_CULL);
		pModel->SetBlndState(EBlndState::NONE);
		pModel->SetShaderPack(_INT(EVertexElement::POSITION) | _INT(EVertexElement::COLOR));
	pModelAssembly->AddModel(pModel);
	pModelAssembly->CreateModelHomo();
	CCameraObject *m_pCamera = new CCameraObject;
	m_pCamera->CreateCameraMatrix(g_fNearPlaneDistant,g_fFarPlaneDistant,ASPECT_RATIO,g_fFOV);
	m_pCamera->CreateViewport(pDeviceContext,0,0,FRAME_BUFFER_WIDTH,FRAME_BUFFER_HEIGHT,0.0f,1.0f);
	m_pCamera->SetModelAssembly(pModelAssembly);
	m_pCamera->UpdateViewMatrix();
	pCamera = m_pCamera;

	pModelAssembly = new CModelAssembly;
		pModel = new CModel;
		pModel->SetMesh(new CCubeMesh(pDevice, 0.5, 1, 0.5, 0));
		pModel->SetTexture(Texture(L"default"));
		pModel->SetMaterial(Material(L"default"));
		pModel->SetDptstnclState(EDpthStnclState::NONE);
		pModel->SetRstrzrState(ERstrzrState::WIRE_NONE_CULL);
		pModel->SetBlndState(EBlndState::NONE);
		pModel->SetShaderPack(_INT(EVertexElement::POSITION) | _INT(EVertexElement::COLOR));
	pModelAssembly->AddModel(pModel);
	pModelAssembly->CreateModelHomo();
	m_pPlayer = new CPlayerObject;
	m_pPlayer->SetCamera(m_pCamera);
	m_pPlayer->SetModelAssembly(pModelAssembly);
	m_pPlayer->SetObjectAttribute(EObjectAttribute::VISIBLE, false);
	m_pPlayer->SetObjectAttribute(EObjectAttribute::PICK, false);
	m_pPlayer->SetObjectAttribute(EObjectAttribute::SELECT, false);
	m_pPlayer->SetPosition(0, 60, -10);
	m_lpObject.push_back(m_pPlayer);
	pPlayer = m_pPlayer;

	pModelAssembly = new CModelAssembly;
		pModel = new CModel;
		pModel->SetMesh(new CSkyboxMesh(pDevice, 5, 5, 5));
		pModel->SetShaderPack(_INT(EVertexElement::POSITION) | _INT(EVertexElement::TEXCOORD1));
		pModel->SetDptstnclState(EDpthStnclState::NONE);
		pModel->SetRstrzrState(ERstrzrState::NONE);
		pModel->SetBlndState(EBlndState::NONE);
		pModel->SetTexture(Texture(L"space_10"));
		pModel->SetShaderPack(ShaderManager->GetShaderPack(_T("skybox")));
	pModelAssembly->AddModel(pModel);
		pModel = new CModel;
		pModel->SetMesh(new CSkyboxMesh(pDevice, 20, 20, 20));
		pModel->SetTexture(Texture(L"space_10"));
		pModel->SetDptstnclState(EDpthStnclState::NONE);
		pModel->SetRstrzrState(ERstrzrState::NONE);
		pModel->SetBlndState(EBlndState::NONE);
		pModel->SetShaderPack(_INT(EVertexElement::POSITION) | _INT(EVertexElement::TEXCOORD1));
	pModelAssembly->AddModel(pModel);
	pModelAssembly->CreateModelHomo();
	pObject = new CObject;
	pObject->SetObjectAttribute(EObjectAttribute::VISIBLE_TEST, false);
	pObject->SetObjectAttribute(EObjectAttribute::PICK, true);
	pObject->SetObjectAttribute(EObjectAttribute::SELECT, true);
	pObject->SetModelAssembly(pModelAssembly);
	pObject->SetPosition(0, 250, 0);
//	pObject->AddUpdate(UpdateRotate);
	m_lpObject.push_back(pObject);

	pModelAssembly = new CModelAssembly;
		pModel = new CModel;
		pModel->SetMesh(new CCubeMesh(pDevice, 10, 10, 10, 0));
		pModel->SetTexture(Texture(L"default"));
		pModel->SetMaterial(Material(L"default"));
		pModel->SetDptstnclState(EDpthStnclState::NONE);
		pModel->SetRstrzrState(ERstrzrState::NONE);
		pModel->SetBlndState(EBlndState::NONE);
//		pModel->SetShaderPack(_INT(EVertexElement::POSITION) | _INT(EVertexElement::TEXCOORD1) | _INT(EVertexElement::TANGENT) | _INT(EVertexElement::NORMAL));
		pModel->SetShaderPack(_INT(EVertexElement::POSITION) | _INT(EVertexElement::TEXCOORD1) | _INT(EVertexElement::NORMAL));
	pModelAssembly->AddModel(pModel);
	pModelAssembly->CreateModelHomo();
	pObject = new CObject;
	pObject->SetModelAssembly(pModelAssembly);
	pObject->SetObjectAttribute(EObjectAttribute::PICK, true);
	pObject->SetObjectAttribute(EObjectAttribute::SELECT, true);
	pObject->SetPosition(0, 30.5, 0);
	pObject->AddUpdate(UpdateRotate);
	m_lpObject.push_back(pObject);

	pModelAssembly = new CModelAssembly;
		pModel = new CModel;
		pModel->SetMesh(new CCubeMesh(pDevice, 5.01, 5.01, 5.01, 0));
		pModel->SetTexture(Texture(L"default"));
		pModel->SetMaterial(Material(L"default"));
		pModel->SetDptstnclState(EDpthStnclState::NONE);
		pModel->SetRstrzrState(ERstrzrState::WIRE_NONE_CULL);
		pModel->SetBlndState(EBlndState::NONE);
		pModel->SetShaderPack(_INT(EVertexElement::POSITION) | _INT(EVertexElement::COLOR));
	pModelAssembly->AddModel(pModel);
		pModel = new CModel;
		pModel->SetMesh(new CCubeMesh(pDevice, 1, 1, 5, 0));
		pModel->SetTexture(Texture(L"default"));
		pModel->SetMaterial(Material(L"default"));
		pModel->SetDptstnclState(EDpthStnclState::NONE);
		pModel->SetRstrzrState(ERstrzrState::NONE);
		pModel->SetBlndState(EBlndState::NONE);
		pModel->SetShaderPack(_INT(EVertexElement::POSITION) | _INT(EVertexElement::TEXCOORD1));
	pModelAssembly->AddModel(pModel);
		pModel = new CModel;
		pModel->SetMesh(new CCubeMesh(pDevice, 1, 5, 1, 0));
		pModel->SetTexture(Texture(L"default"));
		pModel->SetMaterial(Material(L"default"));
		pModel->SetDptstnclState(EDpthStnclState::DEPTH_TEST_NO_WRITE);
		pModel->SetRstrzrState(ERstrzrState::SOLID_NONE_CULL);
		pModel->SetBlndState(EBlndState::ENABLE_BLEND_ALPHA);
	pModelAssembly->AddModel(pModel);
		pModel = new CModel;
		pModel->SetMesh(new CCubeMesh(pDevice, 5, 1, 1, 0));
		pModel->SetTexture(Texture(L"default"));
		pModel->SetMaterial(Material(L"metal"));
		pModel->SetDptstnclState(EDpthStnclState::NONE);
		pModel->SetRstrzrState(ERstrzrState::NONE);
		pModel->SetBlndState(EBlndState::NONE);
		pModel->SetShaderPack(ShaderManager->GetShaderPack(_T("deferred_illuminated")));
	pModelAssembly->AddModel(pModel);
		pModel = new CModel;
		pModel->SetMesh(new CCubeMesh(pDevice, 3, 3, 3, 0));
		pModel->SetTexture(Texture(L"default"));
		pModel->SetMaterial(Material(L"default"));
		pModel->SetDptstnclState(EDpthStnclState::NONE);
		pModel->SetRstrzrState(ERstrzrState::NONE);
		pModel->SetBlndState(EBlndState::NONE);
	pModelAssembly->AddModel(pModel);
	pModelAssembly->CreateModelHomo();
	pObject = new CObject;
	pObject->SetModelAssembly(pModelAssembly);
	pObject->SetObjectAttribute(EObjectAttribute::PICK, true);
	pObject->SetObjectAttribute(EObjectAttribute::SELECT, true);
	vPos = XMFLOAT3(-20, 30, 20);
	vPos.y = g_pHeightMap->GetHeight(vPos.x, vPos.z) + 30;
	pObject->SetPosition(vPos);
	pObject->Rotate(10, 20, 30);
	pObject->AddUpdate(UpdateRotate);
	pObject->AddUpdate(UpdateRotate);
	pObject->AddUpdate(UpdateRotate);
	pObject->AddUpdate(UpdateRotate);
	m_lpObject.push_back(pObject);

	pModelAssembly = new CModelAssembly;
 	pModelAssembly->LoadModel(pDevice, L"../../Resource/Model/Game_Characters/ch1.obj", 0.001, 0);
	int x = 0;
	int y = 0;
	for (int i = 0; i< 10 ; ++i) {
		for (int j = 0; j< 10; ++j) {
			pObject = new CObject;
			pObject->SetModelAssembly(pModelAssembly);
			pObject->SetObjectAttribute(EObjectAttribute::PICK, true);
			pObject->SetObjectAttribute(EObjectAttribute::SELECT, true);
			vPos = XMFLOAT3(x, 0, y);
			vPos.y = g_pHeightMap->GetHeight(vPos.x, vPos.z);
			pObject->SetPosition(vPos);
			m_lpObject.push_back(pObject);
			x += 10;
		}
		x = 0;
		y += 10;
	}

	pModelAssembly = new CModelAssembly;
	pModelAssembly->LoadModel(pDevice, L"../../Resource/Model/hyria_collection/hyria_collection.obj", 10, 0);
	pObject = new CObject;
	pObject->SetModelAssembly(pModelAssembly);
	pObject->SetObjectAttribute(EObjectAttribute::VISIBLE_TEST, false);
	pObject->SetObjectAttribute(EObjectAttribute::PICK, true);
	pObject->SetObjectAttribute(EObjectAttribute::SELECT, true);
	vPos = XMFLOAT3(0, 0, -20);
	vPos.y = g_pHeightMap->GetHeight(vPos.x, vPos.z) + 0.3;
	pObject->SetPosition(vPos);
	pObject->SetDirection(0, 1, 0);
	m_lpObject.push_back(pObject);

	pModelAssembly = new CModelAssembly;
	pModelAssembly->LoadModel(pDevice, L"../../Resource/Model/../../Resource/Model/diva/diva.fbx", 0.05, 0);
	pObject = new CObject;
	pObject->SetModelAssembly(pModelAssembly);
	pObject->SetObjectAttribute(EObjectAttribute::VISIBLE_TEST, false);
	pObject->SetObjectAttribute(EObjectAttribute::PICK, true);
	pObject->SetObjectAttribute(EObjectAttribute::SELECT, true);
	vPos = XMFLOAT3(-14, 0, 0);
	vPos.y = g_pHeightMap->GetHeight(vPos.x, vPos.z);
	pObject->SetPosition(vPos);
	pObject->SetDirection(0, 1, 0);
	m_lpObject.push_back(pObject);

	pModelAssembly = new CModelAssembly;
	pModelAssembly->LoadModel(pDevice, L"../../Resource/Model/tree/tree.obj", 2, 0);
	for (int i = 0; i<2000 ; ++i) {
		pObject = new CObject;
		pObject->SetModelAssembly(pModelAssembly);
		pObject->SetObjectAttribute(EObjectAttribute::PICK, true);
		pObject->SetObjectAttribute(EObjectAttribute::SELECT, true);
		XMFLOAT3 vPos(RandomBetween(-(g_fWorldSize/2), (g_fWorldSize/2)), 0, RandomBetween(-(g_fWorldSize/2), (g_fWorldSize/2)));
		vPos.y = g_pHeightMap->GetHeight(vPos.x, vPos.z);
		pObject->SetPosition(vPos);
		pObject->Rotate(AXIS::Y, rand()%360);
		m_lpObject.push_back(pObject);
	}
	
	pModelAssembly = new CModelAssembly;
	pModelAssembly->LoadModel(pDevice, L"../../Resource/Model/tree/tree.obj", 4, 0);
	for (int i = 0; i<2000 ; ++i) {
		pObject = new CObject;
		pObject->SetModelAssembly(pModelAssembly);
		pObject->SetObjectAttribute(EObjectAttribute::PICK, true);
		pObject->SetObjectAttribute(EObjectAttribute::SELECT, true);
		XMFLOAT3 vPos(RandomBetween(-(g_fWorldSize/2), (g_fWorldSize/2)), 0, RandomBetween(-(g_fWorldSize/2), (g_fWorldSize/2)));
		vPos.y = g_pHeightMap->GetHeight(vPos.x, vPos.z);
		pObject->SetPosition(vPos);
		pObject->Rotate(AXIS::Y, rand()%360);
		m_lpObject.push_back(pObject);
	}

	pModelAssembly = new CModelAssembly;
	pModelAssembly->LoadModel(pDevice, L"../../Resource/Model/tree/tree.obj", 6, 0);
	for (int i = 0; i<2000 ; ++i) {
		pObject = new CObject;
		pObject->SetModelAssembly(pModelAssembly);
		pObject->SetObjectAttribute(EObjectAttribute::PICK, true);
		pObject->SetObjectAttribute(EObjectAttribute::SELECT, true);
		XMFLOAT3 vPos(RandomBetween(-(g_fWorldSize/2), (g_fWorldSize/2)), 0, RandomBetween(-(g_fWorldSize/2), (g_fWorldSize/2)));
		vPos.y = g_pHeightMap->GetHeight(vPos.x, vPos.z);
		pObject->SetPosition(vPos);
		pObject->Rotate(AXIS::Y, rand()%360);
		m_lpObject.push_back(pObject);
	}

	pModelAssembly = new CModelAssembly;
		pModel = new CModel;
		pModel->SetMesh(new CSquareMesh(pDevice, 50, 50, EMeshElement::INDEX|EMeshElement::BOTH_SIDE));
		pModel->SetTexture(Texture(L"light_01"));
		pModel->SetMaterial(Material(L"default"));
		pModel->SetShaderPack(_INT(EVertexElement::POSITION) | _INT(EVertexElement::TEXCOORD1));
		pModel->SetBlndState(EBlndState::ENABLE_BLEND_ALPHA);
	pModelAssembly->AddModel(pModel);
	pModelAssembly->CreateModelHomo();
//	pObject = new CDirLightObject(DirLight(XMFLOAT4(0.05,0.05,0.05,1), XMFLOAT4(0.2,0.2,0.15,1), XMFLOAT4(1,0.9,0.8,1), 1));
	pObject = new CDirLightObject(DirLight(XMFLOAT4(0.05,0.05,0.05,1), XMFLOAT4(0.2,0.2,0.15,1), XMFLOAT4(0.5,0.4,0.3,1), 1));
	pObject->SetModelAssembly(pModelAssembly);
	pObject->SetObjectAttribute(EObjectAttribute::PICK, true);
	pObject->SetObjectAttribute(EObjectAttribute::SELECT, true);
	pObject->SetPosition(0, 400, 0);
	pObject->SetDirection(XMFLOAT3{ 1, -3, 2 });
	m_lpObject.push_back(pObject);

	pModelAssembly = new CModelAssembly;
		pModel = new CModel;
		pModel->SetMesh(new CCrossBillboardMesh(pDevice, 3,3,3, 0));
		pModel->SetTexture(Texture(L"light_01"));
		pModel->SetMaterial(Material(L"default"));
		pModel->SetShaderPack(_INT(EVertexElement::POSITION) | _INT(EVertexElement::TEXCOORD1));
		pModel->SetDptstnclState(EDpthStnclState::DEPTH_TEST_NO_WRITE);
		pModel->SetBlndState(EBlndState::ENABLE_BLEND_ALPHA);
	pModelAssembly->AddModel(pModel);
	pModelAssembly->CreateModelHomo();
	for (int i = 0; i < 300; ++i) {
		pObject = new CPointLightObject(PointLight(XMFLOAT4(RandomBetween(0, 1), RandomBetween(0, 1), RandomBetween(0, 1), RandomBetween(1, 10)), RandomBetween(1, 1.5), RandomBetween(5, 300)));
		pObject->SetModelAssembly(pModelAssembly);
		pObject->SetObjectAttribute(EObjectAttribute::PICK, true);
		pObject->SetObjectAttribute(EObjectAttribute::SELECT, true);
		pObject->AddUpdate(UpdateRotateRandom);
		pObject->AddUpdate(UpdateMoveFoward);
		pObject->AddUpdate(UpdateRePosition);
		XMFLOAT3 vPos(RandomBetween(-(g_fWorldSize/2), (g_fWorldSize/2)), 0, RandomBetween(-(g_fWorldSize/2), (g_fWorldSize/2)));
		vPos.y = g_pHeightMap->GetHeight(vPos.x, vPos.z) + 2;
		pObject->SetPosition(vPos);
		m_lpObject.push_back(pObject);
	}

	pModelAssembly = new CModelAssembly;
		pModel = new CModel;
		pModel->SetMesh(new CCrossBillboardMesh(pDevice, 3, 3, 3, 0));
		pModel->SetTexture(Texture(L"light_01"));
		pModel->SetMaterial(Material(L"default"));
		pModel->SetShaderPack(_INT(EVertexElement::POSITION) | _INT(EVertexElement::TEXCOORD1));
		pModel->SetDptstnclState(EDpthStnclState::DEPTH_TEST_NO_WRITE);
		pModel->SetBlndState(EBlndState::ENABLE_BLEND_ALPHA);
	pModelAssembly->AddModel(pModel);
	pModelAssembly->CreateModelHomo();
	pObject = new CSpotLightObject(SpotLight(XMFLOAT4(0.4, 0.4, 0.4, 1), 1, 300, 15, 40));
	pObject->SetModelAssembly(pModelAssembly);
	pObject->SetObjectAttribute(EObjectAttribute::PICK, false);
	pObject->SetObjectAttribute(EObjectAttribute::SELECT, false);
	pObject->SetObjectAttribute(EObjectAttribute::VISIBLE, false);
	pObject->SetPosition(0, 3, -10);
	pObject->SetDirection(XMFLOAT3(0, -1, 1));
	m_lpObject.push_back(pObject);
	pLight = pObject;

	for (int i = 0; i < 300; ++i) {
		pObject = new CSpotLightObject(SpotLight(XMFLOAT4(RandomBetween(0, 1), RandomBetween(0, 1), RandomBetween(0, 1), RandomBetween(1, 10)), RandomBetween(1, 1.5), RandomBetween(40, 300), RandomBetween(10, 40), RandomBetween(40, 80)));
		pObject->SetModelAssembly(pModelAssembly);
		pObject->SetObjectAttribute(EObjectAttribute::PICK, true);
		pObject->SetObjectAttribute(EObjectAttribute::SELECT, true);
		pObject->AddUpdate(UpdateRotateRandom);
		pObject->AddUpdate(UpdateMoveFoward);
		pObject->AddUpdate(UpdateRePosition);
		XMFLOAT3 vPos(RandomBetween(-(g_fWorldSize/2), (g_fWorldSize/2)), RandomBetween(0.1, 10), RandomBetween(-(g_fWorldSize/2), (g_fWorldSize/2)));
		vPos.y = g_pHeightMap->GetHeight(vPos.x, vPos.z) + 2;
		pObject->SetPosition(vPos);
		pObject->SetDirection(0, 0, 1);
		m_lpObject.push_back(pObject);
	}

	CUiObject *pUiObject = UiManager->CreateUi(L"Player",UiElement::TITLE|UiElement::EXIT|UiElement::MINIMIZE|UiElement::RESIZE);
		CUiObject *pChild = new CUiObject;
		pChild->SetSize(0,20);
		pChild->SetData(m_pPlayer);
		pChild->m_uCoordAttri = UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE;
		pChild->SetRenderFunc(RenderObjectPosition);
		pUiObject->AddChild(pChild);
	pUiObject->SetPosition(200,300);
}

void CScene::CreateTerrain(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CModelAssembly *pModelAssembly = nullptr;
	CModel *pModel = nullptr;
	CObject *pObject = nullptr;

	g_pHeightMap = new CHeightMap(pDevice, _T("../../Resource/Texture/Height_Map/terrain_257_2.raw"), HIGHTMAP_SIZE, HIGHTMAP_SIZE, XMFLOAT3{g_fWorldSize/HIGHTMAP_SIZE, 10, g_fWorldSize/HIGHTMAP_SIZE});
	pModelAssembly = new CModelAssembly;
		pModel = new CModel;
		pModel->SetMesh(new CCTerrainMesh(pDevice, 0,0,HIGHTMAP_SIZE,HIGHTMAP_SIZE,g_pHeightMap ));
		pModel->SetTexture(g_pHeightMap ->GetTexture());
		pModel->SetMaterial(Material(L"default"));
		pModel->SetShaderPack(ShaderManager->GetShaderPack(_T("deferred_terrain_normal")));
		pModel->SetDptstnclState(EDpthStnclState::NONE);
		pModel->SetRstrzrState(ERstrzrState::NONE);
		pModel->SetBlndState(EBlndState::NONE);
	pModelAssembly->AddModel(pModel);
	pModelAssembly->CreateModelHomo();
	pObject = new CObject;
	pObject->SetObjectAttribute(EObjectAttribute::VISIBLE_TEST, false);
	pObject->SetObjectAttribute(EObjectAttribute::PICK, false);
	pObject->SetObjectAttribute(EObjectAttribute::SELECT, false);
	pObject->SetModelAssembly(pModelAssembly);
	pObject->SetPosition(0, 0, 0);
	m_lpObject.push_back(pObject);

	pModelAssembly = new CModelAssembly;
		pModel = new CModel;
		pModel->SetMesh(new CSectorMesh(pDevice, g_fWorldSize, g_fWorldSize/2, g_fWorldSize, 4, g_pHeightMap));
		pModel->SetShaderPack(_INT(EVertexElement::POSITION) | _INT(EVertexElement::COLOR));
		pModel->SetDptstnclState(EDpthStnclState::NONE);
		pModel->SetRstrzrState(ERstrzrState::NONE);
		pModel->SetBlndState(EBlndState::NONE);
	pModelAssembly->AddModel(pModel);
	pModelAssembly->CreateModelHomo();
	pObject = new CObject;
	pObject->SetObjectAttribute(EObjectAttribute::RANGE_TEST, false);
	pObject->SetObjectAttribute(EObjectAttribute::PICK, false);
	pObject->SetObjectAttribute(EObjectAttribute::SELECT, false);
	pObject->SetModelAssembly(pModelAssembly);
	pObject->SetPosition(0, 0.01, 0);
	m_lpObject.push_back(pObject);
}

void CScene::DestroyScene() {
	m_pSelectedObject = nullptr;
	m_pPlayer = nullptr;

	for (CObject* pObject : m_lpObject) {
		pObject->Release();
	}
	m_lpObject.clear();
	m_lpShader.clear();
}

void CScene::Update(ID3D11DeviceContext *pDeviceContext, float fElapsedTime) {
	//나중에 Input Manager에서 받아오도록.. 매번 Message Queue를 늘리는 방식은 좋지 않다.
	
	for (CObject* pObject : m_lpObject) {
		pObject->Update(fElapsedTime);
		pObject->AddToRenderer(pCamera);
	}

	ObjectPicking(fElapsedTime);
}

void CScene::Render(ID3D11DeviceContext * pDeviceContext) {
	if (pLight) {
		pLight->SetPosition(pCamera->GetPosition());
		XMFLOAT3 f3Direction;
		XMStoreFloat3(&f3Direction, pCamera->GetAxisZ());
		pLight->SetDirection(f3Direction);
	}
//※ 카메라 Update와 충돌 가능성 농후함.
	pCamera->UpdateConstantBuffer(pDeviceContext);	
}

//매 프레임마다 검사하지 않도록 강제.
void CScene::ObjectPicking(float fElapsedTime){
	static float fCount = 0;
	fCount += fElapsedTime;
	if (fCount < 0.1) return;

	m_pickData[UpdatePivot] = PickingData();

	if(InputFramework->IsMouseActive()){
		const POINT& vMousePos = InputFramework->GetMousePos();
		XMVECTOR vPickOrigin = m_pPlayer->GetCamera()->GetPosition();
		XMVECTOR vPickDirection = m_pPlayer->GetCamera()->GetPickDirection(vMousePos.x, vMousePos.y);

		for(CObject* pObject : m_lpObject)
			if(pObject->ObjectPicking(vPickOrigin,vPickDirection, m_pickData[UpdatePivot]));

		if (m_pickData[UpdatePivot].m_pObject != nullptr) {
			if (!m_pickData[UpdatePivot].m_pObject->GetObjectAttribute(EObjectAttribute::SELECT)) {
				m_pickData[UpdatePivot].m_pObject = nullptr;
			}
			m_pickData[UpdatePivot].m_bPicked = true;
			XMVECTOR vPickedPos = vPickOrigin + (vPickDirection*m_pickData[UpdatePivot].m_fNearestDistance);
			XMStoreFloat3(&m_pickData[UpdatePivot].m_vPickedPosition, vPickedPos);
		}
	}
}

void CScene::DragDropInputProc(HWND hWnd,UINT nMessageID,WPARAM wParam,LPARAM lParam){
	switch(nMessageID){
	}
}

bool CScene::KeyboardInputProc(HWND hWnd,UINT nMessageID,WPARAM wParam,LPARAM lParam){
	switch (nMessageID) {
	case WM_KEYDOWN:
		switch (wParam) {
		}
		return true;
	case WM_KEYUP:
		switch (wParam) {
		case VK_F1:
			InputFramework->MouseOnOff();
			return true;
		case VK_0:
			ShellExecute(NULL, TEXT("explore"), NULL, NULL, TEXT("../../Resource"), SW_SHOW);
			return true;
		case VK_T: {
			//static int num = 1;
			//CRenderer::SwitchToParalell(num);
			//num = 1 - num;
			break;
		}
		case VK_P:
			return true;
		case VK_K:
			return true;
		case VK_ESCAPE:
			return true;
		}
	}
	return false;
}

void CScene::MouseInputProc(HWND hWnd,UINT nMessageID,WPARAM wParam,LPARAM lParam){
	static bool bPick;
	POINT vMousePos = POINT{ LOWORD(lParam), HIWORD(lParam) };

	switch(nMessageID){
	case WM_MOUSEMOVE:
		bPick = false;
		break;
	case WM_RBUTTONDOWN:
		bPick = true;
		break;
	case WM_RBUTTONUP:
		if (bPick) {
			if (m_pickData[UpdatePivot].m_pObject != nullptr) {
				WCHAR wChar[255];
				if (m_pickData[UpdatePivot].m_pObject->GetObjectAttribute(EObjectAttribute::SELECT)) {
					swprintf_s(wChar, L"Object[%x] %+07.2f, %+07.2f, %+07.2f", m_pickData[UpdatePivot].m_pObject, m_pickData[UpdatePivot].m_vPickedPosition.x, m_pickData[UpdatePivot].m_vPickedPosition.y, m_pickData[UpdatePivot].m_vPickedPosition.z);
					SystemMessage(wChar);
					m_pickData[UpdatePivot].m_pObject->CreateUI(vMousePos);
				}
				else {
					swprintf_s(wChar, L"%+07.2f, %+07.2f, %+07.2f", m_pickData[UpdatePivot].m_vPickedPosition.x, m_pickData[UpdatePivot].m_vPickedPosition.y, m_pickData[UpdatePivot].m_vPickedPosition.z);
					SystemMessage(wChar);
				}
			}
		}
		bPick = false;
		break;
	case WM_LBUTTONDOWN:
		bPick = true;
		break;
	case WM_LBUTTONUP:
		if (bPick) {
			if (m_pickData[UpdatePivot].m_pObject != nullptr) {
				WCHAR wChar[255];
				if (m_pickData[UpdatePivot].m_pObject->GetObjectAttribute(EObjectAttribute::SELECT)) {
					swprintf_s(wChar, L"Object[%x] %+07.2f, %+07.2f, %+07.2f", m_pickData[UpdatePivot].m_pObject, m_pickData[UpdatePivot].m_vPickedPosition.x, m_pickData[UpdatePivot].m_vPickedPosition.y, m_pickData[UpdatePivot].m_vPickedPosition.z);
					SystemMessage(wChar);
					m_pickData[UpdatePivot].m_pModel->CreateUI(vMousePos);
				}
				else {
					swprintf_s(wChar, L"%+07.2f, %+07.2f, %+07.2f", m_pickData[UpdatePivot].m_vPickedPosition.x, m_pickData[UpdatePivot].m_vPickedPosition.y, m_pickData[UpdatePivot].m_vPickedPosition.z);
					SystemMessage(wChar);
				}
			}
		}
		bPick = false;
		break;
	}
}
