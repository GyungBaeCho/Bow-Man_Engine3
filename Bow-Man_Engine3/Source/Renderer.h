#pragma once
#include "ParallelPivot.h"

/*
	--�������--
	
	ȿ������ Render Batch�� ���� �ǽð����� Ư�� Render State�� ���տ� Renderer�� �����Ͽ� ����Ѵ�.
	[Render State�δ� Shader, Blend state, Rasterizer state, depth_stencil state ���� ���°� �ִ�]

	�� �����ӿ�ũ�� Render ������ CModel ��� �Ҹ��� ��ü�ν�, Render Queue�� CModel�� ���� ���ڸ� Packing�� Data�� �����̴�.
	������ CModel�� Render State�� ������, �� Render State�� ������ ġȯ�Ͽ� ���� ID ���� ���´�.

	���� ID���� ���� CModel���� ��� �ѹ��� Render�ϴ� ������ �����Ͽ���.

	CModelHomo�� CObject�� �����ϴ� CModelAssembly ���ο� ������ Renderer ID�� ���� CModel���� ��Ƶ� Data�̴�.

	Render Batch�� ���Ͽ�, Render ������ CObject���� CModel�� �����Ͽ����Ƿ�,
	World ����� GPU�� �����ؾ� �ϴ� Ƚ���� ��������� �þ���.

	���� ���� CObject�� ���ԵǸ�, Renderer ID�� ���� CModel���� ���, World����� GPU�� ���� Ƚ���� �ٿ���.


	RenderData �� CModelHomo��, CModelHomo�� ȣ���� CObject�� World ����� ���� pair Data�̴�.
	m_vRenderData���� ���������� 1 Frame�� Rendering�ϱ� ���� Data�� ����ǰ�, �� Frame���� Pivot�� 0���� �ʱ�ȭ�ȴ�.

	m_vRenderData�� vector �ڷ������μ� World �Ը� ���缭 RenderData�� ������ �� �ְ�,
	Clear ��ɾ�� �����͸� �ʱ�ȭ �ϴ� ���, m_pvtRenderData �ǹ��� ����Ͽ� Data ������ �����ϰ�,
	0���� �ʱ�ȭ �Ͽ� ���� �鿡�� ����ȭ �Ͽ���(���ʿ���, ����ؼ� Resize ȸ��).
*/

//Dummy 
class CUiObject;
class CModelHomo;
class CShaderPack;
enum class EPixelShaderType : char;

//Renderer�� Unengaged �ɶ� ������ �����ð�
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
	--�������--
	
	Renderer Map�� map �ڷᱸ�� �̱⿡ �� Frame���� �˻��Ǿ����� �����Ͱ� �ƴ�,
	Render ID�� Key������, CModel�� ������ Renderer�� �˻����ֱ� ���� �뵵�� ����.

	����, World�� �ʱ�ȭ�ɶ� or �����۾��� �̷���� �� �˻��� �̷������, Play�߿� �˻��� �Ǵ� ���� ����.

	Render�� Data�� �����ϴ� Renderer�� ���� Ư���� ����(Fowrad Shader, Deferred Shader, Blend Shader) Renderer List�� ���Եȴ�.

	�� Renderer List �� Update�� ���Ͽ� Ư�� �Ⱓ����(g_MaxWaitTime) Render Data�� ���� Renderer�� �ǽð����� ���ŵǵ��� �����Ͽ�,
	Dummy Renderer�� �ڸ��� �������� �ʵ��� �Ͽ���.
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

