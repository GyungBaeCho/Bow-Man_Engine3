#include "stdafx.h"
#include "Object.h"
#include "ModelAssembly.h"
#include "ConstantBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "Framework.h"
#include "Camera.h"
#include "Scene.h"
#include "Picking.h"
#include "UiObject.h"
#include "UtilityFunction.h"
#include "ObjectStrategy.h"
#include "RenderState.h"

void CObject::CreateUI(const POINT & vMousePos) {
	CUiObject *m_pUiObject = UiManager->CreateUi(L"Object", UiElement::TITLE | UiElement::EXIT | UiElement::MINIMIZE | UiElement::RESIZE | UiElement::ON_TOP);
	m_pUiObject->SetSizeX(200);
	CUiObject *pChild = new CUiObject;
	pChild->SetData(this);
	pChild->m_uCoordAttri = UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE;
	pChild->SetRenderFunc(RenderAddress);
	m_pUiObject->AddChild(pChild);
	pChild = new CUiObject;
	pChild->SetData(this);
	pChild->m_uCoordAttri = UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE;
	pChild->SetRenderFunc(RenderObjectPosition);
	m_pUiObject->AddChild(pChild);
	pChild = new CUiObject;
	pChild->SetTitle(_T("Visible : "));
	pChild->SetData(&m_ObjectAttribute.m_bVisible);
	pChild->m_uCoordAttri = UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE;
	pChild->SetRenderFunc(RenderBoolElement);
	pChild->SetMouseInputFunc(MouseInputBool);
	m_pUiObject->AddChild(pChild);
	pChild = new CUiObject;
	pChild->SetTitle(_T("Pickable : "));
	pChild->SetData(&m_ObjectAttribute.m_bPickAble);
	pChild->m_uCoordAttri = UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE;
	pChild->SetRenderFunc(RenderBoolElement);
	pChild->SetMouseInputFunc(MouseInputBool);
	m_pUiObject->AddChild(pChild);
	pChild = new CUiObject;
	pChild->SetTitle(_T("Selectable : "));
	pChild->SetData(&m_ObjectAttribute.m_bSelectAble);
	pChild->m_uCoordAttri = UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE;
	pChild->SetRenderFunc(RenderBoolElement);
	pChild->SetMouseInputFunc(MouseInputBool);
	m_pUiObject->AddChild(pChild);
	pChild = new CUiObject;
	pChild->SetTitle(_T("Visible Test : "));
	pChild->SetData(&m_ObjectAttribute.m_bVisibleTest);
	pChild->m_uCoordAttri = UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE;
	pChild->SetRenderFunc(RenderBoolElement);
	pChild->SetMouseInputFunc(MouseInputBool);
	m_pUiObject->AddChild(pChild);
	pChild = new CUiObject;
	pChild->SetTitle(_T("Range Visible Test : "));
	pChild->SetData(&m_ObjectAttribute.m_bRangeVisibleTest);
	pChild->m_uCoordAttri = UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE;
	pChild->SetRenderFunc(RenderBoolElement);
	pChild->SetMouseInputFunc(MouseInputBool);
	m_pUiObject->AddChild(pChild);
	pChild = new CUiObject;
	pChild->SetTitle(_T("Frustum Visible Test : "));
	pChild->SetData(&m_ObjectAttribute.m_bFrustumVisibleTest);
	pChild->m_uCoordAttri = UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE;
	pChild->SetRenderFunc(RenderBoolElement);
	pChild->SetMouseInputFunc(MouseInputBool);
	m_pUiObject->AddChild(pChild);
	pChild = m_pModelAssembly->CreateUI(POINT{ 0, 0 }, UiElement::TITLE, false);
	pChild->SetTitle(_T("Model Assembly"));
	m_pUiObject->AddChild(pChild);
	m_pUiObject->SetPosition(vMousePos.x, vMousePos.y);
}

