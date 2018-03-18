#include "stdafx.h"
#include "Bow-Man_Engine3.h"
#include "D2DFramework.h"
#include "Framework.h"
#include "ImeFramework.h"
#include "InputFramework.h"
#include "UiObject.h"
#include "Scene.h"
#include "ChatDataBase.h"
#include "UtilityFunction.h"
#include "Texture.h"
#include "Timer.h"

//void CD2DFramework::Initialize(HWND hWnd, IDXGISwapChain * pDXGISwapChain)
void CD2DFramework::Initialize(HWND hWnd, ID3D11Texture2D *pBackBuffer)
{
//	HR(pDXGISwapChain->GetBuffer(0,__uuidof(ID3D11Texture2D),(LPVOID *)&pBackBuffer));

	IDXGISurface *pDxgiSurface = NULL;
	HR(pBackBuffer->QueryInterface(&pDxgiSurface));
	pBackBuffer->Release();

	D2D1CreateDeviceContext(pDxgiSurface, NULL, &m_pD2DeviceContext);

	HR(D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED,&m_pD2DFactory));

	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
													D2D1_RENDER_TARGET_TYPE_DEFAULT,
													D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN,D2D1_ALPHA_MODE_PREMULTIPLIED),
													96,
													96
												);

	//2D Render Target
	HR(m_pD2DFactory->CreateDxgiSurfaceRenderTarget(pDxgiSurface,&props,&m_pRenderTarget));
	

	DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,__uuidof(IDWriteFactory),reinterpret_cast<IUnknown * *>(&m_pWirteFactory));

	//Text Format
	//Yj BONMOKGAK
	m_pWirteFactory->CreateTextFormat(L"HYGothic",
									nullptr,
									DWRITE_FONT_WEIGHT_NORMAL,
									DWRITE_FONT_STYLE_NORMAL,
									DWRITE_FONT_STRETCH_NORMAL,
									14.0f,L"",&m_pTextFormat);
	gpTextFormat = m_pTextFormat;


	/*
	HY태백B
	Orbit-B BT
	Yj BLOCK
	한컴 백제 B
	휴먼중간팸체
	문체부 훈민정음체
	*/
	
	m_pWirteFactory->CreateTextFormat(L"문체부 훈민정음체",
									nullptr,
									DWRITE_FONT_WEIGHT_NORMAL,
									DWRITE_FONT_STYLE_NORMAL,
									DWRITE_FONT_STRETCH_NORMAL,
									44,L"",&gpWorldTextFormat);

	gpvpD2DBrush = &m_vpD2DBrush;
	ID2D1SolidColorBrush *pD2DBrush = nullptr;
	m_pRenderTarget->CreateSolidColorBrush(D2D1_COLOR_F{1,1,1,1},&pD2DBrush);
	m_vpD2DBrush.push_back(pD2DBrush);

	m_pRenderTarget->CreateSolidColorBrush(D2D1_COLOR_F{0,0,0,1},&pD2DBrush);
	m_vpD2DBrush.push_back(pD2DBrush);

	m_pRenderTarget->CreateSolidColorBrush(D2D1_COLOR_F{1,0,0,1},&pD2DBrush);
	m_vpD2DBrush.push_back(pD2DBrush);
	
	m_pRenderTarget->CreateSolidColorBrush(D2D1_COLOR_F{0,1,0,1},&pD2DBrush);
	m_vpD2DBrush.push_back(pD2DBrush);

	m_pRenderTarget->CreateSolidColorBrush(D2D1_COLOR_F{0,0,1,1},&pD2DBrush);
	m_vpD2DBrush.push_back(pD2DBrush);

	m_pRenderTarget->CreateSolidColorBrush(D2D1_COLOR_F{1,0,1,1},&pD2DBrush);
	m_vpD2DBrush.push_back(pD2DBrush);

	m_pRenderTarget->CreateSolidColorBrush(D2D1_COLOR_F{1,1,0,1},&pD2DBrush);
	m_vpD2DBrush.push_back(pD2DBrush);

	m_pRenderTarget->CreateSolidColorBrush(D2D1_COLOR_F{0,1,1,1},&pD2DBrush);
	m_vpD2DBrush.push_back(pD2DBrush);

	m_pRenderTarget->CreateSolidColorBrush(D2D1_COLOR_F{1,1,1,0.25f},&pD2DBrush);
	m_vpD2DBrush.push_back(pD2DBrush);
	m_pRenderTarget->CreateSolidColorBrush(D2D1_COLOR_F{1,1,1,0.50f},&pD2DBrush);
	m_vpD2DBrush.push_back(pD2DBrush);
	m_pRenderTarget->CreateSolidColorBrush(D2D1_COLOR_F{1,1,1,0.75f},&pD2DBrush);
	m_vpD2DBrush.push_back(pD2DBrush);

	m_pRenderTarget->CreateSolidColorBrush(D2D1_COLOR_F{0, 0, 0, 1},&gpD2DBrush);

	ID3D11Texture2D *pTexture;
	D3D11_TEXTURE2D_DESC d3dTextureDesc;
	ZeroMemory(&d3dTextureDesc,sizeof(D3D11_TEXTURE2D_DESC));
	d3dTextureDesc.Width = 512;
	d3dTextureDesc.Height = 512;
	d3dTextureDesc.MipLevels = 1;
	d3dTextureDesc.ArraySize = 1;
	d3dTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dTextureDesc.SampleDesc.Count = 1;
	d3dTextureDesc.SampleDesc.Quality = 0;
	d3dTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	d3dTextureDesc.CPUAccessFlags = 0;
	d3dTextureDesc.MiscFlags = 0;
	gpDevice->CreateTexture2D(&d3dTextureDesc, nullptr, &pTexture);

	ID3D11ShaderResourceView *pSrvTexture;
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	::ZeroMemory(&SRVDesc,sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SRVDesc.Texture2D.MipLevels = 1;
	HR(gpDevice->CreateShaderResourceView(pTexture,&SRVDesc,&pSrvTexture));
	gpShaderResource = pSrvTexture;

	IDXGISurface *p2DDxgiSurface = NULL;
	ID2D1RenderTarget* pD2DRenderT;
	HR(pTexture->QueryInterface(&p2DDxgiSurface));
	HR(m_pD2DFactory->CreateDxgiSurfaceRenderTarget(p2DDxgiSurface,&props,&pD2DRenderT));

	pD2DRenderT->BeginDraw();
	pD2DRenderT->Clear(D2D1_COLOR_F{0.0f, 0.0f, 0.0f, 1.0f});

	gpD2DBrush->SetColor(D2D1_COLOR_F{1,1,1,1});
	std::wstring wstr = L"나랏말싸미 듕귁에 달아 문자와를 서로 사맛디 아니할쎄 이런 전차로 어린백성이 니르고저 할빼이셔도 마참내 제 뜻을 능히펴지 못할놈이 하니다. 내이를 어여삐 녀겨 새로 스물여덟자를 맹가노니 사람마다 수비니겨 날로쓰매 편아케 하고저 할 따라미니라.";
	pD2DRenderT->DrawText(wstr.c_str(),
					wstr.size(),
					gpWorldTextFormat,
					D2D1_RECT_F{
						0,
						0,
						512,
						512
					},
					gpD2DBrush,
					D2D1_DRAW_TEXT_OPTIONS_CLIP,
					DWRITE_MEASURING_MODE_NATURAL
			);
	pD2DRenderT->EndDraw();

	D2D1_BITMAP_PROPERTIES bmpprops;
	bmpprops.dpiX = 96.0f;
	bmpprops.dpiY = 96.0f;
	bmpprops.pixelFormat = { DXGI_FORMAT_R8G8B8A8_UNORM , D2D1_ALPHA_MODE_IGNORE };
	ID2D1Bitmap *pBitmapBackBuffer;
	pD2DRenderT->CreateSharedBitmap(__uuidof(IDXGISurface), (void*)pDxgiSurface, &bmpprops, &pBitmapBackBuffer);
}

