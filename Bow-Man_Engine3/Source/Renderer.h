#pragma once
#include "ParallelPivot.h"

/*
	--참고사항--
	
	효율적인 Render Batch를 위해 실시간으로 특정 Render State의 조합용 Renderer를 생성하여 사용한다.
	[Render State로는 Shader, Blend state, Rasterizer state, depth_stencil state 등의 상태가 있다]

	본 프레임워크의 Render 단위는 CModel 라고 불리는 객체로써, Render Queue는 CModel의 내부 인자를 Packing한 Data의 연속이다.
	각각의 CModel은 Render State를 가지며, 이 Render State를 정수로 치환하여 고유 ID 값을 갖는다.

	동일 ID값을 갖는 CModel들을 모아 한번에 Render하는 구조로 개발하였다.

	CModelHomo는 CObject가 소유하는 CModelAssembly 내부에 동일한 Renderer ID를 가진 CModel들을 모아둔 Data이다.

	Render Batch를 위하여, Render 단위를 CObject에서 CModel로 변경하였으므로,
	World 행렬을 GPU로 전달해야 하는 횟수가 비약적으로 늘었다.

	따라서 동일 CObject에 포함되며, Renderer ID가 같은 CModel들을 묶어서, World행렬을 GPU에 묶는 횟수를 줄였다.


	RenderData 는 CModelHomo와, CModelHomo를 호출한 CObject의 World 행렬을 묶은 pair Data이다.
	m_vRenderData에는 최종적으로 1 Frame을 Rendering하기 위한 Data가 저장되고, 매 Frame마다 Pivot이 0으로 초기화된다.

	m_vRenderData는 vector 자료형으로서 World 규모에 맞춰서 RenderData를 저장할 수 있고,
	Clear 명령어로 데이터를 초기화 하는 대신, m_pvtRenderData 피벗을 사용하여 Data 갯수를 저장하고,
	0으로 초기화 하여 성능 면에서 최적화 하였다(불필요한, 계속해서 Resize 회피).
*/

//Dummy 
class CUiObject;
class CModelHomo;
class CShaderPack;
enum class EPixelShaderType : char;

//Renderer가 Unengaged 될때 까지의 유예시간
const float g_MaxWaitTime = 1;

typedef std::pair<CModelHomo*, XMFLOAT4X4> RenderData;
class CRenderer {
private:
	UINT						m_tRenderer;

	CShaderPack					*m_pShaderPack = nullptr;
	ID3D11RasterizerState		*m_pRstrzrState = nullptr;
	ID3D11DepthStencilState		*m_pDpthStnclState = nullptr;
	ID3D11BlendState			*m_pBlndState = nullptr;

	UINT						m_pvtRenderData[2];
	std::vector<RenderData>		m_vRenderData[2];

	float						m_fQueueTimer = 0;
	bool						m_bEngaged = false;

	static void (*FuncAddRenderData)(CRenderer* pThis, CModelHomo* pModelHomo, const XMFLOAT4X4& mtxWorld);
	static void (*FuncClearRenderData)(CRenderer* pThis);
	static void (*FuncUpdate)(CRenderer* pThis, float fElapsedTime);
	static void (*FuncRender)(CRenderer* pThis, ID3D11DeviceContext* pDeviceContext);

private:
	static void NormalAddRenderData(CRenderer *pThis, CModelHomo* pModelHomo, const XMFLOAT4X4& mtxWorld);
	static void NormalClearRenderData(CRenderer *pThis);
	static void NormalUpdate(CRenderer *pThis, float fElapsedTime);
	static void NormalRender(CRenderer *pThis, ID3D11DeviceContext *pDeviceContext);
	static void ParallelAddRenderData(CRenderer *pThis, CModelHomo* pModelHomo, const XMFLOAT4X4& mtxWorld);
	static void ParallelClearRenderData(CRenderer *pThis);
	static void ParallelUpdate(CRenderer *pThis, float fElapsedTime);
	static void ParallelRender(CRenderer *pThis, ID3D11DeviceContext *pDeviceContext);

public:
	CRenderer() {}
	~CRenderer() {}

	void Initialize(UINT tRenderer);

	void CreateUI(const POINT& vMousePos);

	void AddRenderData(CModelHomo* pModelHomo, const XMFLOAT4X4& mtxWorld) {
		FuncAddRenderData(this, pModelHomo, mtxWorld);
	}
	void ClearRenderData() {
		FuncClearRenderData(this);
	}
	void ClearAll() {
		m_pvtRenderData[0] = 0;
		m_pvtRenderData[1] = 0;
	}

	void Update(float fElapsedTime) {
		FuncUpdate(this, fElapsedTime);
	}
	void Render(ID3D11DeviceContext *pDeviceContext) {
		FuncRender(this, pDeviceContext);
	}

	static void SwitchToParalell(bool bFlag);
	void SetEngaged(bool bFlag);

	UINT* GetpvtRendererData() {
		return &m_pvtRenderData[0];
	}
	UINT GetRendererType() const {
		return m_tRenderer;
	}
	EPixelShaderType GetPixelShaderType();

	ID3D11BlendState* GetBlendState() {
		return m_pBlndState;
	}
	bool GetEngaged() {
		return m_bEngaged;
	}
};
_declspec(selectany) void (*CRenderer::FuncAddRenderData)(CRenderer* pThis, CModelHomo* pModelHomo, const XMFLOAT4X4& mtxWorld) = NormalAddRenderData;
_declspec(selectany) void (*CRenderer::FuncClearRenderData)(CRenderer* pThis)													= NormalClearRenderData;
_declspec(selectany) void (*CRenderer::FuncUpdate)(CRenderer* pThis, float fElapsedTime)										= NormalUpdate;
_declspec(selectany) void (*CRenderer::FuncRender)(CRenderer* pThis, ID3D11DeviceContext* pDeviceContext)						= NormalRender;