void CObject::Rotate(const XMFLOAT3& d3dxvAxis,float fDegree){
	XMMATRIX mtxRotate;
	mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&d3dxvAxis), XMConvertToRadians(fDegree));
	mtxRotate = XMMatrixMultiply(mtxRotate, GetWorldMatrix());
	XMStoreFloat4x4(&m_mtxWorld, mtxRotate);
}
void CObject::Rotate(AXIS axis, float fDegree){
	XMMATRIX mtxRotate;

	XMFLOAT3 d3dxvAxis;
	switch(axis){
	case AXIS::X: d3dxvAxis = XMFLOAT3(1, 0, 0); break;
	case AXIS::Y: d3dxvAxis = XMFLOAT3(0, 1, 0); break;
	case AXIS::Z: d3dxvAxis = XMFLOAT3(0, 0, 1); break;
	}
	mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&d3dxvAxis), XMConvertToRadians(fDegree));
	mtxRotate = XMMatrixMultiply(mtxRotate, GetWorldMatrix());
	XMStoreFloat4x4(&m_mtxWorld, mtxRotate);
}
void CObject::Rotate(float xfDgree, float yfDgree, float zfDgree){
	XMMATRIX mtxRotate;
	mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(xfDgree), XMConvertToRadians(yfDgree), XMConvertToRadians(zfDgree));
	mtxRotate = XMMatrixMultiply(mtxRotate, GetWorldMatrix());
	XMStoreFloat4x4(&m_mtxWorld, mtxRotate);
}

//World Matrix를 서로 직교화 시켜준다.
void CObject::OrthogonolizeWorldMatrix(){
	//XMFLOAT3 d3dxvRight = GetAxisVector(AXIS::X);
	//XMFLOAT3 d3dxvUp = GetAxisVector(AXIS::Y);
	//XMFLOAT3 d3dxvLook = GetAxisVector(AXIS::Z);

	//D3DXVec3Normalize(&d3dxvLook, &d3dxvLook);
	//D3DXVec3Cross(&d3dxvRight, &d3dxvUp, &d3dxvLook);
	//D3DXVec3Normalize(&d3dxvRight, &d3dxvRight);
	//D3DXVec3Cross(&d3dxvUp, &d3dxvLook, &d3dxvRight);
	//D3DXVec3Normalize(&d3dxvUp, &d3dxvUp);

	//SetAxisVector(AXIS::X, d3dxvRight);
	//SetAxisVector(AXIS::Y, d3dxvUp);
	//SetAxisVector(AXIS::Z, d3dxvLook);
}

void CObject::SetModelAssembly(CModelAssembly * modelAssembly) {
	m_pModelAssembly = modelAssembly;
	m_AABB = modelAssembly->GetAABB();
	m_AABB.Transform(m_AABB, GetWorldMatrix());
}

void CObject::AddToRenderer(CCameraObject * pCamera){
	if (!m_ObjectAttribute.m_bVisible) return;

	if (m_ObjectAttribute.m_bVisibleTest) {
		if (m_ObjectAttribute.m_bRangeVisibleTest)
			if (!pCamera->IsInRange(GetPosition())) return;

		if (m_ObjectAttribute.m_bFrustumVisibleTest)
			if (!pCamera->IsInFrustum(m_AABB)) return;
	}

	m_pModelAssembly->AddToRenderer(m_mtxWorld);
	CScene::m_nRenderedObject++;
}

void CObject::Render(ID3D11DeviceContext *pDeviceContext,CCameraObject *pCamera){
	if(!m_ObjectAttribute.m_bVisible) return;
	if(!pCamera->IsInFrustum(m_AABB)) return;

	UpdateWorldConstant(pDeviceContext, m_mtxWorld);
	
	pDeviceContext->RSSetState(m_pRasterizerState);
	pDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 1);
	pDeviceContext->OMSetBlendState(m_pBlendState, NULL, 0xffffffff);

	m_pModelAssembly->Render(pDeviceContext);
	CScene::m_nRenderedObject++;
}