void CD2DFramework::Update(float fElapsedTime){
	UiManager->Update(fElapsedTime);
}

void CD2DFramework::Render(){
	UiManager->Render(m_pRenderTarget);
//	D2D1_DRAW_TEXT_OPTIONS_NO_SNAP;
//	D2D1_DRAW_TEXT_OPTIONS_CLIP;
//	D2D1_DRAW_TEXT_OPTIONS_NONE;
//	D2D1_DRAW_TEXT_OPTIONS_FORCE_DWORD;
//	DWRITE_MEASURING_MODE_NATURAL;
//	DWRITE_MEASURING_MODE_GDI_CLASSIC;
//	DWRITE_MEASURING_MODE_GDI_NATURAL;
	InputFramework->Render(m_pRenderTarget);
}

void CD2DFramework::RenderInfo(CStopWatch& update, CStopWatch& render) {
	m_pRenderTarget->BeginDraw();

	float fScreenWidth = FRAME_BUFFER_WIDTH;
	gpD2DBrush->SetColor(D2D1_COLOR_F{ 0, 0, 0, 0.6f });
	m_pRenderTarget->FillRectangle(D2D1_RECT_F{ 0, 0, fScreenWidth, 80 }, gpD2DBrush);

	gpD2DBrush->SetColor(D2D1_COLOR_F{ 1, 1, 1, 1 });
	WCHAR * pwChar = update.GetText();
	float fUpdateTime = update.GetAvgElapsedTime();
//	float fUpdateTime = update.GetElapsedTime();
	m_pRenderTarget->DrawText(pwChar, lstrlen(pwChar), m_pTextFormat, D2D1_RECT_F{ 0, 0, fScreenWidth, 300 }, gpD2DBrush);

	pwChar = render.GetText();
	float fRenderTime = render.GetAvgElapsedTime();
//	float fRenderTime = render.GetElapsedTime();
	m_pRenderTarget->DrawText(pwChar, lstrlen(pwChar), m_pTextFormat, D2D1_RECT_F{ 0, 20, fScreenWidth, 300 }, gpD2DBrush);

	pwChar = TimerEngine->GetAdvanceInfo();
	float fWaitTime = TimerEngine->GetAvgWaitTime();
	//	float fElapsedTime = TimerEngine->GetElapsedTime();
	m_pRenderTarget->DrawText(pwChar, lstrlen(pwChar), m_pTextFormat, D2D1_RECT_F{ 0, 40, fScreenWidth, 300 }, gpD2DBrush);

	pwChar = TimerEngine->GetText();
	float fElapsedTime = TimerEngine->GetAvgElapsedTime();
//	float fElapsedTime = TimerEngine->GetElapsedTime();
	m_pRenderTarget->DrawText(pwChar, lstrlen(pwChar), m_pTextFormat, D2D1_RECT_F{ 0, 60, fScreenWidth, 300 }, gpD2DBrush);

	float fBeginX = 250;
	fUpdateTime		*= 50000;
	fRenderTime		*= 50000;
	fWaitTime		*= 50000;
	fElapsedTime	*= 50000;
	float fMaxTime	= 1.0f/70 * 50000;;

	gpD2DBrush->SetColor(D2D1_COLOR_F{ 1, 1, 0, 0.8f });
	m_pRenderTarget->FillRectangle(D2D1_RECT_F{ fBeginX, 0, fUpdateTime + fBeginX, 20 }, gpD2DBrush);

	gpD2DBrush->SetColor(D2D1_COLOR_F{ 0, 1, 1, 0.8f });
	m_pRenderTarget->FillRectangle(D2D1_RECT_F{ fBeginX, 20, fRenderTime + fBeginX, 40 }, gpD2DBrush);

	gpD2DBrush->SetColor(D2D1_COLOR_F{ 0, 1, 0, 0.8f });
	m_pRenderTarget->FillRectangle(D2D1_RECT_F{ fBeginX, 40, fWaitTime + fBeginX, 60 }, gpD2DBrush);

	gpD2DBrush->SetColor(D2D1_COLOR_F{ 1, 0, 1, 0.8f });
	m_pRenderTarget->FillRectangle(D2D1_RECT_F{ fBeginX, 60, fElapsedTime + fBeginX, 80 }, gpD2DBrush);

	gpD2DBrush->SetColor(D2D1_COLOR_F{ 1, 1, 1, 1 });
	m_pRenderTarget->DrawLine(D2D1_POINT_2F{ fBeginX+fMaxTime , 60}, D2D1_POINT_2F{ fBeginX + fMaxTime , 80}, gpD2DBrush);

	m_pRenderTarget->EndDraw();
}

