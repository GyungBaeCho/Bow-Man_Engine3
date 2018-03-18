#pragma once
#include "UtilityFunction.h"

enum class EDpthStnclState : char {
	NONE,
	NO_DEPTH_TEST,
	DEPTH_TEST_NO_WRITE,
	GRT_EQUAL,
};

enum class ERstrzrState : char {
	NONE,
	SOLID_NONE_CULL,
	SOLID_BACK_CULL,
	SOLID_FRONT_CULL,
	WIRE_NONE_CULL,
	WIRE_BACK_CULL,
	WIRE_FRONT_CULL,
};

enum class EBlndState : char {
	NONE,
	ONE_BLEND,
	SRC_BLEND,
	SRC_BLEND2,
	ENABLE_BLEND_ALPHA,
	ALPHA_COVERAGE,
	ALPHA_COVARAGE_ALPHA_BLENDING,
};

enum class ESmplrState : char {
	NONE,
	CLAMP,
	WARP,
	MIPMAP,
};

enum class EStateType : char {
	NONE,
	DEPTHSTENCIL,
	RASTERIZER,
	BLEND,
	SAMPLER,
};

class CRenderState{
private:
	std::wstring m_wsName;
	EStateType m_eStateType = EStateType::NONE;
	UINT m_ID;

public:
	CRenderState() {}
	virtual ~CRenderState() {}

	virtual void Release() = 0;

	void SetName(const std::wstring& wsName) {
		m_wsName = wsName;
	}
	void SetID(UINT ID) {
		m_ID = ID;
	}
	void SetStateType(const EStateType& eStateType) {
		m_eStateType = eStateType;
	}
	virtual void SetStateToDevice(ID3D11DeviceContext *pDeviceContext) = 0;

	const std::wstring& GetName() {
		return m_wsName;
	}
	const EStateType& GetStateType(){
		return m_eStateType;
	}
	UINT GetID() {
		return m_ID;
	}
};

class CDepthStencilState : public CRenderState {
private:
	ID3D11DepthStencilState *m_pState = nullptr;
	UINT m_uStencilRef = 1;
	static UINT m_pvtID;

public:
	CDepthStencilState (ID3D11Device* pDevice, const D3D11_DEPTH_STENCIL_DESC *pStateDesc, const std::wstring& wsName) {
		Create(pDevice, pStateDesc);
		SetName(wsName);
		SetID(m_pvtID++);
		SetStateType(EStateType::DEPTHSTENCIL);
	}
	virtual ~CDepthStencilState () {}

	template<class T>
	void Create(ID3D11Device* pDevice, T* pStateDesc) {
		if (pStateDesc)
			pDevice->CreateDepthStencilState(pStateDesc, &m_pState);
		else
			m_pState = nullptr;
	}

	virtual void Release() {
		SAFE_RELEASE(m_pState);
	}

	virtual void SetStateToDevice(ID3D11DeviceContext *pDeviceContext) {
		pDeviceContext->OMSetDepthStencilState(m_pState, m_uStencilRef);
	}
};
_declspec(selectany) UINT CDepthStencilState::m_pvtID = 0;

class CRaserizerState : public CRenderState {
private:
	ID3D11RasterizerState *m_pState = nullptr;
	UINT m_uStencilRef = 1;
	static UINT m_pvtID;

public:
	CRaserizerState (ID3D11Device* pDevice, const D3D11_RASTERIZER_DESC *pStateDesc, const std::wstring& wsName) {
		Create(pDevice, pStateDesc);
		SetName(wsName);
		SetID(m_pvtID++);
		SetStateType(EStateType::RASTERIZER);
	}
	virtual ~CRaserizerState () {}

	template<class T>
	void Create(ID3D11Device* pDevice, T* pStateDesc) {
		if (pStateDesc)
			pDevice->CreateRasterizerState(pStateDesc, &m_pState);
		else
			m_pState = nullptr;
	}

