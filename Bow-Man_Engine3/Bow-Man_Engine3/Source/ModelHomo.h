#pragma once

/*
	--참고사항--

	본 프레임워크의 Render 단위는 CModel이다.

	하지만 Entity의 기본단위는 CObject이고, CObject는 CModelAssembly를 통하여 여러 CModel을 관리한다.
	
*/

//Dummy Declare
class CModel;
class CRenderer;

class CModelHomo {
private:
	UINT					m_tRenderer;
	CRenderer				*m_pRenderer;
	std::vector<CModel*>	m_vpModel;

public:
	CModelHomo() {}
	~CModelHomo() {}

	void Release();

	void AddModel(CModel* pModel) {
		m_vpModel.push_back(pModel);
	}

	void Render(ID3D11DeviceContext *pDeviceContext);

	void AddToRenderData(const XMFLOAT4X4& mtxWorld);

	void SetRenderer(UINT tRenderer);
	void SetRenderer(CRenderer* pRenderer);

	UINT GetRendererType() {
		return m_tRenderer;
	}
	CRenderer* GetRenderer() {
		return m_pRenderer;
	}
};