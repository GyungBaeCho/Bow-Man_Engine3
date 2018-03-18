#pragma once
#include "ParallelPivot.h"
#include "ConstantBuffer.h"

//Dummy class 
class CScene;
class CStopWatch;
class CShaderPack;

_declspec(selectany) ID3D11Device* gpDevice = nullptr;
_declspec(selectany) ID3D11DeviceContext* gpImmediateContext = nullptr;
_declspec(selectany) ID3D11DeviceContext* gpDeferredContext = nullptr;
__declspec(selectany) HIMC	ghIMC = nullptr;
_declspec(selectany) volatile float gfElapsedTime = 0;

_declspec(selectany) int g_nBurdon = 0;

namespace Time{
	enum EStopWatchIndex : char{
		Update,
		Render,
		UI_Render,
		Elapsed_Time,
	};
};

#define SCREEN_WIDTH	CFramework::m_nScreenWidth
#define SCREEN_HEIGHT	CFramework::m_nScreenHeight

class CScreenBuffer {
private:
	ID3D11Texture2D				*m_pTexBuffer = nullptr;
	ID3D11RenderTargetView		*m_pRtvBuffer = nullptr;
	ID3D11ShaderResourceView	*m_pSrvBuffer = nullptr;
	ID2D1Bitmap					*m_pBitBuffer = nullptr;

public:
	CScreenBuffer() {}
	~CScreenBuffer() {}
};

