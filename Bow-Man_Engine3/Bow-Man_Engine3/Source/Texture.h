#pragma once
#include "ResourceData.h"

//Dummy Class
//class CResource;

//--------------------------------------------------
class CTexture : public CResource{
private:
	std::vector<ID3D11ShaderResourceView*>	m_vpsrvTexture;
	std::vector<ID3D11SamplerState*>		m_vSamplerState;
	ID2D1Bitmap	*m_pBitMap;

public:
	CTexture();
	CTexture(ID3D11ShaderResourceView* pSrvTexture);
	CTexture(ID3D11Device *pDevice, const WCHAR* pwChar);

	virtual ~CTexture(){}

	virtual void Release() {}

	void AddTexture(ID3D11ShaderResourceView *psrvTexture) { 
		m_vpsrvTexture.push_back(psrvTexture);
	}
	void AddTexture(ID3D11Device *pDevice, const WCHAR* pwTextureFile);

	void AddSampler(ID3D11SamplerState *pSamplerState) {
		m_vSamplerState.push_back(pSamplerState);
	}

	void SetResourceToDevice(ID3D11DeviceContext *pDeviceContext){
		pDeviceContext->VSSetShaderResources(0, m_vpsrvTexture.size(), m_vpsrvTexture.data());
		pDeviceContext->DSSetShaderResources(0, m_vpsrvTexture.size(), m_vpsrvTexture.data());
		pDeviceContext->PSSetShaderResources(0, m_vpsrvTexture.size(), m_vpsrvTexture.data());
		pDeviceContext->VSSetSamplers(0, m_vSamplerState.size(), m_vSamplerState.data());
		pDeviceContext->DSSetSamplers(0, m_vSamplerState.size(), m_vSamplerState.data());
		pDeviceContext->PSSetSamplers(0, m_vSamplerState.size(), m_vSamplerState.data());
	}

	virtual void Render(ID3D11DeviceContext *pDeviceContext) {}

	ID2D1Bitmap* GetBitmap() {
		return m_pBitMap;
	}
};

__declspec(selectany) ID2D1Bitmap	*gpBitMap = nullptr;