#define RendererManager		CRendererManager::GetInstance()
class CRendererManager {
private:
	static CRendererManager* m_pInstance;

/*
	--참고사항--
	
	Renderer Map은 map 자료구조 이기에 매 Frame마다 검색되어지는 데이터가 아닌,
	Render ID를 Key값으로, CModel에 적합한 Renderer를 검색해주기 위한 용도로 사용됨.

	따라서, World가 초기화될때 or 수정작업이 이루어질 때 검색이 이루어지며, Play중에 검색이 되는 경우는 없다.

	Render할 Data가 존재하는 Renderer는 각각 특성에 따라(Fowrad Shader, Deferred Shader, Blend Shader) Renderer List에 삽입된다.

	각 Renderer List 는 Update를 통하여 특정 기간동안(g_MaxWaitTime) Render Data가 없던 Renderer는 실시간으로 제거되도록 구현하여,
	Dummy Renderer가 자리를 차지하지 않도록 하였다.
*/
	
	std::unordered_map<UINT, CRenderer*> m_mpRenderer;
	std::list<CRenderer*> m_lpShadowRenderer;
	std::list<CRenderer*> m_vpDeferredRenderer;
	std::list<CRenderer*> m_vpFowardRenderer;
	std::list<CRenderer*> m_vpBlendRenderer;
	CRenderer *m_pSkyboxRenderer = nullptr;

	CUiObject *m_pRendererManagerUi = nullptr;

private:
	CRendererManager() {}
	~CRendererManager() {}

public:
	static void CreateInstance() {
		if (m_pInstance) return;
		m_pInstance = new CRendererManager;
	}
	static CRendererManager* GetInstance() {
		return m_pInstance;
	}

	void Initialize();
	void CreateUI(const POINT & vMousePos);

	//Update & Remove Renderer From Renderer List if Unused for Reasonable Time
	void Update(float fElapsedTime) {
		if (m_pSkyboxRenderer) {
			m_pSkyboxRenderer->Update(fElapsedTime);
			if (!m_pSkyboxRenderer->GetEngaged())
				m_pSkyboxRenderer = nullptr;
		}

		m_vpDeferredRenderer.remove_if([fElapsedTime](CRenderer* pRenderer) {
			pRenderer->Update(fElapsedTime);
			return !(pRenderer->GetEngaged());
		});

		m_vpFowardRenderer.remove_if([fElapsedTime](CRenderer* pRenderer) {
			pRenderer->Update(fElapsedTime);
			return !(pRenderer->GetEngaged());
		});

		m_vpBlendRenderer.remove_if([fElapsedTime](CRenderer* pRenderer) {
			pRenderer->Update(fElapsedTime);
			return !(pRenderer->GetEngaged());
		});

		//Clear Render Pivot Before (Adding New Render Data List & Rendering)
		//If Cleared Right After the Rendering, it might be impossible to Render Render Infos..
		ClearRenderData();
	}
	
	void Render(ID3D11DeviceContext *pDeviceContext) {
		for (CRenderer* pRenderer : m_vpDeferredRenderer)	pRenderer->Render(pDeviceContext);
		for (CRenderer* pRenderer : m_vpFowardRenderer)		pRenderer->Render(pDeviceContext);
		for (CRenderer* pRenderer : m_vpBlendRenderer)		pRenderer->Render(pDeviceContext);
	};
	
	void RenderSkybox(ID3D11DeviceContext *pDeviceContext) {
		(m_pSkyboxRenderer) ? m_pSkyboxRenderer->Render(pDeviceContext) : NULL;
	}

	void RenderDeferred(ID3D11DeviceContext *pDeviceContext) {
		for (CRenderer* pRenderer : m_vpDeferredRenderer)	pRenderer->Render(pDeviceContext);
	};

	void RenderFoward(ID3D11DeviceContext *pDeviceContext) {
		for (CRenderer* pRenderer : m_vpFowardRenderer)		pRenderer->Render(pDeviceContext);
	};

	void RenderBlend(ID3D11DeviceContext *pDeviceContext) {
		for (CRenderer* pRenderer : m_vpBlendRenderer)		pRenderer->Render(pDeviceContext);
	};

	void ClearRenderData() {
		(m_pSkyboxRenderer) ? m_pSkyboxRenderer->ClearRenderData() : NULL;
		for (CRenderer* pRenderer : m_vpDeferredRenderer)	pRenderer->ClearRenderData();
		for (CRenderer* pRenderer : m_vpFowardRenderer)		pRenderer->ClearRenderData();
		for (CRenderer* pRenderer : m_vpBlendRenderer)		pRenderer->ClearRenderData();
	}

	CRenderer* CreateRenderer(UINT tRenderer);

	void AddRenderData(CRenderer* pRenderer, CModelHomo* pModelHomo, const XMFLOAT4X4& mtxWorld) {
		AddToRendererList(pRenderer);
		pRenderer->AddRenderData(pModelHomo, mtxWorld);
	}
	void AddToRendererList(CRenderer* pRenderer);

	CRenderer* GetRenderer(UINT tRenderer);
};

_declspec(selectany) CRendererManager* CRendererManager::m_pInstance = nullptr;

