#include "stdafx.h"
#include "Renderer.h"
#include "Model.h"
#include "ModelHomo.h"
#include "ModelAssembly.h"
#include "UiObject.h"
#include "ConstantBuffer.h"
#include "Scene.h"
#include "Shader.h"
#include "RenderState.h"

void CRenderer::Initialize(UINT tRenderer) {
	UINT idShader						= (tRenderer % _INT(ERendererStateSlot::DPTH_STNCL)) / _INT(ERendererStateSlot::SHADER);
	EDpthStnclState	eDpthStnclState		= static_cast<EDpthStnclState>((tRenderer % _INT(ERendererStateSlot::RSTRZR)) / _INT(ERendererStateSlot::DPTH_STNCL));
	ERstrzrState eRstrzrSate			= static_cast<ERstrzrState>((tRenderer % _INT(ERendererStateSlot::BLND)) / _INT(ERendererStateSlot::RSTRZR));
	EBlndState eBlndState				= static_cast<EBlndState>((tRenderer % _INT(ERendererStateSlot::SHADOW)) / _INT(ERendererStateSlot::BLND));
	bool bShadow;

	m_tRenderer = tRenderer;
	m_pShaderPack = ShaderManager->GetShaderPack(idShader);
	m_pDpthStnclState = DEPTH_STENCIL_STATE[eDpthStnclState];
	m_pRstrzrState = RASTERIZER_STATE[eRstrzrSate];
	m_pBlndState = BLEND_STATE[eBlndState];
}

void CRenderer::CreateUI(const POINT & vMousePos) {
	CUiObject *m_pUiObject = UiManager->CreateUi(L"Renderer", UiElement::TITLE | UiElement::EXIT | UiElement::MINIMIZE | UiElement::RESIZE | UiElement::ON_TOP);
	CUiObject *pChild = new CUiObject;
	pChild->SetData(this);
	pChild->m_uCoordAttri = UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE;
	pChild->SetRenderFunc(RenderAddress);
	m_pUiObject->AddChild(pChild);
	pChild = m_pShaderPack->CreateUI(POINT{ 0, 0 }, UiElement::TITLE, false);
	pChild->SetTitle(_T("Shader"));
	m_pUiObject->AddChild(pChild);
	m_pUiObject->SetPosition(vMousePos.x, vMousePos.y);
}

void CRenderer::SwitchToParalell(bool bFlag) {
	if (bFlag) {
		FuncAddRenderData = ParallelAddRenderData;
		FuncClearRenderData = ParallelClearRenderData;
		FuncUpdate = ParallelUpdate;
		FuncRender = ParallelRender;
		SystemMessage(_T("Parallel Framework"));
	}
	else {
		FuncAddRenderData = NormalAddRenderData;
		FuncClearRenderData = NormalClearRenderData;
		FuncUpdate = NormalUpdate;
		FuncRender = NormalRender;
		SystemMessage(_T("Normal Framework"));
	}
}

void CRenderer::SetEngaged(bool bFlag) {
	m_bEngaged = bFlag;
	if (bFlag)
		CScene::m_nRenderer++;
	else
		CScene::m_nRenderer--;
}

EPixelShaderType CRenderer::GetPixelShaderType() {
	return m_pShaderPack->GetPixelShaderType();
}

void CRenderer::NormalAddRenderData(CRenderer * pThis, CModelHomo * pModelHomo, const XMFLOAT4X4 & mtxWorld) {
	if (pThis->m_pvtRenderData[0] == pThis->m_vRenderData[0].size()) {
		pThis->m_vRenderData[0].push_back(std::make_pair(pModelHomo, mtxWorld));
		pThis->m_pvtRenderData[0]++;
	}
	else {
		pThis->m_vRenderData[0][pThis->m_pvtRenderData[0]] = std::make_pair(pModelHomo, mtxWorld);
		pThis->m_pvtRenderData[0]++;
	}
}

void CRenderer::NormalClearRenderData(CRenderer * pThis) {
	pThis->m_pvtRenderData[0] = 0;
}

void CRenderer::NormalUpdate(CRenderer * pThis, float fElapsedTime) {
	if (pThis->m_pvtRenderData[0] == 0) {
		pThis->m_fQueueTimer += fElapsedTime;
		if (g_MaxWaitTime < pThis->m_fQueueTimer) {
			pThis->m_fQueueTimer = 0;
			pThis->SetEngaged(false);
		}
	}
	else
		pThis->m_fQueueTimer = 0;
}


void CRenderer::NormalRender(CRenderer * pThis, ID3D11DeviceContext * pDeviceContext) {
	pThis->m_pShaderPack->PrepareRender(pDeviceContext);
	pDeviceContext->RSSetState(pThis->m_pRstrzrState);
	pDeviceContext->OMSetDepthStencilState(pThis->m_pDpthStnclState, 1);
	pDeviceContext->OMSetBlendState(pThis->m_pBlndState, NULL, 0xffffffff);

	for (int i = 0; i<pThis->m_pvtRenderData[0]; ++i) {
		UpdateWorldConstant(pDeviceContext, pThis->m_vRenderData[0][i].second);
		pThis->m_vRenderData[0][i].first->Render(pDeviceContext);
	}
}