	virtual void Release() {
		SAFE_RELEASE(m_pState);
	}

	virtual void SetStateToDevice(ID3D11DeviceContext *pDeviceContext) {
		pDeviceContext->RSSetState(m_pState);
	}
};
_declspec(selectany) UINT CRaserizerState::m_pvtID = 0;


_declspec(selectany) std::map<EDpthStnclState, std::wstring>	gsDpthStnclState;
_declspec(selectany) std::map<ERstrzrState, std::wstring>		gsRstrzrState;
_declspec(selectany) std::map<EBlndState, std::wstring>			gsBlndState;
_declspec(selectany) std::map<ESmplrState, std::wstring>		gsSmplrState;

#define DEPTH_STENCIL_STATE			CRenderStateManager::m_mpDepthStenciltate
#define RASTERIZER_STATE			CRenderStateManager::m_mpRasterizerState
#define BLEND_STATE					CRenderStateManager::m_mpBlendState
#define SAMPLER_STATE				CRenderStateManager::m_mpSamplerState

#define RenderState					CRenderStateManager::GetInstance()

class CRenderStateManager
{
private:
	static CRenderStateManager* m_pInstance;

public:
	static std::map<EDpthStnclState, ID3D11DepthStencilState*>	m_mpDepthStenciltate;
	static std::map<ERstrzrState, ID3D11RasterizerState*>		m_mpRasterizerState;
	static std::map<EBlndState, ID3D11BlendState*>				m_mpBlendState;
	static std::map<ESmplrState, ID3D11SamplerState*>			m_mpSamplerState;

	std::map<std::wstring, CRenderState*>	m_mpDs;
	std::vector<CRenderState*>				m_vpDs;

	std::map<std::wstring, CRenderState*>	m_mpRs;
	std::vector<CRenderState*>				m_vpRs;

	std::map<std::wstring, CRenderState*> m_mpBs;
	std::map<std::wstring, CRenderState*> m_mpSs;

private:
	CRenderStateManager(){}
	~CRenderStateManager(){}

public:
	static void CreateInstance() {		m_pInstance = new CRenderStateManager;		}
	static CRenderStateManager* GetInstance() {		return m_pInstance;			}
	void Initialize(ID3D11Device *pDevice);

	void Release(){
		for(std::pair<const EDpthStnclState,ID3D11DepthStencilState*>& pState : m_mpDepthStenciltate){
			if(pState.second)
				pState.second->Release();
		}
		for(std::pair<const ERstrzrState,ID3D11RasterizerState*>& pState : m_mpRasterizerState){
			if(pState.second)
				pState.second->Release();
		}
		for(std::pair<const EBlndState,ID3D11BlendState*>& pState : m_mpBlendState){
			if(pState.second)
				pState.second->Release();
		}
		for(std::pair<const ESmplrState,ID3D11SamplerState*>& pState : m_mpSamplerState){
			if(pState.second)
				pState.second->Release();
		}
	}
	void CreateAddState(ID3D11Device *pDevice, const D3D11_DEPTH_STENCIL_DESC* pState, const std::wstring& wsName);
	void CreateAddState(ID3D11Device *pDevice, const D3D11_RASTERIZER_DESC* pState, const std::wstring& wsName);
	void AddState(CRenderState* pRenderState);
};

__declspec(selectany) CRenderStateManager*									CRenderStateManager::m_pInstance = nullptr;
__declspec(selectany) std::map<EDpthStnclState, ID3D11DepthStencilState*>	CRenderStateManager::m_mpDepthStenciltate;
__declspec(selectany) std::map<ERstrzrState, ID3D11RasterizerState*>		CRenderStateManager::m_mpRasterizerState;
__declspec(selectany) std::map<EBlndState, ID3D11BlendState*>			CRenderStateManager::m_mpBlendState;
__declspec(selectany) std::map<ESmplrState, ID3D11SamplerState*>		CRenderStateManager::m_mpSamplerState;