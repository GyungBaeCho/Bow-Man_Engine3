#include "stdafx.h"
#include "ModelHomo.h"
#include "Model.h"
#include "Renderer.h"
#include "Scene.h"

void CModelHomo::Release() {
	m_pRenderer->ClearRenderData();
	delete this;
}

void CModelHomo::Render(ID3D11DeviceContext * pDeviceContext) {
	//const auto &end = m_vpModel.end();
	//for (auto& pivot = m_vpModel.begin(); pivot != end; ++pivot) {
	//	(*pivot)->Render(pDeviceContext);
	//}
	int nModel = m_vpModel.size();
	for (int i = 0; i < nModel; ++i)
		m_vpModel[i]->Render(pDeviceContext);;
}

void CModelHomo::AddToRenderData(const XMFLOAT4X4& mtxWorld)
{
//	m_pRenderer->AddRenderData(this, mtxWorld);
	RendererManager->AddRenderData(m_pRenderer, this, mtxWorld);
	CScene::m_nHomoModel++;
}

void CModelHomo::SetRenderer(UINT tRenderer) {
	m_tRenderer = tRenderer;
	m_pRenderer = RendererManager->GetRenderer(tRenderer);
}

void CModelHomo::SetRenderer(CRenderer * pRenderer) {
	m_pRenderer = pRenderer;
}



