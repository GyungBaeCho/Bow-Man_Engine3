#pragma once
#include "ParallelPivot.h"

//dummy class
class CStopWatch;
class CStopWatch;
class CUiManager;

enum class BRUSH_COLOR : int{
	WHITE,
	BLACK,
	RED,
	GREEN,
	BLUE,
	MAGENTA,
	YELLOW,
	CYAN,
	WHITE_A25,
	WHITE_A50,
	WHITE_A75,
};

__declspec(selectany) ID2D1RenderTarget						*gpD2DRenderTarget = nullptr;
__declspec(selectany) IDWriteTextFormat						*gpTextFormat = nullptr;
__declspec(selectany) IDWriteTextFormat						*gpWorldTextFormat = nullptr;
__declspec(selectany) std::vector<ID2D1SolidColorBrush*>	*gpvpD2DBrush;
__declspec(selectany) ID2D1SolidColorBrush*					gpD2DBrush = nullptr;
__declspec(selectany) ID3D11ShaderResourceView				*gpShaderResource = nullptr;

struct ChatData;

//Direct 2D Framework
#define D2DFramework	CD2DFramework::GetInstance()

class CD2DFramework
{
private:
	static CD2DFramework				*m_pInstance;

	ID2D1DeviceContext					*m_pD2DeviceContext = nullptr;
	ID2D1RenderTarget					*m_pRenderTarget	= nullptr;
	ID2D1Factory						*m_pD2DFactory		= nullptr;
	IDWriteFactory						*m_pWirteFactory	= nullptr;
	IDWriteTextFormat					*m_pTextFormat		= nullptr;
	std::vector<ID2D1SolidColorBrush*>  m_vpD2DBrush;

	CUiManager							*m_pUiManager		= nullptr;

private:
	CD2DFramework(){
		m_pRenderTarget		= nullptr;
		m_pD2DFactory		= nullptr;
		m_pWirteFactory		= nullptr;
		m_pTextFormat		= nullptr;
	}
	~CD2DFramework(){
		Release();
	//	ID2D1DeviceContext;
	}

public:
	static void CreateInstance(){	if(!m_pInstance) m_pInstance = new CD2DFramework;	}
	static CD2DFramework* GetInstance(){	return m_pInstance;	}

//	void Initialize(HWND hWnd, IDXGISwapChain* pDXGISwapChain);
	void Initialize(HWND hWnd, ID3D11Texture2D *pBackBuffer);

	void Release(){
		SAFE_RELEASE(m_pRenderTarget);
		SAFE_RELEASE(m_pD2DFactory);
		SAFE_RELEASE(m_pWirteFactory);
		SAFE_RELEASE(m_pTextFormat);
		for(ID2D1SolidColorBrush *pD2DBrush : m_vpD2DBrush) SAFE_RELEASE(pD2DBrush);
	}

	void Update(float fElapsedTime);
	void Render();
	void RenderInfo(CStopWatch& update, CStopWatch& render);

	ID2D1DeviceContext* GetDeviceContext() {
		return m_pD2DeviceContext;
	}
	ID2D1RenderTarget* GetRenderTarget() {
		return m_pRenderTarget;
	}

	bool MouseInputProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
	bool KeyInputProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
};

__declspec(selectany) CD2DFramework		*CD2DFramework::m_pInstance = nullptr;

ID2D1Bitmap * LoadTexture(const WCHAR * pwTextureFile);
