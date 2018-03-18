#include "stdafx.h"
#include "RenderState.h"

void CRenderStateManager::CreateAddState(ID3D11Device * pDevice, const D3D11_DEPTH_STENCIL_DESC * pState, const std::wstring & wsName) {
	if (m_mpDs[wsName] != nullptr) {
		MyMessageBox(_T("DepthStencilState Already Exists! : ["), wsName, _T("]"));
		return;
	}
	AddState(new CDepthStencilState(pDevice, pState, wsName));
}

void CRenderStateManager::CreateAddState(ID3D11Device * pDevice, const D3D11_RASTERIZER_DESC * pState, const std::wstring & wsName) {
	if (m_mpRs[wsName] != nullptr) {
		MyMessageBox(_T("RasterizerState Already Exists! : ["), wsName, _T("]"));
		return;
	}
	AddState(new CRaserizerState(pDevice, pState, wsName));
}

void CRenderStateManager::AddState(CRenderState * pRenderState) {
	const std::wstring& wsName = pRenderState->GetName();

	switch (pRenderState->GetStateType()) {
	case EStateType::NONE:
		break;
	case EStateType::DEPTHSTENCIL:
		if (m_mpDs[wsName] == nullptr) {
			m_mpDs[wsName] = pRenderState;
			m_vpDs.push_back(pRenderState);
		}
		else
			MyMessageBox(_T("DepthStencilState Already Exists! : ["), wsName, _T("]"));
		break;
	case EStateType::RASTERIZER:
		if (m_mpRs[wsName] == nullptr) {
			m_mpRs[wsName] = pRenderState;
			m_vpRs.push_back(pRenderState);
		}
		else
			MyMessageBox(_T("RasterizerState Already Exists! : ["), wsName, _T("]"));
		break;
		break;
	case EStateType::BLEND:
		break;
	case EStateType::SAMPLER:
		break;
	}
	pRenderState->GetName();
}

