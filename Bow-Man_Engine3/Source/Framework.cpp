#include "stdafx.h"
#include "Bow-Man_Engine3.h"
#include "Framework.h"
#include "D2DFramework.h"
#include "ChatDataBase.h"
#include "ServerFramework.h"
#include "InputFramework.h"
#include "RenderState.h"
#include "Timer.h"

#include "ResourceData.h"
#include "ModelAssembly.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"
#include "Renderer.h"
#include "Sound.h"

#include "Scene.h"

#include "Object.h"
#include "Light.h"
#include "Player.h"
#include "Camera.h"

//Timers
CStopWatch m_UpdateTimer = CStopWatch(_T("Update"));
CStopWatch m_RenderTimer = CStopWatch(_T("Render"));

void CFramework::CreateDevice(){
	UINT dwCreateDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
	dwCreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE DriverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};
	UINT nDriverTypes = sizeof(DriverTypes) / sizeof(D3D_DRIVER_TYPE);

	D3D_FEATURE_LEVEL pFeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};

	UINT nFeatureLevels = _ARRAYSIZE(pFeatureLevels);
	D3D_DRIVER_TYPE nDriverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL nFeatureLevel = D3D_FEATURE_LEVEL_11_0;
	HRESULT hResult = S_OK;

	for(UINT i = 0; i < nDriverTypes; i++)
	{
		nDriverType = DriverTypes[i];
		if(SUCCEEDED(hResult = D3D11CreateDevice(NULL,nDriverType,NULL,dwCreateDeviceFlags,pFeatureLevels,nFeatureLevels,D3D11_SDK_VERSION,&m_pDevice,&nFeatureLevel,&m_pImmediateContext))) break;
	}
	if(!m_pDevice){
		MessageBox(NULL,L"D3D11Device Creation Failure",L"Error",NULL);
		exit(-1);
	}
	m_pDevice->CreateDeferredContext(NULL,&m_pDeferredContext);

	gpDevice = m_pDevice;
	gpImmediateContext = m_pImmediateContext;
	gpDeferredContext = m_pDeferredContext;
}

void CFramework::CreateSwapChain(HWND hWnd){
	RECT rcClient;
	GetClientRect(hWnd,&rcClient);
	m_nScreenWidth = rcClient.right - rcClient.left;
	m_nScreenHeight = rcClient.bottom - rcClient.top;

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc,sizeof(dxgiSwapChainDesc));
	dxgiSwapChainDesc.BufferCount = 1;
	dxgiSwapChainDesc.BufferDesc.Width = m_nScreenWidth;
	dxgiSwapChainDesc.BufferDesc.Height = m_nScreenHeight;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	//UNORDERED_ACCESS가 있으면 멀티 샘플링 텍스쳐를 생성할 수 없는듯 하다
//	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_UNORDERED_ACCESS;
	dxgiSwapChainDesc.OutputWindow = hWnd;
	dxgiSwapChainDesc.SampleDesc.Count = 1;
	dxgiSwapChainDesc.SampleDesc.Quality = 0;
	dxgiSwapChainDesc.Windowed = TRUE;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
	/*전체화면 모드로 전환할 때 현재의 후면 버퍼 설정에 가장 잘 부합하는 디스플레이 모드가 자동적으로 선택된다*/
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hResult = S_OK;
	HR(hResult = m_pDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM,4,&m_n4xMSAAQuality));

#ifdef _WITH_MSAA4_MULTISAMPLING
	dxgiSwapChainDesc.SampleDesc.Count = 4;
	dxgiSwapChainDesc.SampleDesc.Quality = m_n4xMSAAQuality - 1;
#else
	dxgiSwapChainDesc.SampleDesc.Count = 1;
	dxgiSwapChainDesc.SampleDesc.Quality = 0;
#endif

	IDXGIFactory1 *pDxgiFactory = NULL;
	HR(hResult = CreateDXGIFactory1(__uuidof(IDXGIFactory1),(void **)&pDxgiFactory));
	IDXGIDevice *pdxgiDevice = NULL;
	HR(hResult = m_pDevice->QueryInterface(__uuidof(IDXGIDevice),(void **)&pdxgiDevice));
	HR(hResult = pDxgiFactory->CreateSwapChain(pdxgiDevice,&dxgiSwapChainDesc,&m_pDXGISwapChain));
	if(pdxgiDevice) pdxgiDevice->Release();
	if(pDxgiFactory) pDxgiFactory->Release();

	//DXGI_USAGE a;
	//ID3D11Texture2D *pBackBuffer;
	//HR(m_pDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pBackBuffer));
	//pBackBuffer->IDXGIResource::GetUsage(&a);

	ID3D11Texture2D *pBackBuffer;
	HR(m_pDXGISwapChain->GetBuffer(0,__uuidof(ID3D11Texture2D),(LPVOID *)&pBackBuffer));
	m_pTexBackBuffer = pBackBuffer;
	HR(m_pDevice->CreateRenderTargetView(pBackBuffer,NULL,&m_pRtvBackBuffer));

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	::ZeroMemory(&SRVDesc,sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SRVDesc.Texture2D.MipLevels = 1;
	HR(m_pDevice->CreateShaderResourceView(pBackBuffer,&SRVDesc,&m_pSrvBackBuffer));

	D3D11_UNORDERED_ACCESS_VIEW_DESC UAViewDesc;
	UAViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	UAViewDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	UAViewDesc.Texture2D.MipSlice = 0;
	HR(m_pDevice->CreateUnorderedAccessView(pBackBuffer,&UAViewDesc,&m_pUavBackBuffer));
	pBackBuffer->Release();
}