bool CObject::ObjectPicking(const XMVECTOR & vRayPosition,const XMVECTOR & vRayDirection,PickingData& pickData){
	if(!GetObjectAttribute(EObjectAttribute::PICK)) return false;

	XMVECTOR vOperatedRayPosition;
	XMVECTOR vOperatedRayDirection;
	XMFLOAT4X4 mtxInverseWorld;
	mtxInverseWorld._11 = m_mtxWorld._11;
	mtxInverseWorld._12 = m_mtxWorld._21;
	mtxInverseWorld._13 = m_mtxWorld._31;
	mtxInverseWorld._14 = m_mtxWorld._14;

	mtxInverseWorld._21 = m_mtxWorld._12;
	mtxInverseWorld._22 = m_mtxWorld._22;
	mtxInverseWorld._23 = m_mtxWorld._32;
	mtxInverseWorld._24 = m_mtxWorld._24;

	mtxInverseWorld._31 = m_mtxWorld._13;
	mtxInverseWorld._32 = m_mtxWorld._23;
	mtxInverseWorld._33 = m_mtxWorld._33;
	mtxInverseWorld._34 = m_mtxWorld._34;

	mtxInverseWorld._41 = m_mtxWorld._41;
	mtxInverseWorld._42 = m_mtxWorld._42;
	mtxInverseWorld._43 = m_mtxWorld._43;
	mtxInverseWorld._44 = m_mtxWorld._44;
					    
	XMVECTOR vVector;
	vVector = -XMVector3Dot(GetPosition(),GetAxisX());
	XMStoreFloat(&mtxInverseWorld._41,vVector);
	vVector = -XMVector3Dot(GetPosition(),GetAxisY());
	XMStoreFloat(&mtxInverseWorld._42,vVector);
	vVector = -XMVector3Dot(GetPosition(),GetAxisZ());
	XMStoreFloat(&mtxInverseWorld._43,vVector);

	vOperatedRayPosition =  XMVector3TransformCoord(vRayPosition,XMLoadFloat4x4(&mtxInverseWorld));
	vOperatedRayDirection = XMVector3TransformNormal(vRayDirection,XMLoadFloat4x4(&mtxInverseWorld));
	vOperatedRayDirection = XMVector3Normalize(vOperatedRayDirection);

	float fResultDistance;
	if (!m_pModelAssembly->GetAABB().Intersects(vOperatedRayPosition, vOperatedRayDirection, fResultDistance)) return false;
	if (pickData.m_fNearestDistance < fResultDistance) return false;

	pickData.m_mtxWorld = GetWorldMatrix();
	if (m_pModelAssembly->ModelAssemblyPicking(vOperatedRayPosition, vOperatedRayDirection, pickData)) {
		pickData.m_pObject = this;
		return true;
	}
	return false;
}

void CObject::SetRState(ERstrzrState RS){
	m_pRasterizerState = RASTERIZER_STATE[RS];
}

void CObject::SetDSState(EDpthStnclState DSS){
	m_pDepthStencilState = DEPTH_STENCIL_STATE[DSS];
}

void CObject::SetBState(EBlndState BS){
	m_pBlendState = BLEND_STATE[BS];
}

void CObject::SetPosition(const XMFLOAT3 & vPosition) {
	m_mtxWorld._41 = vPosition.x;
	m_mtxWorld._42 = vPosition.y;
	m_mtxWorld._43 = vPosition.z;
	m_pModelAssembly->GetAABB().Transform(m_AABB, GetWorldMatrix());
}

void CObject::SetPosition(const XMVECTOR & vVector) {
	XMFLOAT3 vPosition;
	XMStoreFloat3(&vPosition, vVector);
	m_mtxWorld._41 = vPosition.x;
	m_mtxWorld._42 = vPosition.y;
	m_mtxWorld._43 = vPosition.z;
	m_pModelAssembly->GetAABB().Transform(m_AABB, GetWorldMatrix());
}

void CObject::SetPosition(float x, float y, float z) {
	m_mtxWorld._41 = x;
	m_mtxWorld._42 = y;
	m_mtxWorld._43 = z;
	m_pModelAssembly->GetAABB().Transform(m_AABB, GetWorldMatrix());
}

