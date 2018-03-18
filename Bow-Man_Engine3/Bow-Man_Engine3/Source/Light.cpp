#include "stdafx.h"
#include "ConstantBuffer.h"
#include "UtilityFunction.h"

#include "RenderState.h"
#include "Scene.h"

#include "Light.h"
#include "Camera.h"

#include "Shader.h"
#include "ModelAssembly.h"

void CDirLightObject::AddToRenderer(CCameraObject * pCamera)
{
	if (!m_ObjectAttribute.m_bVisible) return;

	LightManager->AddLight(this);

	if(!pCamera->IsInFrustum(m_AABB)) return;

	if (m_ObjectAttribute.m_bRangeVisibleTest)
		if (!pCamera->IsInRange(GetPosition())) return;

	if (m_ObjectAttribute.m_bFrustumVisibleTest) {
		if (!pCamera->IsInFrustum(m_AABB)) return;
	}
	m_pModelAssembly->AddToRenderer(m_mtxWorld);
	CScene::m_nRenderedObject++;
}

void CPointLightObject::CreateUI(const POINT & vMousePos) {
	CUiObject *m_pUiObject = UiManager->CreateUi(L"Light Object", UiElement::TITLE | UiElement::EXIT | UiElement::MINIMIZE | UiElement::RESIZE | UiElement::ON_TOP);
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
	pChild->SetTitle(_T("Range : "));
	pChild->SetData(&m_LightData.m_fRange);
	pChild->m_uCoordAttri = UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE;
	pChild->SetRenderFunc(RenderFloat);
	m_pUiObject->AddChild(pChild);
	pChild = new CUiObject;
	pChild->SetTitle(_T("Power : "));
	pChild->SetData(&m_LightData.m_fPower);
	pChild->m_uCoordAttri = UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE;
	pChild->SetRenderFunc(RenderFloat);
	m_pUiObject->AddChild(pChild);

	m_pUiObject->SetSize(200, 200);
	m_pUiObject->SetPosition(vMousePos.x, vMousePos.y);
}

void CPointLightObject::AddToRenderer(CCameraObject * pCamera)
{
	if (!m_ObjectAttribute.m_bVisible) return;
	if (m_ObjectAttribute.m_bRangeVisibleTest) {
		if (!pCamera->IsInRange(GetPosition())) return;

		if (m_ObjectAttribute.m_bFrustumVisibleTest) {
			BoundingBox aabbTransposed;
			m_AABB.Extents = XMFLOAT3(m_LightData.m_fRange, m_LightData.m_fRange, m_LightData.m_fRange);
			m_AABB.Center = { 0, 0, 0 };
			m_AABB.Transform(aabbTransposed, GetWorldMatrix());
			if (!pCamera->IsInFrustum(aabbTransposed)) return;
		}
	}

	LightManager->AddLight(this);

	m_pModelAssembly->AddToRenderer(m_mtxWorld);
	CScene::m_nRenderedObject++;
	CScene::m_nPointLight++;
}


void CSpotLightObject::AddToRenderer(CCameraObject * pCamera)
{
	if (!m_ObjectAttribute.m_bVisible) return;

	if (m_ObjectAttribute.m_bRangeVisibleTest) {
		if (!pCamera->IsInRange(GetPosition())) return;

		if (m_ObjectAttribute.m_bFrustumVisibleTest) {
			BoundingSphere boundingSphere(GetF3Position(), m_LightData.m_fRange);
			if (!pCamera->IsInFrustum(boundingSphere)) return;
		}
	}

	LightManager->AddLight(this);

	m_pModelAssembly->AddToRenderer(m_mtxWorld);
	CScene::m_nRenderedObject++;
	CScene::m_nSpotLight++;
}

void CLightManager::CreateInstance() {
	if (m_pInstance) MyMessageBox(__FILE__, " : ", __LINE__, "\n", __FUNCTION__, "\nLightEngine Instance Not Nullptr");
	m_pInstance = new CLightManager;
}

void CLightManager::Initialize(ID3D11Device * pDevice)
{
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(CLightData) * 1;
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = &m_arrLightData;
	pDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pcbLight);

	m_pDirLightShaderPack = ShaderManager->GetShaderPack(_T("DirLightProcess"));
	m_pPointLightShaderPack = ShaderManager->GetShaderPack(_T("PointLightProcess"));
	m_pSpotLightShaderPack = ShaderManager->GetShaderPack(_T("SpotLightProcess"));
}

void CLightManager::UpdateConstantBuffer(ID3D11DeviceContext * pDeviceContext) {
	::UpdateConstantBuffer(pDeviceContext, m_pcbLight, m_arrLightData[RenderPivot]);
	pDeviceContext->HSSetConstantBuffers(CB_Slot::LIGHT, 1, &m_pcbLight);
	pDeviceContext->DSSetConstantBuffers(CB_Slot::LIGHT, 1, &m_pcbLight);
	pDeviceContext->PSSetConstantBuffers(CB_Slot::LIGHT, 1, &m_pcbLight);
}

void CLightManager::LightProcess(ID3D11DeviceContext * pDeviceContext) {
	m_pDirLightShaderPack->PrepareRender(pDeviceContext);
	m_pDirLightShaderPack->QuadRenderInstance(pDeviceContext, m_arrLightData[RenderPivot].m_nLight.NUM_OF_DIR);

	pDeviceContext->RSSetState(RASTERIZER_STATE[ERstrzrState::SOLID_FRONT_CULL]);
	pDeviceContext->OMSetBlendState(BLEND_STATE[EBlndState::ONE_BLEND], NULL, 0xffffffff);
	pDeviceContext->OMSetDepthStencilState(DEPTH_STENCIL_STATE[EDpthStnclState::GRT_EQUAL], 1);

	m_pPointLightShaderPack->PrepareRender(pDeviceContext);
	m_pPointLightShaderPack->PointRenderInstance(pDeviceContext, 2, m_arrLightData[RenderPivot].m_nLight.NUM_OF_POINT);

	m_pSpotLightShaderPack->PrepareRender(pDeviceContext);
	m_pSpotLightShaderPack->PointRenderInstance(pDeviceContext, 1, m_arrLightData[RenderPivot].m_nLight.NUM_OF_SPOT);

	m_arrLightData[RenderPivot].FlushLight();
}