typedef class CFramework
{
private:
	HWND						m_hWnd					= NULL;
	//Direct 3D
	ID3D11Device				*m_pDevice				= nullptr;
	ID3D11DeviceContext			*m_pImmediateContext	= nullptr;
	ID3D11DeviceContext			*m_pDeferredContext		= nullptr;

	//Frame Buffers
	IDXGISwapChain				*m_pDXGISwapChain		= nullptr;
	ID3D11Texture2D				*m_pTexBackBuffer		= nullptr;
	ID3D11RenderTargetView		*m_pRtvBackBuffer		= nullptr;
	ID3D11ShaderResourceView	*m_pSrvBackBuffer		= nullptr;
	ID3D11UnorderedAccessView	*m_pUavBackBuffer		= nullptr;

	//HDR
	ID3D11Texture2D				*m_pTexHDR				= nullptr;
	ID3D11RenderTargetView		*m_pRtvHDR				= nullptr;
	ID3D11ShaderResourceView	*m_pSrvHDR				= nullptr;

	ID3D11Buffer				*m_pBuffIntermediate	= nullptr;
	ID3D11UnorderedAccessView	*m_pUavIntermediate		= nullptr;
	ID3D11ShaderResourceView	*m_pSrvIntermediate		= nullptr;

	ID3D11Buffer				*m_pBuffAverage			= nullptr;
	ID3D11UnorderedAccessView	*m_pUavAverage			= nullptr;
	ID3D11ShaderResourceView	*m_pSrvAverage			= nullptr;

	ID3D11Buffer				*m_pBuffPrevAverage		= nullptr;
	ID3D11UnorderedAccessView	*m_pUavPrevAverage		= nullptr;
	ID3D11ShaderResourceView	*m_pSrvPrevAverage		= nullptr;

	ID3D11Buffer				*m_pcbDownFactor		= nullptr;
	ID3D11Buffer				*m_pcbToneFactor		= nullptr;

	UINT						m_nDownScaleGroups		= 0;

	//HDR Quarter - Bloom
	ID3D11Texture2D				*m_pTexHdrQuarter		= nullptr;
	ID3D11ShaderResourceView	*m_pSrvHdrQuarter		= nullptr;
	ID3D11UnorderedAccessView	*m_pUavHdrQuarter		= nullptr;

	ID3D11ShaderResourceView	*m_pSrvBloom1			= nullptr;
	ID3D11UnorderedAccessView	*m_pUavBloom1			= nullptr;
	
	ID3D11ShaderResourceView	*m_pSrvBloom2			= nullptr;
	ID3D11UnorderedAccessView	*m_pUavBloom2			= nullptr;
	
	ID3D11ShaderResourceView	*m_pSrvBloomFinal		= nullptr;
	ID3D11UnorderedAccessView	*m_pUavBloomFinal		= nullptr;

	//GBuffer
	ID3D11Texture2D				*m_pTexAlbedo			= nullptr;
	ID3D11RenderTargetView		*m_pRtvAlbedo			= nullptr;
	ID3D11ShaderResourceView	*m_pSrvAlbedo			= nullptr;
	ID2D1Bitmap					*m_pBitAlbedo			= nullptr;

	ID3D11Texture2D				*m_pTexNormal			= nullptr;
	ID3D11RenderTargetView		*m_pRtvNormal			= nullptr;
	ID3D11ShaderResourceView	*m_pSrvNormal			= nullptr;
	ID2D1Bitmap					*m_pBitNormal			= nullptr;

	ID3D11Texture2D				*m_pTexPosition			= nullptr;
	ID3D11RenderTargetView		*m_pRtvPosition			= nullptr;
	ID3D11ShaderResourceView	*m_pSrvPosition			= nullptr;
	ID2D1Bitmap					*m_pBitPosition			= nullptr;

	//Depth Stencil BUffer
	ID3D11Texture2D				*m_pTexDepthStencil		= nullptr;
	ID3D11DepthStencilView		*m_pDsvDepthStencil		= nullptr;
	ID3D11DepthStencilView		*m_pDsvDepthStencilReadOnly		= nullptr;
	ID3D11ShaderResourceView	*m_pSrvDepthStencil		= nullptr;
	ID2D1Bitmap					*m_pBitDepthStencil		= nullptr;

	//Muti-Sampling
	UINT						m_n4xMSAAQuality		= 0;

	//
	DownScaleFactor				m_DownScale;
	ToneFactor					m_ToneFactor;
	CShaderPack					*m_pFinalPathShaderPack	= 0;

	//Scene
	std::vector<CScene*>		m_vpScene;
	CScene						*m_pPresentScene		= nullptr;

/*
	--참고사항--
	Render Update 병렬 처리를 위한 부분이다.

	병렬모드에서는 Update가 Main Thread에서 Render가 새로 생성된 Thread에서 처리된다.

	다음 Frame오 넘어가기 위해서는, 두개의 Thread가 반드시 모두 끝나야 하므로,
	m_ThraedFlag를 이용하여 동기화 하였고,
	한쪽이 기다리는 시간이 크지 않고 즉각이 중요하기 때문에 Back Off 없이 Busy Waiting하도록 구현하였다.

	Window Procedure가 호출 될 수 있어야, Input Event를 처리할 수 있기 때문에
	반드시 Render와 Update는 동시에 시작되어, 동시에 끝나도록(한쪽이 기다리도록)해야한다.
	
	FuncAdvance 함수 포인터는 전략 패턴을 사용한 부분으로, 병렬모드와 일반모드를 쉽게 Switching할 수 있도록 사용되었다.

*/
	//Thread
	std::thread*				m_pRenderThread = nullptr;
	volatile bool				m_bRenderThread = false;
	volatile std::atomic<byte>	m_ThraedFlag = 1;

	void (*FuncAdvance)(CFramework*) = NormalAdvance;

public:
	static int	m_nScreenWidth;
	static int	m_nScreenHeight;

private:
	void CreateDevice();
	void CreateSwapChain(HWND hWnd);
	void CreateHdrBuffer();
	void CreateBloomBuffer();
	void CreateGBuffer();
	void CreateDepthStencilBuffer();

public:
	CFramework(){}
	~CFramework(){
		Release();
	}

	void Initialize(HWND hWnd);

	void Release(){
		SAFE_RELEASE(m_pDevice);
		SAFE_RELEASE(m_pImmediateContext);
		SAFE_RELEASE(m_pDeferredContext);	
		
		SAFE_RELEASE(m_pDXGISwapChain);
		
		SAFE_RELEASE(m_pRtvBackBuffer);
		SAFE_RELEASE(m_pSrvBackBuffer);
		SAFE_RELEASE(m_pUavBackBuffer);
		
		SAFE_RELEASE(m_pDsvDepthStencil);
		SAFE_RELEASE(m_pSrvDepthStencil);
	}

	void CreateUI();

	void Update(float fElapsedTime);
	void Render();

	static void RenderThread(CFramework* pThis);

	//일반 모드
	static void NormalAdvance(CFramework* pThis);
	//병렬 모드
	static void ParallelAdvance(CFramework* pThis);

	void FrameAdvance();

	LRESULT CALLBACK MouseInputProc(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK KeyboardInputProc(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK DragDropProc(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
}FW;

__declspec(selectany) int		CFramework::m_nScreenWidth = 0;
__declspec(selectany) int		CFramework::m_nScreenHeight = 0;
