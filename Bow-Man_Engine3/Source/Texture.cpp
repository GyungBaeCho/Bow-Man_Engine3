#include "stdafx.h"
//#include "ResourceData.h"
#include "Texture.h"

#include "RenderState.h"
#include "D2DFramework.h"
#include "UtilityFunction.h"

CTexture::CTexture() {		
	AddSampler(SAMPLER_STATE[ESmplrState::WARP]);
	AddSampler(SAMPLER_STATE[ESmplrState::CLAMP]);
}

CTexture::CTexture(ID3D11ShaderResourceView * pSrvTexture) : CTexture() {
	AddTexture(pSrvTexture);
	AddSampler(SAMPLER_STATE[ESmplrState::WARP]);
	AddSampler(SAMPLER_STATE[ESmplrState::CLAMP]);
}

CTexture::CTexture(ID3D11Device * pDevice, const WCHAR * pwChar) {
	AddTexture(pDevice, pwChar);
	AddSampler(SAMPLER_STATE[ESmplrState::WARP]);
	AddSampler(SAMPLER_STATE[ESmplrState::CLAMP]);
}

void CTexture::AddTexture(ID3D11Device * pDevice, const WCHAR * pwTextureFile) {
	std::wstring wFileName;
	std::wstring wFileFormat;
	GetFileNameFormat(pwTextureFile, wFileName, wFileFormat);
	SetName(wFileName);

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

	ID3D11ShaderResourceView *psrvTexture;
	CreateShaderResourceView(pDevice, image.GetImages(), image.GetImageCount(), image.GetMetadata(), &psrvTexture);
	m_vpsrvTexture.push_back(psrvTexture);


	ID2D1DeviceContext *pD2DeviceContex = D2DFramework->GetDeviceContext();
	ID2D1RenderTarget *pRenderTarget = D2DFramework->GetRenderTarget();


	IWICBitmapDecoder *p_image_decoder = NULL;
	IWICImagingFactory *p_wic_factory = NULL;

	CoInitialize(NULL);
	if (S_OK == CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&p_wic_factory))) {
		// p_wic_factory 를 사용하여 이미지를 읽어들이는 루틴을 구성하면 된다.
	}

	if (S_OK == p_wic_factory->CreateDecoderFromFilename(pwTextureFile, NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &p_image_decoder));

	IWICBitmapFrameDecode *p_wic_frame_decoder = NULL;
	// 일반적인 이미지는 첫번째 프레임에 이미지가 저장되어 있기 때문에 첫번째 프레임을 사용할 수 있는 객체를 구성한다.
	p_image_decoder->GetFrame(0, &p_wic_frame_decoder);

	// 이미지형식 변환 인터페이스
	IWICFormatConverter *p_format_converter = NULL;
	// Direct2D 용 비트맵 객체 인터페이스​

	// WIC Factory 객체를 사용하여 이미지형식 변환 객체를 생성한다.
	if (S_OK == p_wic_factory->CreateFormatConverter(&p_format_converter)) {
		// 사용자가 지정한 이미지의 첫 번째 프레임에 접근할수 있는 p_wic_frame_decoder를 넘겨서 
		// 사용자가 지정한 형식의 비트맵으로 변환한다.
		if (S_OK == p_format_converter->Initialize(p_wic_frame_decoder, GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom)) {
			// 변환된 이미지 형식을 사용하여 Direct2D용 비트맵을 생성합니다.
			pRenderTarget->CreateBitmapFromWicBitmap(p_format_converter, NULL, &m_pBitMap);
		}
	}
}
