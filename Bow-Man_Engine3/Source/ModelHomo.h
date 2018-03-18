#pragma once

/*
	--�������--

	�� �����ӿ�ũ�� Render ������ CModel�̴�.

	������ Entity�� �⺻������ CObject�̰�, CObject�� CModelAssembly�� ���Ͽ� ���� CModel�� �����Ѵ�.
	
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