void CRenderer::ParallelAddRenderData(CRenderer * pThis, CModelHomo * pModelHomo, const XMFLOAT4X4 & mtxWorld) {
	if (pThis->m_pvtRenderData[UpdatePivot] == pThis->m_vRenderData[UpdatePivot].size()) {
		pThis->m_vRenderData[UpdatePivot].push_back(std::make_pair(pModelHomo, mtxWorld));
		pThis->m_pvtRenderData[UpdatePivot]++;
	}
	else {
		pThis->m_vRenderData[UpdatePivot][pThis->m_pvtRenderData[UpdatePivot]] = std::make_pair(pModelHomo, mtxWorld);
		pThis->m_pvtRenderData[UpdatePivot]++;
	}
}

void CRenderer::ParallelClearRenderData(CRenderer * pThis) {
	pThis->m_pvtRenderData[UpdatePivot] = 0;
}

void CRenderer::ParallelUpdate(CRenderer * pThis, float fElapsedTime) {
	if (pThis->m_pvtRenderData[UpdatePivot] == 0) {
		pThis->m_fQueueTimer += fElapsedTime;
		if (g_MaxWaitTime < pThis->m_fQueueTimer) {
			pThis->m_fQueueTimer = 0;
			pThis->m_bEngaged = false;
			CScene::m_nRenderer--;
		}
	}
	else
		pThis->m_fQueueTimer = 0;
}

void CRenderer::ParallelRender(CRenderer * pThis, ID3D11DeviceContext * pDeviceContext) {
	pThis->m_pShaderPack->PrepareRender(pDeviceContext);
	pDeviceContext->RSSetState(pThis->m_pRstrzrState);
	pDeviceContext->OMSetDepthStencilState(pThis->m_pDpthStnclState, 1);
	pDeviceContext->OMSetBlendState(pThis->m_pBlndState, NULL, 0xffffffff);

	for (int i = 0; i<pThis->m_pvtRenderData[RenderPivot]; ++i) {
		UpdateWorldConstant(pDeviceContext, pThis->m_vRenderData[RenderPivot][i].second);
		pThis->m_vRenderData[RenderPivot][i].first->Render(pDeviceContext);
	}
}

void CRendererManager::Initialize() {
	m_pRendererManagerUi = UiManager->CreateUi(L"Renderer Manager", UiElement::TITLE | UiElement::EXIT | UiElement::MINIMIZE | UiElement::RESIZE);
	m_pRendererManagerUi->SetPosition(0, 600);
}

void CRendererManager::CreateUI(const POINT & vMousePos) {
	if (m_pRendererManagerUi) return;

	m_pRendererManagerUi = UiManager->CreateUi(L"Renderer Manager", UiElement::TITLE | UiElement::EXIT | UiElement::MINIMIZE | UiElement::RESIZE);
	m_pRendererManagerUi->SetOuterInterface(&m_pRendererManagerUi);
}

//---------------------------------------------------------------------------------------------------------
CRenderer * CRendererManager::CreateRenderer(UINT tRenderer) {
	CRenderer* pRenderer = new CRenderer;
	pRenderer->Initialize(tRenderer);
	m_mpRenderer[tRenderer] = pRenderer;

	CUiObject *pChild = new CUiObject(UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE);
	pChild->SetTitle(L"Renderer Pivot : ");
	pChild->SetSize(20, 20);
	pChild->SetData(pRenderer->GetpvtRendererData());
	pChild->SetRenderFunc(RenderInt);
	m_pRendererManagerUi->AddChild(pChild);
	return pRenderer;
}

void CRendererManager::AddToRendererList(CRenderer * pRenderer) {
	if (pRenderer->GetEngaged()) return;

	switch (pRenderer->GetPixelShaderType()) {
	case EPixelShaderType::NONE:
		MyMessageBox(_T("This Shader is not for the Renderer!!!"));
		break;
	case EPixelShaderType::SKYBOX:
		m_pSkyboxRenderer = pRenderer;
		break;
	case EPixelShaderType::DEFERRED:
		m_vpDeferredRenderer.push_back(pRenderer);
		break;
	case EPixelShaderType::FOWARD:
		if (pRenderer->GetBlendState())
			m_vpBlendRenderer.push_back(pRenderer);
		else
			m_vpFowardRenderer.push_back(pRenderer);
		break;
//	case EPixelShaderType::BLEND:
//		break;
	}
	pRenderer->SetEngaged(true);
}

CRenderer * CRendererManager::GetRenderer(UINT tRenderer) {
	CRenderer* pRenderer = m_mpRenderer[tRenderer];

	//Create Renderer if Empty
	if (!pRenderer)
		pRenderer = CreateRenderer(tRenderer);

	//Add To Renderer List
	AddToRendererList(pRenderer);

	return pRenderer;
}