void CRenderStateManager::Initialize(ID3D11Device * pDevice) {
	gsDpthStnclState[EDpthStnclState::NONE] = _T("NONE");
	gsDpthStnclState[EDpthStnclState::DEPTH_TEST_NO_WRITE] = _T("DEPTH_TEST_NO_WRITE");
	gsDpthStnclState[EDpthStnclState::NO_DEPTH_TEST] = _T("NO_DEPTH_TEST");
	gsDpthStnclState[EDpthStnclState::GRT_EQUAL] = _T("GRT_EQUAL");

	gsRstrzrState[ERstrzrState::NONE] = _T("NONE");
	gsRstrzrState[ERstrzrState::SOLID_NONE_CULL] = _T("SOLID_NONE_CULL");
	gsRstrzrState[ERstrzrState::SOLID_BACK_CULL] = _T("SOLID_BACK_CULL");
	gsRstrzrState[ERstrzrState::SOLID_FRONT_CULL] = _T("SOLID_FRONT_CULL");
	gsRstrzrState[ERstrzrState::WIRE_NONE_CULL] = _T("WIRE_NONE_CULL");
	gsRstrzrState[ERstrzrState::WIRE_BACK_CULL] = _T("WIRE_BACK_CULL");
	gsRstrzrState[ERstrzrState::WIRE_FRONT_CULL] = _T("WIRE_FRONT_CULL");

	gsBlndState[EBlndState::NONE] = _T("NONE");
	gsBlndState[EBlndState::ONE_BLEND] = _T("ONE_BLEND");
	gsBlndState[EBlndState::SRC_BLEND] = _T("SRC_BLEND");
	gsBlndState[EBlndState::SRC_BLEND2] = _T("SRC_BLEND2");
	gsBlndState[EBlndState::ENABLE_BLEND_ALPHA] = _T("ENABLE_BLEND_ALPHA");
	gsBlndState[EBlndState::ALPHA_COVERAGE] = _T("ALPHA_COVERAGE");
	gsBlndState[EBlndState::ALPHA_COVARAGE_ALPHA_BLENDING] = _T("ALPHA_COVARAGE_ALPHA_BLENDING");

	gsSmplrState[ESmplrState::NONE] = _T("NONE");
	gsSmplrState[ESmplrState::CLAMP] = _T("CLAMP");
	gsSmplrState[ESmplrState::WARP] = _T("WARP");
	gsSmplrState[ESmplrState::MIPMAP] = _T("MIPMAP");

	//깊이 스텐실 상태
	ID3D11DepthStencilState* pDepthStencilState = nullptr;
	D3D11_DEPTH_STENCIL_DESC DepthStencilDesc;

	m_mpDepthStenciltate[EDpthStnclState::NONE] = nullptr;
	CreateAddState(pDevice, static_cast<D3D11_DEPTH_STENCIL_DESC*>(nullptr), _T("NONE"));

	ZeroMemory(&DepthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	DepthStencilDesc.DepthEnable = false;
	DepthStencilDesc.StencilEnable = false;
	pDevice->CreateDepthStencilState(&DepthStencilDesc, &pDepthStencilState);
	m_mpDepthStenciltate[EDpthStnclState::NO_DEPTH_TEST] = pDepthStencilState;
	CreateAddState(pDevice, &DepthStencilDesc, _T("NO_DEPTH_TEST"));

	ZeroMemory(&DepthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	DepthStencilDesc.DepthEnable = true;
	DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	DepthStencilDesc.StencilEnable = false;
	pDevice->CreateDepthStencilState(&DepthStencilDesc, &pDepthStencilState);
	m_mpDepthStenciltate[EDpthStnclState::DEPTH_TEST_NO_WRITE] = pDepthStencilState;
	CreateAddState(pDevice, &DepthStencilDesc, _T("DEPTH_TEST_NO_WRITE"));

	ZeroMemory(&DepthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	DepthStencilDesc.DepthEnable = true;
	DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	DepthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
	DepthStencilDesc.StencilEnable = false;
	pDevice->CreateDepthStencilState(&DepthStencilDesc, &pDepthStencilState);
	m_mpDepthStenciltate[EDpthStnclState::GRT_EQUAL] = pDepthStencilState;
	CreateAddState(pDevice, &DepthStencilDesc, _T("GRT_EQUAL"));

	//래스터라이져 상태
	ID3D11RasterizerState* pRasterizerState = nullptr;
	D3D11_RASTERIZER_DESC RasterizerDesc;

	m_mpRasterizerState[ERstrzrState::NONE] = nullptr;
	CreateAddState(pDevice, static_cast<D3D11_RASTERIZER_DESC*>(nullptr), _T("NONE"));

	ZeroMemory(&RasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));	//솔리드 프레임, 은면제거 하지 않음, SOLID_NONE
	RasterizerDesc.FillMode = D3D11_FILL_SOLID;
	RasterizerDesc.CullMode = D3D11_CULL_NONE;
	RasterizerDesc.FrontCounterClockwise = false;
	RasterizerDesc.DepthClipEnable = true;
	pDevice->CreateRasterizerState(&RasterizerDesc, &pRasterizerState);
	m_mpRasterizerState[ERstrzrState::SOLID_NONE_CULL] = pRasterizerState;
	CreateAddState(pDevice, &RasterizerDesc, _T("SOLID_NONE_CULL"));

	ZeroMemory(&RasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));	//솔리드 프레임, 은면제거 함, SOLID_BACK
	RasterizerDesc.FillMode = D3D11_FILL_SOLID;
	RasterizerDesc.CullMode = D3D11_CULL_BACK;
	RasterizerDesc.FrontCounterClockwise = false;
	RasterizerDesc.DepthClipEnable = true;
	pDevice->CreateRasterizerState(&RasterizerDesc, &pRasterizerState);
	m_mpRasterizerState[ERstrzrState::SOLID_BACK_CULL] = pRasterizerState;
	CreateAddState(pDevice, &RasterizerDesc, _T("SOLID_BACK_CULL"));

	ZeroMemory(&RasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));	//솔리드 프레임, FRONT 면 제거 하지 않음, SOLID_FRONT
	RasterizerDesc.FillMode = D3D11_FILL_SOLID;
	RasterizerDesc.CullMode = D3D11_CULL_FRONT;
	RasterizerDesc.FrontCounterClockwise = false;
	RasterizerDesc.DepthClipEnable = true;
	pDevice->CreateRasterizerState(&RasterizerDesc, &pRasterizerState);
	m_mpRasterizerState[ERstrzrState::SOLID_FRONT_CULL] = pRasterizerState;
	CreateAddState(pDevice, &RasterizerDesc, _T("SOLID_FRONT_CULL"));

	ZeroMemory(&RasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));	//와이어 프레임, 은면제거 하지 않음, WIRE_NONE
	RasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	RasterizerDesc.CullMode = D3D11_CULL_NONE;
	RasterizerDesc.FrontCounterClockwise = false;
	RasterizerDesc.DepthClipEnable = true;
	pDevice->CreateRasterizerState(&RasterizerDesc, &pRasterizerState);
	m_mpRasterizerState[ERstrzrState::WIRE_NONE_CULL] = pRasterizerState;
	CreateAddState(pDevice, &RasterizerDesc, _T("WIRE_NONE_CULL"));

	ZeroMemory(&RasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));	//와이어 프레임, 은면제거 함, WIRE_BACK
	RasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	RasterizerDesc.CullMode = D3D11_CULL_BACK;
	RasterizerDesc.FrontCounterClockwise = false;
	RasterizerDesc.DepthClipEnable = true;
	pDevice->CreateRasterizerState(&RasterizerDesc, &pRasterizerState);
	m_mpRasterizerState[ERstrzrState::WIRE_BACK_CULL] = pRasterizerState;
	CreateAddState(pDevice, &RasterizerDesc, _T("WIRE_BACK_CULL"));

	ZeroMemory(&RasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));	//와이어 프레임, BACK 면 제거 하지 않음, SOLID_BACK
	RasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	RasterizerDesc.CullMode = D3D11_CULL_FRONT;
	RasterizerDesc.FrontCounterClockwise = false;
	RasterizerDesc.DepthClipEnable = true;
	pDevice->CreateRasterizerState(&RasterizerDesc, &pRasterizerState);
	m_mpRasterizerState[ERstrzrState::WIRE_FRONT_CULL] = pRasterizerState;
	CreateAddState(pDevice, &RasterizerDesc, _T("WIRE_FRONT_CULL"));

	//블렌드 상태
	ID3D11BlendState* pBlendState = nullptr;
	D3D11_BLEND_DESC BlendStateDesc;

	m_mpBlendState[EBlndState::NONE] = nullptr;

	ZeroMemory(&BlendStateDesc, sizeof(D3D11_BLEND_DESC));		//ONE_BLEND
	BlendStateDesc.AlphaToCoverageEnable = false;
	BlendStateDesc.IndependentBlendEnable = false;
	BlendStateDesc.RenderTarget[0].BlendEnable = true;
	BlendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	BlendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	BlendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	BlendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	pDevice->CreateBlendState(&BlendStateDesc, &pBlendState);
	m_mpBlendState[EBlndState::ONE_BLEND] = pBlendState;

	ZeroMemory(&BlendStateDesc, sizeof(D3D11_BLEND_DESC));		//SRC_BLEND
	BlendStateDesc.AlphaToCoverageEnable = false;
	BlendStateDesc.IndependentBlendEnable = false;
	BlendStateDesc.RenderTarget[0].BlendEnable = true;
	BlendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_COLOR;
	BlendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR;
	BlendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	BlendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	pDevice->CreateBlendState(&BlendStateDesc, &pBlendState);
	m_mpBlendState[EBlndState::SRC_BLEND] = pBlendState;

	ZeroMemory(&BlendStateDesc, sizeof(D3D11_BLEND_DESC));		//SRC_BLEND2
	BlendStateDesc.AlphaToCoverageEnable = false;
	BlendStateDesc.IndependentBlendEnable = false;
	BlendStateDesc.RenderTarget[0].BlendEnable = true;
	BlendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	pDevice->CreateBlendState(&BlendStateDesc, &pBlendState);
	m_mpBlendState[EBlndState::SRC_BLEND2] = pBlendState;

	ZeroMemory(&BlendStateDesc, sizeof(D3D11_BLEND_DESC));		//ENABLE_BLEND_ALPHA
	BlendStateDesc.AlphaToCoverageEnable = false;
	BlendStateDesc.IndependentBlendEnable = false;
	BlendStateDesc.RenderTarget[0].BlendEnable = true;
	BlendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	pDevice->CreateBlendState(&BlendStateDesc, &pBlendState);
	m_mpBlendState[EBlndState::ENABLE_BLEND_ALPHA] = pBlendState;

	ZeroMemory(&BlendStateDesc, sizeof(D3D11_BLEND_DESC));		//ALPHA_COVERAGE
	BlendStateDesc.AlphaToCoverageEnable = true;
	BlendStateDesc.IndependentBlendEnable = false;
	BlendStateDesc.RenderTarget[0].BlendEnable = false;
	BlendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	pDevice->CreateBlendState(&BlendStateDesc, &pBlendState);
	m_mpBlendState[EBlndState::ALPHA_COVERAGE] = pBlendState;

	ZeroMemory(&BlendStateDesc, sizeof(D3D11_BLEND_DESC));		//ALPHA_COVARAGE_ALPHA_BLENDING
	BlendStateDesc.AlphaToCoverageEnable = true;
	BlendStateDesc.IndependentBlendEnable = false;
	BlendStateDesc.RenderTarget[0].BlendEnable = true;
	BlendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	BlendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	pDevice->CreateBlendState(&BlendStateDesc, &pBlendState);
	m_mpBlendState[EBlndState::ALPHA_COVARAGE_ALPHA_BLENDING] = pBlendState;

	//샘플러 상태
	ID3D11SamplerState *pSamplerState = nullptr;
	D3D11_SAMPLER_DESC SamplerDesc;

	m_mpSamplerState[ESmplrState::NONE] = nullptr;

	ZeroMemory(&SamplerDesc, sizeof(D3D11_SAMPLER_DESC));	//CLAMP
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	SamplerDesc.MinLOD = 0;
	SamplerDesc.MaxLOD = 9;
	pDevice->CreateSamplerState(&SamplerDesc, &pSamplerState);
	m_mpSamplerState[ESmplrState::CLAMP] = pSamplerState;

	ZeroMemory(&SamplerDesc, sizeof(D3D11_SAMPLER_DESC));	//WARP
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;		//D3D11_FILTER_ANISOTROPIC
	SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	SamplerDesc.MipLODBias = 0;
	SamplerDesc.MinLOD = 0;
	SamplerDesc.MaxLOD = 9;
	pDevice->CreateSamplerState(&SamplerDesc, &pSamplerState);
	m_mpSamplerState[ESmplrState::WARP] = pSamplerState;

	ZeroMemory(&SamplerDesc, sizeof(D3D11_SAMPLER_DESC));	//	MIPMAP
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;		//D3D11_FILTER_ANISOTROPIC
	SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	SamplerDesc.MipLODBias = 0;
	SamplerDesc.MinLOD = -FLT_MAX;
	SamplerDesc.MaxLOD = FLT_MAX;
	pDevice->CreateSamplerState(&SamplerDesc, &pSamplerState);
	m_mpSamplerState[ESmplrState::MIPMAP] = pSamplerState;
}
