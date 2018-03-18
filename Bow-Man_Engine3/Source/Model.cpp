#include "stdafx.h"
#include "Model.h"
#include "Mesh.h"

#include "UiObject.h"
#include "Picking.h"

#include "Shader.h"
#include "Material.h"
#include "Texture.h"

#include "Scene.h"

void CModel::CalModelID() {
	m_tRenderer = 0;
	m_tRenderer += m_pShaderPack->GetID()	* _INT(ERendererStateSlot::SHADER);
	m_tRenderer += _INT(m_eDpthStnclState)	* _INT(ERendererStateSlot::DPTH_STNCL);
	m_tRenderer += _INT(m_eRstrzrSate)		* _INT(ERendererStateSlot::RSTRZR);
	m_tRenderer += _INT(m_eBlndState)		* _INT(ERendererStateSlot::BLND);
	m_tRenderer += _INT(m_bShadow)			* _INT(ERendererStateSlot::SHADOW);
}

void CModel::Release() {
	m_pMesh->Release();
	m_pTexture->Release();
	m_pMaterial->Release();
}

void CModel::CreateUI(const POINT & vMousePos) {
	CUiObject *pUiObject = UiManager->CreateUi(L"Model", UiElement::TITLE | UiElement::EXIT | UiElement::MINIMIZE | UiElement::RESIZE | UiElement::ON_TOP);
	pUiObject->SetSizeX(200);
	CUiObject *pChild = new CUiObject(UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE);
	pChild->SetTitle(_T("Renderer Type : "));
	pChild->SetData(&m_tRenderer);
	pChild->SetSize(0, 20);
	pChild->SetRenderFunc(RenderInt);
	pUiObject->AddChild(pChild);
	if (m_pMesh) {
		pChild = new CUiObject(UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE);
		pChild->SetTitle(_T("Mesh"));
		pChild->SetContent(m_pMesh->GetName());
		pChild->SetData(m_pMesh);
		pChild->SetSize(0, 20);
		pChild->SetRenderFunc(RenderElement);
		pUiObject->AddChild(pChild);
	}
	if (m_pTexture) {
		pChild = new CUiObject(UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE);
		pChild->SetTitle(_T("Texture"));
		pChild->SetData(&m_pTexture);
		pChild->SetSize(0, 100);
		pChild->SetRenderFunc(RenderTextureAddress);
		pChild->SetMouseInputFunc(MouseInputTextureAddress);
		pUiObject->AddChild(pChild);
	}
	if (m_pMaterial) {
		pChild = new CUiObject(UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE);
		pChild->SetTitle(_T("Material"));
		pChild->SetData(&m_pMaterial);
		pChild->SetSize(0, 100);
		pChild->SetRenderFunc(RenderMaterialAddress);
		pUiObject->AddChild(pChild);
	}
	pChild = new CUiObject(UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE);
	pChild->SetTitle(_T("ShaderPack"));
	pChild->SetData(&m_pShaderPack);
	pChild->SetData(this, 1);
	pChild->SetSize(0, 20);
	pChild->SetRenderFunc(RenderShaderAddress);
	pChild->SetMouseInputFunc(MouseInputShaderAddress);
	pUiObject->AddChild(pChild);
	pChild = new CUiObject(UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE);
	pChild->SetTitle(_T("DpthStncl"));
	pChild->SetContent(gsDpthStnclState[m_eDpthStnclState]);
	pChild->SetData(this);
	pChild->SetSize(0, 20);
	pChild->SetRenderFunc(RenderElement);
	pChild->SetMouseInputFunc(MouseInputDpthStncl);
	pUiObject->AddChild(pChild);
	pChild = new CUiObject(UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE);
	pChild->SetTitle(_T("Rstrzr"));
	pChild->SetContent(gsRstrzrState[m_eRstrzrSate]);
	pChild->SetData(this);
	pChild->SetSize(0, 20);
	pChild->SetRenderFunc(RenderElement);
	pChild->SetMouseInputFunc(MouseInputRstrzr);
	pUiObject->AddChild(pChild);
	pChild = new CUiObject(UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE);
	pChild->SetTitle(_T("Blnd"));
	pChild->SetContent(gsBlndState[m_eBlndState]);
	pChild->SetData(this);
	pChild->SetSize(0, 20);
	pChild->SetRenderFunc(RenderElement);
	pChild->SetMouseInputFunc(MouseInputBlnd);
	pUiObject->AddChild(pChild);
	pUiObject->SetPosition(vMousePos.x, vMousePos.y);
}

void CModel::Render(ID3D11DeviceContext * pDeviceContext) {
	(m_pTexture)	? m_pTexture->SetResourceToDevice(pDeviceContext) : NULL;
	(m_pMaterial)	? m_pMaterial->SetResourceToDevice(pDeviceContext) : NULL;
	(m_pMeshBuffer)	? m_pMeshBuffer->Render(pDeviceContext) : NULL;

	CScene::m_nRenderedModel++;
}

void CModel::Render2(ID3D11DeviceContext * pDeviceContext)
{
	m_pShaderPack->PrepareRender(pDeviceContext);
	(m_pTexture) ? m_pTexture->SetResourceToDevice(pDeviceContext) : NULL;
	(m_pMaterial) ? m_pMaterial->SetResourceToDevice(pDeviceContext) : NULL;
	m_pMesh->Render(pDeviceContext);
	CScene::m_nRenderedModel++;
}

void CModel::SetShaderPack(CShaderPack * pShader) {
	if (!m_pMesh) return;
	m_pMeshBuffer = m_pMesh->GetMeshBuffer(pShader->GetInputLayout());
	m_pShaderPack = pShader;
}

void CModel::SetShaderPack(UINT uInputLayout) {
	if (!m_pMesh) return;
	m_pMeshBuffer = m_pMesh->GetMeshBuffer(uInputLayout);
	m_pShaderPack = ShaderManager->GetDefaultShaderPack(uInputLayout);
}

void CModel::SetShaderPack(UINT uInputLayout, std::string sName) {
	if (!m_pMesh) return;
}

void CModel::SetMesh(CMesh * pMesh) {
	if (pMesh) m_pMesh = pMesh;
	m_pMeshBuffer = m_pMesh->GetMeshBuffer();
	UINT uInputLayout = m_pMeshBuffer->GetInputLayout();
	m_pShaderPack = ShaderManager->GetDefaultShaderPack(uInputLayout);
}

bool CModel::ModelPicking(const XMVECTOR & vRayPosition, const XMVECTOR & vRayDirection, PickingData & pickData) {
	if (m_pMesh->MeshPicking(vRayPosition, vRayDirection, pickData)) {
		pickData.m_pModel = this;
		return true;
	}
	return false;
}

UINT CModel::GetShaderPackID() {
	return m_pShaderPack->GetID();
}
