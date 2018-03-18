#pragma once
#include "ResourceData.h"

//Dummy Class
//class CResource;

//--------------------------------------------------
class CMaterial : public CResource{
private:
	XMFLOAT4				m_vDiffuseSpecExp;
	
public:
	static ID3D11Buffer*	m_pcbMaterialColors;
	
public:
	CMaterial(float r, float g, float b, float specular){		
		m_vDiffuseSpecExp = XMFLOAT4(r, g, b, specular);		
	}
	CMaterial(XMFLOAT4 vColor){
		m_vDiffuseSpecExp = vColor;
	}
	virtual ~CMaterial(){}

	static void CreateConstantBuffer(ID3D11Device *pDevice);

	void Release() override {
		if (m_pcbMaterialColors) m_pcbMaterialColors->Release();
	}

	void SetResourceToDevice(ID3D11DeviceContext *pDeviceContext);

	void Render(ID3D11DeviceContext *pDeviceContext) override {}

	const XMFLOAT4& GetMaterialColor() const {
		return m_vDiffuseSpecExp;
	}
};
__declspec(selectany) ID3D11Buffer* CMaterial::m_pcbMaterialColors = nullptr;