void CFramework::CreateHdrBuffer() {
	//HDR Texture Description
	D3D11_TEXTURE2D_DESC Texture2DDesc;
	::ZeroMemory(&Texture2DDesc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2DDesc.Width = m_nScreenWidth;
	Texture2DDesc.Height = m_nScreenHeight;
	Texture2DDesc.MipLevels = 1;
	Texture2DDesc.ArraySize = 1;
	Texture2DDesc.SampleDesc.Count = 1;
	Texture2DDesc.SampleDesc.Quality = 0;
	Texture2DDesc.Usage = D3D11_USAGE_DEFAULT;
	Texture2DDesc.Format = DXGI_FORMAT_R16G16B16A16_TYPELESS;
	Texture2DDesc.CPUAccessFlags = 0;
	Texture2DDesc.MiscFlags = 0;
	Texture2DDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	//Create Texture
	ID3D11Texture2D	*pTexture;
	HR(m_pDevice->CreateTexture2D(&Texture2DDesc, nullptr, &pTexture));
	m_pTexHDR = pTexture;

	//Create Render Target View
	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;
	::ZeroMemory(&RTVDesc, sizeof(RTVDesc));
	RTVDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	HR(m_pDevice->CreateRenderTargetView(pTexture, &RTVDesc, &m_pRtvHDR));

	//Create Shader Resource View
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	::ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	SRVDesc.Texture2D.MipLevels = 1;
	HR(m_pDevice->CreateShaderResourceView(pTexture, &SRVDesc, &m_pSrvHDR));

	//Texture Release
	pTexture->Release();

	int n = 16;
	int n2 = 1024;
	m_nDownScaleGroups = (UINT)ceil(static_cast<float>((m_nScreenWidth * m_nScreenHeight) / (n * n2)));

	//Intermediate
	D3D11_BUFFER_DESC IntermediateBuffDesc;
	ZeroMemory(&IntermediateBuffDesc, sizeof(IntermediateBuffDesc));
	IntermediateBuffDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	IntermediateBuffDesc.StructureByteStride = sizeof(float);
	IntermediateBuffDesc.ByteWidth = IntermediateBuffDesc.StructureByteStride * m_nDownScaleGroups;
	IntermediateBuffDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	m_pDevice->CreateBuffer(&IntermediateBuffDesc, NULL, &m_pBuffIntermediate);

	D3D11_UNORDERED_ACCESS_VIEW_DESC IntermediateUAVDesc;
	ZeroMemory(&IntermediateUAVDesc, sizeof(IntermediateUAVDesc));
	IntermediateUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	IntermediateUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	IntermediateUAVDesc.Buffer.NumElements = m_nDownScaleGroups;
	m_pDevice->CreateUnorderedAccessView(m_pBuffIntermediate, &IntermediateUAVDesc, &m_pUavIntermediate);

	D3D11_SHADER_RESOURCE_VIEW_DESC IntermediateSRVDesc;
	ZeroMemory(&IntermediateSRVDesc, sizeof(IntermediateSRVDesc));
	IntermediateSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	IntermediateSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	IntermediateSRVDesc.Buffer.NumElements = m_nDownScaleGroups;
	m_pDevice->CreateShaderResourceView(m_pBuffIntermediate, &IntermediateSRVDesc, &m_pSrvIntermediate);

	//Average
	D3D11_BUFFER_DESC AverageBuffDesc;
	ZeroMemory(&AverageBuffDesc, sizeof(AverageBuffDesc));
	AverageBuffDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	AverageBuffDesc.StructureByteStride = sizeof(float);
	AverageBuffDesc.ByteWidth = AverageBuffDesc.StructureByteStride;
	AverageBuffDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	m_pDevice->CreateBuffer(&AverageBuffDesc, NULL, &m_pBuffAverage);

	D3D11_UNORDERED_ACCESS_VIEW_DESC AverageUAVDesc;
	ZeroMemory(&AverageUAVDesc, sizeof(AverageUAVDesc));
	AverageUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	AverageUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	AverageUAVDesc.Buffer.NumElements = 1;
	m_pDevice->CreateUnorderedAccessView(m_pBuffAverage, &AverageUAVDesc, &m_pUavAverage);

	D3D11_SHADER_RESOURCE_VIEW_DESC AverageSRVDesc;
	ZeroMemory(&AverageSRVDesc, sizeof(AverageSRVDesc));
	AverageSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	AverageSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	AverageSRVDesc.Buffer.NumElements = 1;
	m_pDevice->CreateShaderResourceView(m_pBuffAverage, &AverageSRVDesc, &m_pSrvAverage);

	//Previous Lum
	m_pDevice->CreateBuffer(&AverageBuffDesc, NULL, &m_pBuffPrevAverage);
	m_pDevice->CreateUnorderedAccessView(m_pBuffPrevAverage, &AverageUAVDesc, &m_pUavPrevAverage);
	m_pDevice->CreateShaderResourceView(m_pBuffPrevAverage, &AverageSRVDesc, &m_pSrvPrevAverage);

	//Constant Buffer
	D3D11_BUFFER_DESC ConstantBuffDesc;
	ZeroMemory(&ConstantBuffDesc, sizeof(ConstantBuffDesc));
	ConstantBuffDesc.Usage = D3D11_USAGE_DYNAMIC;
	ConstantBuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ConstantBuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ConstantBuffDesc.ByteWidth = sizeof(DownScaleFactor);
	m_pDevice->CreateBuffer(&ConstantBuffDesc, NULL, &m_pcbDownFactor);

	ConstantBuffDesc.ByteWidth = sizeof(ToneFactor);
	m_pDevice->CreateBuffer(&ConstantBuffDesc, NULL, &m_pcbToneFactor);
}

void CFramework::CreateBloomBuffer()
{
	D3D11_TEXTURE2D_DESC Txt2DDesc;
	::ZeroMemory(&Txt2DDesc,sizeof(Txt2DDesc));
	Txt2DDesc.Width = m_nScreenWidth / 4;
	Txt2DDesc.Height = m_nScreenHeight / 4;
	Txt2DDesc.MipLevels = 1;
	Txt2DDesc.ArraySize = 1;
	Txt2DDesc.SampleDesc.Count = 1;
	Txt2DDesc.SampleDesc.Quality = 0;
	Txt2DDesc.Usage = D3D11_USAGE_DEFAULT;
	Txt2DDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	Txt2DDesc.CPUAccessFlags = 0;
	Txt2DDesc.MiscFlags = 0;
	Txt2DDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	::ZeroMemory(&SRVDesc,sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	SRVDesc.Texture2D.MipLevels = 1;

	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
	UAVDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	UAVDesc.Texture2D.MipSlice = 0;

	HR(m_pDevice->CreateTexture2D(&Txt2DDesc, NULL, &m_pTexHdrQuarter));
	HR(m_pDevice->CreateShaderResourceView(m_pTexHdrQuarter,&SRVDesc,&m_pSrvHdrQuarter));
	HR(m_pDevice->CreateUnorderedAccessView(m_pTexHdrQuarter,&UAVDesc,&m_pUavHdrQuarter));

	ID3D11Texture2D *pTexture;
	HR(m_pDevice->CreateTexture2D(&Txt2DDesc, NULL, &pTexture));
	HR(m_pDevice->CreateShaderResourceView(pTexture,&SRVDesc,&m_pSrvBloom1));
	HR(m_pDevice->CreateUnorderedAccessView(pTexture,&UAVDesc,&m_pUavBloom1));
	SAFE_RELEASE(pTexture);

	HR(m_pDevice->CreateTexture2D(&Txt2DDesc, NULL, &pTexture));
	HR(m_pDevice->CreateShaderResourceView(pTexture,&SRVDesc,&m_pSrvBloom2));
	HR(m_pDevice->CreateUnorderedAccessView(pTexture,&UAVDesc,&m_pUavBloom2));
	SAFE_RELEASE(pTexture);

	HR(m_pDevice->CreateTexture2D(&Txt2DDesc, NULL, &pTexture));
	HR(m_pDevice->CreateShaderResourceView(pTexture,&SRVDesc,&m_pSrvBloomFinal));
	HR(m_pDevice->CreateUnorderedAccessView(pTexture,&UAVDesc,&m_pUavBloomFinal));
	SAFE_RELEASE(pTexture);
}

void CFramework::CreateGBuffer() {
	D3D11_TEXTURE2D_DESC Texture2DDesc;
	::ZeroMemory(&Texture2DDesc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2DDesc.Width = m_nScreenWidth;
	Texture2DDesc.Height = m_nScreenHeight;
	Texture2DDesc.MipLevels = 1;
	Texture2DDesc.ArraySize = 1;
	Texture2DDesc.SampleDesc.Count = 1;
	Texture2DDesc.SampleDesc.Quality = 0;
	Texture2DDesc.Usage = D3D11_USAGE_DEFAULT;
	Texture2DDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2DDesc.CPUAccessFlags = 0;
	Texture2DDesc.MiscFlags = 0;
	Texture2DDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;
	::ZeroMemory(&RTVDesc, sizeof(RTVDesc));
	RTVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	::ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SRVDesc.Texture2D.MipLevels = 1;

	D2D1_BITMAP_PROPERTIES bmpprops;
	bmpprops.dpiX = 96.0f;
	bmpprops.dpiY = 96.0f;
	bmpprops.pixelFormat.format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bmpprops.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE ;

	IDXGISurface *pDxgiSurface = nullptr;
	
	//Albedo
	Texture2DDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	RTVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bmpprops.pixelFormat.format = DXGI_FORMAT_R8G8B8A8_UNORM;
	HR(m_pDevice->CreateTexture2D(&Texture2DDesc, nullptr, &m_pTexAlbedo));
	HR(m_pDevice->CreateRenderTargetView(m_pTexAlbedo, &RTVDesc, &m_pRtvAlbedo));
	HR(m_pDevice->CreateShaderResourceView(m_pTexAlbedo, &SRVDesc, &m_pSrvAlbedo));
	HR(m_pTexAlbedo->QueryInterface(&pDxgiSurface));
	D2DFramework->GetRenderTarget()->CreateSharedBitmap(__uuidof(IDXGISurface), (void*)pDxgiSurface, &bmpprops, &m_pBitAlbedo);
	
	//Normal
	Texture2DDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	RTVDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	SRVDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	bmpprops.pixelFormat.format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	HR(m_pDevice->CreateTexture2D(&Texture2DDesc, nullptr, &m_pTexNormal));
	HR(m_pDevice->CreateRenderTargetView(m_pTexNormal, &RTVDesc, &m_pRtvNormal));
	HR(m_pDevice->CreateShaderResourceView(m_pTexNormal, &SRVDesc, &m_pSrvNormal));
	HR(m_pTexNormal->QueryInterface(&pDxgiSurface));
	D2DFramework->GetRenderTarget()->CreateSharedBitmap(__uuidof(IDXGISurface), (void*)pDxgiSurface, &bmpprops, &m_pBitNormal);

	//Position
	Texture2DDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	RTVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	SRVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	bmpprops.pixelFormat.format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	HR(m_pDevice->CreateTexture2D(&Texture2DDesc, nullptr, &m_pTexPosition));
	HR(m_pDevice->CreateRenderTargetView(m_pTexPosition, &RTVDesc, &m_pRtvPosition));
	HR(m_pDevice->CreateShaderResourceView(m_pTexPosition, &SRVDesc, &m_pSrvPosition));
	HR(m_pTexPosition->QueryInterface(&pDxgiSurface));
	D2DFramework->GetRenderTarget()->CreateSharedBitmap(__uuidof(IDXGISurface), (void*)pDxgiSurface, &bmpprops, &m_pBitPosition);
}

void CFramework::CreateDepthStencilBuffer(){
	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC BufferDesc;
	ZeroMemory(&BufferDesc,sizeof(D3D11_TEXTURE2D_DESC));
	BufferDesc.Width = m_nScreenWidth;
	BufferDesc.Height = m_nScreenHeight;
	BufferDesc.MipLevels = 1;
	BufferDesc.ArraySize = 1;
	BufferDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
#ifdef _WITH_MSAA4_MULTISAMPLING
	BufferDesc.SampleDesc.Count = 4;
	BufferDesc.SampleDesc.Quality = m_n4xMSAAQuality - 1;
#else
	BufferDesc.SampleDesc.Count = 1;
	BufferDesc.SampleDesc.Quality = 0;
#endif
	BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	BufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	BufferDesc.CPUAccessFlags = 0;
	BufferDesc.MiscFlags = 0;

	HR(m_pDevice->CreateTexture2D(&BufferDesc,NULL,&m_pTexDepthStencil));

	//DepthStencilView
	D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc;
	ZeroMemory(&DepthStencilViewDesc,sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	DepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
#ifdef _WITH_MSAA4_MULTISAMPLING
	DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
#else
	DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
#endif
	DepthStencilViewDesc.Texture2D.MipSlice = 0;
	HR(m_pDevice->CreateDepthStencilView(m_pTexDepthStencil,&DepthStencilViewDesc,&m_pDsvDepthStencil));


	DepthStencilViewDesc.Flags = D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL;
	HR(m_pDevice->CreateDepthStencilView(m_pTexDepthStencil,&DepthStencilViewDesc,&m_pDsvDepthStencilReadOnly));

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	::ZeroMemory(&SRVDesc,sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	SRVDesc.Texture2D.MipLevels = 1;
	SRVDesc.Texture2D.MostDetailedMip = 0;
	HR(m_pDevice->CreateShaderResourceView(m_pTexDepthStencil,&SRVDesc,&m_pSrvDepthStencil));

	D2D1_BITMAP_PROPERTIES bmpprops;
	bmpprops.dpiX = 96.0f;
	bmpprops.dpiY = 96.0f;
	bmpprops.pixelFormat = { DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_IGNORE };

	IDXGISurface *pDxgiSurface = nullptr;
	HR(m_pTexDepthStencil->QueryInterface(&pDxgiSurface));
	D2DFramework->GetRenderTarget()->CreateSharedBitmap(__uuidof(IDXGISurface), (void*)pDxgiSurface, &bmpprops, &m_pBitDepthStencil);
}

void CFramework::Initialize(HWND hWnd){
	ghIMC = ImmGetContext(hWnd);
	m_hWnd = hWnd;

	CreateDevice();
	CreateSwapChain(hWnd);
	CreateConstantBuffers<ObjectMatrix>(m_pDevice, &gCB_World);

	//Direct 2D Framework
	D2DFramework->CreateInstance();
	D2DFramework->Initialize(hWnd, m_pTexBackBuffer);

	CreateHdrBuffer();
	CreateBloomBuffer();
	CreateGBuffer();
	CreateDepthStencilBuffer();
	
	//Input Framework
	InputFramework->CreateInstance();
	InputFramework->Initialize(hWnd,gWindowRect);

	//Ui Manager
	UiManager->CreateInstance();
	UiManager->Initialize(hWnd);
	(XMVerifyCPUSupport()) ? SystemMessage(L"SIMD Available!") : SystemMessage(L"SIMD Unavailable!");
	
	//Render States(Blend, Depth Stemcil, Rasterizer, Sampler)
	RenderState->CreateInstance();
	RenderState->Initialize(m_pDevice);

	////Sound Framework
	//CoInitializeEx(NULL, COINIT_MULTITHREADED);
	//ComPtr<IXAudio2> pXAudio2 = nullptr;
	//
	//HRESULT hr;
	//if (FAILED(hr = XAudio2Create(&pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR)))
	//	cout << "Error On Creating XAudio" << endl;
	//
	//IXAudio2MasteringVoice* pMasterVoice = NULL;
	//if (FAILED(hr = pXAudio2->CreateMasteringVoice(&pMasterVoice, 1)))
	//	cout << "Error On Creating MasteringVoice" << endl;
	//
	////1. Declare WAVEFORMATEXTENSIBLE and XAUDIO2_BUFFER structures.
	//WAVEFORMATEXTENSIBLE wfx = { 0 };
	//XAUDIO2_BUFFER buffer = { 0 };
	//
	////2. Open the audio file with CreateFile.
	//WCHAR * strFileName = _TEXT("../../Resource/Sound/airplane.wav");
	//HANDLE hFile = CreateFile(
	//	strFileName,
	//	GENERIC_READ,
	//	FILE_SHARE_READ,
	//	NULL,
	//	OPEN_EXISTING,
	//	0,
	//	NULL);
	//
	//if (INVALID_HANDLE_VALUE == hFile)
	//	cout << HRESULT_FROM_WIN32(GetLastError()) << endl;
	//
	//if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
	//	cout << HRESULT_FROM_WIN32(GetLastError()) << endl;
	//
	////3. Locate the 'RIFF' chunk in the audio file, and check the file type.
	//DWORD dwChunkSize;
	//DWORD dwChunkPosition;
	////check the file type, should be fourccWAVE or 'XWMA'
	//FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition);
	//DWORD filetype;
	//ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);
	//if (filetype != fourccWAVE)
	//	cout << "Error Not fourccWAVE" << endl;
	//
	////4. Locate the 'fmt ' chunk, and copy its contents into a WAVEFORMATEXTENSIBLE structure.
	//FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);
	//ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);
	//
	////5. Locate the 'data' chunk, and read its contents into a buffer.
	////fill out the audio data buffer with the contents of the fourccDATA chunk
	//FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
	//BYTE * pDataBuffer = new BYTE[dwChunkSize];
	//ReadChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition);
	//
	////6. Populate an XAUDIO2_BUFFER structure.
	//buffer.AudioBytes = dwChunkSize;  //buffer containing audio data
	//buffer.pAudioData = pDataBuffer;  //size of the audio buffer in bytes
	//buffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer
	//
	////7. 
	//WAVEFORMATEX w;
	//w.wFormatTag = wfx.Format.wFormatTag; ;
	//w.nChannels = wfx.Format.nChannels; ;
	//w.nSamplesPerSec = wfx.Format.nSamplesPerSec; ;
	//w.nAvgBytesPerSec = wfx.Format.nAvgBytesPerSec; ;
	//w.nBlockAlign = wfx.Format.nBlockAlign; ;
	//w.wBitsPerSample = wfx.Format.wBitsPerSample; ;
	//w.cbSize = wfx.Format.cbSize; ;
	//IXAudio2SourceVoice* pSourceVoice = NULL;
	//if (FAILED(hr = pXAudio2->CreateSourceVoice(&pSourceVoice, (WAVEFORMATEX*)&wfx)))
	//	cout << "Error " << hr << endl;
	//
	////8. 
	//if (FAILED(hr = pSourceVoice->SubmitSourceBuffer(&buffer)))
	//	cout << "Error " << endl;
	//
	//if (FAILED(hr = pSourceVoice->Start(0)))
	//	cout << "Error " << endl;
	//Sleep(4000);
	//Resource Manager

	ResourceManager->CreateInstance();
	ResourceManager->Initialize(m_pDevice);

	//Timer Engine
	TimerEngine->CreateInstance();
	TimerEngine->Initialize();

	//StopWatch Manager
	StopWatchMananger->CreateInstance();
	StopWatchMananger->AddStopWatch(_T("Update"));
	StopWatchMananger->AddStopWatch(_T("Render"));
	StopWatchMananger->AddStopWatch(_T("UI-Render"));
	StopWatchMananger->AddStopWatch(_T("Elapsed-Time"));
	StopWatchMananger->CreateUI({0, 80});

	//Shader Manager(Resource Manager에 통합 예정)
	ShaderManager->CreateInstance();
	ShaderManager->Initialize(m_pDevice);

	//Light Manager
	LightManager->CreateInstance();
	LightManager->Initialize(m_pDevice);

	RendererManager->CreateInstance();
	RendererManager->Initialize();

	Server->CreateInstance();
	Server->Initialize();

	XMStoreFloat4x4(&CObject::m_mtxIdentity, XMMatrixIdentity());
//	CObject::CreateConstantBuffer(m_pDevice);
	CCameraObject::CreateConstantBuffer(m_pDevice);
//	CModel::CreateConstantBuffer(m_pDevice);
	CMaterial::CreateConstantBuffer(m_pDevice);

	//DownScale Factor
	m_DownScale.nWidth = m_nScreenWidth / 4;
	m_DownScale.nHeight = m_nScreenHeight / 4;
	m_DownScale.nTotalPixels = m_DownScale.nWidth * m_DownScale.nHeight;
	m_DownScale.nGroupSize = m_nDownScaleGroups;
	m_DownScale.fBloomThreshold = 1;
	m_DownScale.fAdaption = 0;
	m_DownScale.fBloomThreshold = 1;

	//Tone Factor
	m_ToneFactor.fMiddleGrey = 8;
	m_ToneFactor.fWhite = 1;
	m_ToneFactor.fLumWhiteSqr = pow(m_ToneFactor.fWhite * m_ToneFactor.fMiddleGrey, 2);
	m_ToneFactor.fBloomScale = 1;

	m_pFinalPathShaderPack = ShaderManager->GetShaderPack(_T("FinalPath"));

	CScene* pNewScene = new CScene;
	pNewScene->CreateScene(m_pDevice, m_pImmediateContext);
	m_vpScene.push_back(pNewScene);
	m_pPresentScene = pNewScene;

	CreateUI();
}

void CFramework::CreateUI() {
	CUiObject *pUiObject = UiManager->CreateUi(L"System", UiElement::TITLE|UiElement::EXIT|UiElement::MINIMIZE|UiElement::RESIZE);
		CUiObject *pChild = new CUiObject(UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE);
		pChild = new CUiObject(UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE);
		pChild->SetTitle(L"Visible Object : ");
		pChild->SetSize(20, 20);
		pChild->SetData(&CScene::m_nRenderedObject);
		pChild->SetRenderFunc(RenderInt);
	pUiObject->AddChild(pChild);
		pChild = new CUiObject(UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE);
		pChild->SetTitle(L"Visible Model : ");
		pChild->SetSize(20, 20);
		pChild->SetData(&CScene::m_nRenderedModel);
		pChild->SetRenderFunc(RenderInt);
	pUiObject->AddChild(pChild);
		pChild = new CUiObject(UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE);
		pChild->SetTitle(L"Visible Homo : ");
		pChild->SetSize(20, 20);
		pChild->SetData(&CScene::m_nHomoModel);
		pChild->SetRenderFunc(RenderInt);
	pUiObject->AddChild(pChild);
		pChild = new CUiObject(UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE);
		pChild->SetTitle(L"Visible Point Light : ");
		pChild->SetSize(20, 20);
		pChild->SetData(&CScene::m_nPointLight);
		pChild->SetRenderFunc(RenderInt);
	pUiObject->AddChild(pChild);
		pChild = new CUiObject(UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE);
		pChild->SetTitle(L"Visible Spot Light : ");
		pChild->SetSize(20, 20);
		pChild->SetData(&CScene::m_nSpotLight);
		pChild->SetRenderFunc(RenderInt);
	pUiObject->AddChild(pChild);
		pChild = new CUiObject(UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE);
		pChild->SetTitle(L"Num of Renderer : ");
		pChild->SetSize(20, 20);
		pChild->SetData(&CScene::m_nRenderer);
		pChild->SetRenderFunc(RenderInt);
	pUiObject->AddChild(pChild);
		pChild = new CUiObject(UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE);
		pChild->SetTitle(L"Update Burden : ");
		pChild->SetSize(20, 20);
		pChild->SetData(&g_nBurdon);
		pChild->SetRenderFunc(RenderInt);
	pUiObject->AddChild(pChild);
		pChild = new CUiObject(UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE);
		pChild->SetTitle(L"Developer's Blog");
		pChild->SetSize(20, 20);
		pChild->SetData(new std::wstring(L"http://blog.naver.com/xmfhvlex"));
		pChild->SetMouseInputFunc(MouseInputWebPage);
		pChild->SetRenderFunc(RenderElement);
	pUiObject->AddChild(pChild);
	pUiObject->SetPosition(0, 190);

	pUiObject = UiManager->CreateUi(L"G-Buffers", UiElement::TITLE | UiElement::EXIT | UiElement::MINIMIZE | UiElement::RESIZE);
		pChild = new CUiObject(UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE);
		pChild->SetTitle(L"Albedo Buffer");
		pChild->SetSize(0, 100);
		pChild->SetData(m_pBitAlbedo);
		pChild->SetRenderFunc(RenderBitmap);
	pUiObject->AddChild(pChild);
		pChild = new CUiObject(UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE);
		pChild->SetTitle(L"Normal Buffer");
		pChild->SetSize(0, 100);
		pChild->SetData(m_pBitNormal);
		pChild->SetRenderFunc(RenderBitmap);
	pUiObject->AddChild(pChild);
	pChild = new CUiObject(UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE);
		pChild->SetTitle(L"Position Buffer");
		pChild->SetSize(0, 100);
		pChild->SetData(m_pBitPosition);
		pChild->SetRenderFunc(RenderBitmap);
	pUiObject->AddChild(pChild);
	pUiObject->SetPosition(0, 370);

	static std::wstring str;
	str += _T("1. 마우스 휠 버튼 클릭 : Mouse Capture On/Off\n");
	str += _T("  └W,A,S,D,Q,E 이동 + Shift(가속)\n");
	str += _T("  └ESC key로 선택 UI 제거 및 프로그램 종료\n");
	str += _T("2. Object 왼쪽 마우스 클릭 : Model Part Ui\n");
	str += _T("  └Texture Database UI에서 드래그 하여 적용 가능\n");
	str += _T("3. Object 오른쪽 마우스 클릭 : Object Ui\n");
	str += _T("4. 폴더로부터 텍스쳐 드래그 드랍 삽입 가능\n");
	str += _T("5. 실시간 Render State 변경 가능\n\n");
	str += _T("--------------주요 기능--------------\n");
	str += _T("6. ~Key : Update Render 병렬 처리모드 On/Off\n");
	str += _T("  └←→↑↓Key : Update 부하 증감(Update Burden)\n");
	pUiObject = UiManager->CreateUi(L"사용 설명서", UiElement::TITLE | UiElement::EXIT | UiElement::MINIMIZE | UiElement::RESIZE);
	pUiObject->SetSizeX(400);
		pChild = new CUiObject(UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE);
		pChild->SetSize(0, 200);
		pChild->SetData(&str);
		pChild->SetRenderFunc(RenderWstring);
	pUiObject->AddChild(pChild);
	pUiObject->SetPosition(750, 570);
}

void CFramework::Update(float fElapsedTime) {
	StopWatchMananger->BeginTimer(Time::Update);
	m_UpdateTimer.BeginTimer();
	atomic_thread_fence(std::memory_order_seq_cst);
	RendererManager->Update(fElapsedTime);
	m_pPresentScene->Update(m_pImmediateContext, fElapsedTime);
	InputFramework->Update(fElapsedTime);
	D2DFramework->Update(fElapsedTime);
	static float num = 0;
	for (int i = 0; i<g_nBurdon; ++i) {
		num += fElapsedTime;
	}
	num = 0;
	atomic_thread_fence(std::memory_order_seq_cst);
	StopWatchMananger->EndTimer(Time::Update, fElapsedTime);
	m_UpdateTimer.EndTimer(fElapsedTime);
}

void CFramework::Render() {
	//Render Sequenc
	/*
	Multi-Render Target Render(Deferred Render)

	Light Render

	Skybox Render

	Foward Render

	Blend Render

	HDR Down Scale

	Bloom

	Final Path

	UI Render
	*/

	StopWatchMananger->BeginTimer(Time::Render);
	m_RenderTimer.BeginTimer();
	atomic_thread_fence(std::memory_order_seq_cst);

	ID3D11RenderTargetView *RTV_NULL[] = { nullptr, nullptr, nullptr, nullptr };
	ID3D11UnorderedAccessView *UAV_NULL[] = { nullptr, nullptr, nullptr, nullptr };
	ID3D11ShaderResourceView *SRV_NULL[] = { nullptr, nullptr, nullptr, nullptr };
	ID3D11Buffer *BUF_NULL[] = { nullptr, nullptr, nullptr, nullptr };

	m_pPresentScene->Render(m_pImmediateContext);
	LightManager->UpdateConstantBuffer(m_pImmediateContext);

	//Deferred Render
	ID3D11RenderTargetView *RTV_MULTI[] = { m_pRtvAlbedo, m_pRtvNormal, m_pRtvPosition };
	m_pImmediateContext->OMSetRenderTargets(ARRAYSIZE(RTV_MULTI), RTV_MULTI, m_pDsvDepthStencil);
	RendererManager->RenderDeferred(m_pImmediateContext);
	m_pImmediateContext->OMSetRenderTargets(ARRAYSIZE(RTV_NULL), RTV_NULL, nullptr);

	//Light Process
	ID3D11ShaderResourceView *SRV_LIGHT[] = { m_pSrvAlbedo, m_pSrvNormal, m_pSrvPosition, m_pSrvDepthStencil };
	m_pImmediateContext->OMSetRenderTargets(1, &m_pRtvHDR, m_pDsvDepthStencilReadOnly);
	m_pImmediateContext->PSSetShaderResources(0, ARRAYSIZE(SRV_LIGHT), SRV_LIGHT);
	LightManager->LightProcess(m_pImmediateContext);
	m_pImmediateContext->OMSetRenderTargets(ARRAYSIZE(RTV_NULL), RTV_NULL, nullptr);
	m_pImmediateContext->PSSetShaderResources(0, ARRAYSIZE(SRV_NULL), SRV_NULL);

	//SkyBox Render
	ID3D11ShaderResourceView *SRV_SKYBOX[] = { m_pSrvAlbedo, m_pSrvNormal, m_pSrvPosition, m_pSrvDepthStencil };
	m_pImmediateContext->PSSetShaderResources(0, ARRAYSIZE(SRV_SKYBOX), SRV_SKYBOX);
	m_pImmediateContext->OMSetRenderTargets(1, &m_pRtvHDR, NULL);
	RendererManager->RenderSkybox(m_pImmediateContext);
	m_pImmediateContext->OMSetRenderTargets(ARRAYSIZE(RTV_NULL), RTV_NULL, nullptr);
	m_pImmediateContext->PSSetShaderResources(0, ARRAYSIZE(SRV_NULL), SRV_NULL);

	//Foward Render
	m_pImmediateContext->OMSetRenderTargets(1, &m_pRtvHDR, m_pDsvDepthStencil);
	RendererManager->RenderFoward(m_pImmediateContext);
	RendererManager->RenderBlend(m_pImmediateContext);
	m_pImmediateContext->OMSetRenderTargets(ARRAYSIZE(RTV_NULL), RTV_NULL, nullptr);

	//HDR---------------------------------------------(HDR Calculation)
	float g_fAdaptation = 1.0f;
	m_DownScale.fAdaption = min(TimeElapsed / g_fAdaptation, 0.9999f);
	//	m_DownScale.fAdaption = min(g_fAdaptation < 0.0001f ? 1.0f : TimeElapsed / g_fAdaptation, 0.9999f);
	UpdateConstantBuffer(m_pImmediateContext, m_pcbDownFactor, m_DownScale);

	//HDR
	ID3D11UnorderedAccessView *UAV[] = { m_pUavIntermediate, m_pUavHdrQuarter };
	m_pImmediateContext->CSSetUnorderedAccessViews(0, 2, UAV, NULL);
	m_pImmediateContext->CSSetShaderResources(0, 1, &m_pSrvHDR);
	m_pImmediateContext->CSSetConstantBuffers(0, 1, &m_pcbDownFactor);
	g_pDownScaleFirstPass->Dispatch(m_pImmediateContext, m_nDownScaleGroups, 1, 1);
	m_pImmediateContext->CSSetUnorderedAccessViews(0, ARRAYSIZE(UAV_NULL), UAV_NULL, NULL);
	m_pImmediateContext->CSSetShaderResources(0, ARRAYSIZE(SRV_NULL), SRV_NULL);
	m_pImmediateContext->CSSetConstantBuffers(0, ARRAYSIZE(BUF_NULL), BUF_NULL);

	ID3D11UnorderedAccessView *UAV2[] = { m_pUavAverage };
	ID3D11ShaderResourceView *SRV2[] = { m_pSrvIntermediate, m_pSrvPrevAverage };
	m_pImmediateContext->CSSetUnorderedAccessViews(0, ARRAYSIZE(UAV2), UAV2, NULL);
	m_pImmediateContext->CSSetShaderResources(0, ARRAYSIZE(SRV2), SRV2);
	m_pImmediateContext->CSSetConstantBuffers(0, 1, &m_pcbDownFactor);
	g_pDownScaleSecondPass->Dispatch(m_pImmediateContext, 1, 1, 1);
	m_pImmediateContext->CSSetUnorderedAccessViews(0, ARRAYSIZE(UAV_NULL), UAV_NULL, NULL);
	m_pImmediateContext->CSSetShaderResources(0, ARRAYSIZE(SRV_NULL), SRV_NULL);
	m_pImmediateContext->CSSetConstantBuffers(0, ARRAYSIZE(BUF_NULL), BUF_NULL);

	//Bloom
	ID3D11ShaderResourceView *SRV_BLOOM[] = { m_pSrvHdrQuarter, m_pSrvPrevAverage };
	m_pImmediateContext->CSSetUnorderedAccessViews(0, 1, &m_pUavBloom1, NULL);
	m_pImmediateContext->CSSetShaderResources(0, ARRAYSIZE(SRV_BLOOM), SRV_BLOOM);
	m_pImmediateContext->CSSetConstantBuffers(0, 1, &m_pcbDownFactor);
	g_pBloomFactor->Dispatch(m_pImmediateContext, m_nDownScaleGroups, 1, 1);
	m_pImmediateContext->CSSetUnorderedAccessViews(0, ARRAYSIZE(UAV_NULL), UAV_NULL, NULL);
	m_pImmediateContext->CSSetShaderResources(0, ARRAYSIZE(SRV_NULL), SRV_NULL);
	m_pImmediateContext->CSSetConstantBuffers(0, ARRAYSIZE(BUF_NULL), BUF_NULL);

	//Virtical
	m_pImmediateContext->CSSetUnorderedAccessViews(0, 1, &m_pUavBloom2, NULL);
	m_pImmediateContext->CSSetShaderResources(0, 1, &m_pSrvBloom1);
	m_pImmediateContext->CSSetConstantBuffers(0, 1, &m_pcbDownFactor);
	g_pBloomVerticle->Dispatch(m_pImmediateContext, (UINT)ceil(m_nScreenWidth / 4.0f), (UINT)ceil((m_nScreenHeight / 4.0f) / (128.0f - 12.0f)), 1);
	m_pImmediateContext->CSSetUnorderedAccessViews(0, ARRAYSIZE(UAV_NULL), UAV_NULL, NULL);
	m_pImmediateContext->CSSetShaderResources(0, ARRAYSIZE(SRV_NULL), SRV_NULL);
	m_pImmediateContext->CSSetConstantBuffers(0, ARRAYSIZE(BUF_NULL), BUF_NULL);

	//Horizontal
	m_pImmediateContext->CSSetUnorderedAccessViews(0, 1, &m_pUavBloomFinal, NULL);
	m_pImmediateContext->CSSetShaderResources(0, 1, &m_pSrvBloom2);
	m_pImmediateContext->CSSetConstantBuffers(0, 1, &m_pcbDownFactor);
	g_pBloomHorizontal->Dispatch(m_pImmediateContext, (UINT)ceil((m_nScreenWidth / 4.0f) / (128.0f - 12.0f)), (UINT)ceil(m_nScreenHeight / 4.0f), 1);
	m_pImmediateContext->CSSetUnorderedAccessViews(0, ARRAYSIZE(UAV_NULL), UAV_NULL, NULL);
	m_pImmediateContext->CSSetShaderResources(0, ARRAYSIZE(SRV_NULL), SRV_NULL);
	m_pImmediateContext->CSSetConstantBuffers(0, ARRAYSIZE(BUF_NULL), BUF_NULL);

	//Final Path
	UpdateConstantBuffer(m_pImmediateContext, m_pcbToneFactor, m_ToneFactor);
	ID3D11ShaderResourceView *SRV3[] = { m_pSrvHDR, m_pSrvAverage, m_pSrvBloomFinal };
	m_pImmediateContext->PSSetSamplers(0, 1, &SAMPLER_STATE[ESmplrState::CLAMP]);
	m_pImmediateContext->OMSetDepthStencilState(NULL, 1);
	m_pImmediateContext->RSSetState(NULL);
	m_pImmediateContext->OMSetBlendState(NULL, NULL, 0xffffffff);

	m_pImmediateContext->OMSetRenderTargets(1, &m_pRtvBackBuffer, nullptr);
	m_pImmediateContext->PSSetShaderResources(0, ARRAYSIZE(SRV3), SRV3);
	m_pImmediateContext->PSSetConstantBuffers(ETC, 1, &m_pcbToneFactor);
	m_pFinalPathShaderPack->QuadRender(m_pImmediateContext);
	m_pImmediateContext->OMSetRenderTargets(ARRAYSIZE(RTV_NULL), RTV_NULL, nullptr);
	m_pImmediateContext->PSSetShaderResources(0, ARRAYSIZE(SRV_NULL), SRV_NULL);
	m_pImmediateContext->PSSetConstantBuffers(0, ARRAYSIZE(BUF_NULL), BUF_NULL);

	//UI Render
	StopWatchMananger->BeginTimer(Time::UI_Render);
	D2DFramework->RenderInfo(m_UpdateTimer, m_RenderTimer);
	D2DFramework->Render();
	StopWatchMananger->EndTimer(Time::UI_Render, gfElapsedTime);

	m_pDXGISwapChain->Present(0, 0);

	std::swap(m_pBuffAverage, m_pBuffPrevAverage);
	std::swap(m_pUavAverage, m_pUavPrevAverage);
	std::swap(m_pSrvAverage, m_pSrvPrevAverage);

	//			float m_fClearColor[4]{ 0.7, 0.9, 1, 1 };
	float m_fClearColor[4]{ 0.0, 0.0, 0, 1 };
	m_pImmediateContext->ClearRenderTargetView(m_pRtvBackBuffer, m_fClearColor);
	m_pImmediateContext->ClearRenderTargetView(m_pRtvHDR, m_fClearColor);
	m_pImmediateContext->ClearRenderTargetView(m_pRtvAlbedo, m_fClearColor);
	m_pImmediateContext->ClearRenderTargetView(m_pRtvNormal, m_fClearColor);
	m_pImmediateContext->ClearRenderTargetView(m_pRtvPosition, m_fClearColor);
	m_pImmediateContext->ClearDepthStencilView(m_pDsvDepthStencil, D3D11_CLEAR_DEPTH, 1.0f, 0);

	atomic_thread_fence(std::memory_order_seq_cst);
	StopWatchMananger->EndTimer(Time::Render, gfElapsedTime);
	m_RenderTimer.EndTimer(gfElapsedTime);
}

void CFramework::RenderThread(CFramework * pThis) {
	while (pThis->m_bRenderThread) {
		while (pThis->m_ThraedFlag == 0);
		pThis->Render();

		atomic_thread_fence(std::memory_order_seq_cst);
		pThis->m_ThraedFlag = 0;
	}
}

void CFramework::NormalAdvance(CFramework * pThis) {
	pThis->Update(gfElapsedTime);
	pThis->Render();
}

void CFramework::ParallelAdvance(CFramework * pThis) {
	pThis->m_ThraedFlag = 1;
	atomic_thread_fence(std::memory_order_seq_cst);
	pThis->Update(gfElapsedTime);

	//Waiting Render Thread
	while (pThis->m_ThraedFlag);

	PivotUpdate();
}

void CFramework::FrameAdvance() {
	FuncAdvance(this);

	StopWatchMananger->EndTimer(Time::Elapsed_Time, gfElapsedTime);
	gfElapsedTime = TimerEngine->Update();
	StopWatchMananger->BeginTimer(Time::Elapsed_Time);

	#pragma region Data Flush
	CScene::m_nRenderedObject = 0;
	CScene::m_nRenderedModel = 0;
	CScene::m_nHomoModel = 0;
	CScene::m_nPointLight = 0;
	CScene::m_nSpotLight = 0;
	#pragma endregion Data Flush
}

LRESULT CFramework::MouseInputProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!InputFramework->IsMouseActive()) {
		InputFramework->MouseInputProc(hWnd, message, wParam, lParam);
	}
	else {
		if (D2DFramework->MouseInputProc(hWnd, message, wParam, lParam) == false) {
			InputFramework->MouseInputProc(hWnd, message, wParam, lParam);
			m_pPresentScene->MouseInputProc(hWnd, message, wParam, lParam);
		}
	}

//	if(InputFramework->IsMouseActive()) D2DFramework->MouseInputProc(hWnd, message, wParam, lParam);

	//switch(message){
 //   case WM_SIZE:{
	///*	FrameBuffer->ResizeBuffer(hWnd, m_pDevice, m_pDeviceContext, lParam);
	//	cout << m_nScreenWidth << "   " << m_nScreenHeight << endl;

	//	CCamera* pCamera = Player->GetCamera();
	//	if (pCamera) pCamera->CreateViewport(m_pDeviceContext, 0, 0, m_nScreenWij
	//	';lkjhg\]
	//	['p\
	//	]['dth, m_nScreenHeight, 0.0f, 1.0f);
	//	break;*/
	//}
	//case WM_LBUTTONDOWN:case WM_LBUTTONUP: case WM_RBUTTONDOWN:
	//case WM_RBUTTONUP:	case WM_MOUSEMOVE: case WM_MOUSEWHEEL:
	//case WM_MBUTTONDOWN:	
	//	InputFramework->MouseInputProc(hWnd, message, wParam, lParam);
	//	D2DFramework->MouseInputProc(hWnd, message, wParam, lParam);
 //       break;
	//}

	return LRESULT();
}

LRESULT CFramework::KeyboardInputProc(HWND hWnd,UINT nMessageID,WPARAM wParam,LPARAM lParam)
{
	if(D2DFramework->KeyInputProc(hWnd, nMessageID, wParam, lParam)) return NULL;
	m_pPresentScene->KeyboardInputProc(hWnd, nMessageID, wParam, lParam);

	HINSTANCE hInst;

	switch(nMessageID){
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_ESCAPE:
			switch (MessageBox(NULL, L"Exit The Engine?", L"Exit", MB_OKCANCEL)) {
			case 1:
				gbLoop = false;
				InputFramework->Release();
				TimerEngine->Release();
				RenderState->Release();
				D2DFramework->Release();
				Server->Release();
				for (CScene *pScene : m_vpScene) {
					pScene->DestroyScene();
					delete pScene;
				}
				m_pPresentScene = nullptr;
				m_vpScene.clear();
				break;
			case 2:
				SetCapture(hWnd);
				SetCursor(NULL);
				break;
			}
		}
		break;
	case WM_KEYUP:
		switch (wParam) {
		case VK_OEM_3:
			if (m_pRenderThread) {
				TimerEngine->SetName(_T("Normal"));
				m_bRenderThread = false;
				m_ThraedFlag = 1;
				atomic_thread_fence(std::memory_order_seq_cst);
				m_pRenderThread->join();
				delete m_pRenderThread;
				m_pRenderThread = nullptr;
				FuncAdvance = NormalAdvance;
				CRenderer::SwitchToParalell(false);
				SetParallel(false);
			}
			else{
				TimerEngine->SetName(_T("Parllel "));
				m_bRenderThread = true;
				m_ThraedFlag = 0;
				atomic_thread_fence(std::memory_order_seq_cst);
				m_pRenderThread = new std::thread{ RenderThread, this };
				FuncAdvance = ParallelAdvance;
				CRenderer::SwitchToParalell(true);
				SetParallel(true);
			}
			break;
		case VK_1:
			CreateUI();
			break;
		case VK_2:
			ShaderManager->CreateUI(POINT{ 1500, 0 });
			break;
		case VK_3:
			ResourceManager->CreateUI(POINT{ 2000, 0 });
			break;
		case VK_4:

			break;
		case VK_5:

			break;
		case VK_DOWN:
			g_nBurdon -= 1000000;
			if (g_nBurdon < 0)
				g_nBurdon = 0;
			break;
		case VK_UP:
			g_nBurdon += 1000000;
			break;
		case VK_LEFT:
			g_nBurdon -= 100000;
			if (g_nBurdon < 0)
				g_nBurdon = 0;
			break;
		case VK_RIGHT:
			g_nBurdon += 100000;
			break;
		case VK_P:
		//	ShellExecute(0, 0, L"http://blog.naver.com/xmfhvlex", 0, 0, SW_SHOW);
		//	ShellExecute(0, L"open", L"iexplore", L"http://blog.naver.com/xmfhvlex", NULL, SW_SHOW);
		//	ShellExecute(0, L"open", L"chrome", L"http://blog.naver.com/xmfhvlex", NULL, SW_SHOW);
		//	ShellExecute(NULL, TEXT("explore"), TEXT("c:\windows"), NULL, NULL, SW_SHOW);
	//		ShellExecuteW();
	//		hInst = ShellExecute(hWnd, L"open", L"D:/01. Program/Color Script/Color Scripter/Color Scripter.exe", L"", L"", SW_SHOW);
		//	hInst = ShellExecute(hWnd, L"open", L"HoneyView3", L"original.png", L"../../", SW_SHOW);
		//	hInst = ShellExecute(hWnd, L"open", L"D:/01. Program/HoneyView3/HoneyView3", L"original.png", L"../../Resource/", SW_SHOW);
		//	hInst = ShellExecute(hWnd, L"open", L"D:/01. Program/HoneyView3/HoneyView3", L"../../Resource/original.png", NULL, SW_SHOW);
		//	switch (LOWORD(hInst)) {
		//	case ERROR_FILE_NOT_FOUND:
		//		cout << "ERROR_FILE_NOT_FOUND" << endl;
		//		break;
		//	}
		//	"D:\01. Program\Color Script\Color Scripter\Color Scripter.exe"
			break;
		case VK_K: {
	//		m_pPresentScene->DestroyScene();
	//		m_pPresentScene->CreateScene(m_pDevice, m_pImmediateContext);
			//static int num = 0;
			//num++;
			//if(num >= m_vpScene.size())
			//	num = 0;
			//std::cout << num << std::endl;
			//m_pPresentScene = m_vpScene[num];
			break;
			}
		}
		break;
	}
	return LRESULT();
}

LRESULT CFramework::DragDropProc(HWND hWnd,UINT nMessageID,WPARAM wParam,LPARAM lParam)
{
	UINT buffsize = 512;
	WCHAR buf[512];
	HDROP hDropInfo = (HDROP) wParam;
	DragQueryFile(hDropInfo, 0, buf, buffsize);
//	SystemMessage(buf);
	
	ResourceManager->AddResourceFromFile(gpDevice, buf);

	return LRESULT();
}