bool CD2DFramework::MouseInputProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam){
	return UiManager->MouseInputProc(hWnd,message,wParam,lParam);
}

bool CD2DFramework::KeyInputProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam){
	return UiManager->KeyInputProc(hWnd, message, wParam, lParam);
}

ID2D1Bitmap * LoadTexture(const WCHAR * pwTextureFile) {
	std::wstring wFileName;
	std::wstring wFileFormat;
	GetFileNameFormat(pwTextureFile, wFileName, wFileFormat);

	ScratchImage image;
	if (wFileFormat == L"dds") {
		HR(LoadFromDDSFile(pwTextureFile, DDS_FLAGS_NONE, nullptr, image));
	}
	else if (wFileFormat == L"tga") {
		HR(LoadFromTGAFile(pwTextureFile, nullptr, image));
	}
	else {
		LoadFromWICFile(pwTextureFile, WIC_FLAGS_NONE, nullptr, image);
	}

	ID2D1RenderTarget *pRenderTarget = D2DFramework->GetRenderTarget();

	IWICBitmapDecoder *p_image_decoder = NULL;
	IWICImagingFactory *p_wic_factory = NULL;

	CoInitialize(NULL);
	if (S_OK == CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&p_wic_factory))) {
		// p_wic_factory 를 사용하여 이미지를 읽어들이는 루틴을 구성하면 된다.
	}

	if (S_OK == p_wic_factory->CreateDecoderFromFilename(pwTextureFile, NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &p_image_decoder)) {

	}

	IWICBitmapFrameDecode *p_wic_frame_decoder = NULL;
	p_image_decoder->GetFrame(0, &p_wic_frame_decoder);

	ID2D1Bitmap* pBitmap = nullptr;
	IWICFormatConverter *p_format_converter = NULL;
	if (S_OK == p_wic_factory->CreateFormatConverter(&p_format_converter)) {
		if (S_OK == p_format_converter->Initialize(p_wic_frame_decoder, GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom)) {
			pRenderTarget->CreateBitmapFromWicBitmap(p_format_converter, NULL, &pBitmap);
		}
	}
	return pBitmap;
}
