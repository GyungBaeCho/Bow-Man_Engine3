#pragma once
#include "ResourceData.h"
#include "UiObject.h"

//Dummy Declare
struct PickingData;
class CModel;
class CModelHomo;

class CModelAssembly : public CResource {
private:
	std::vector<CModel*>		m_vpModel;
	std::vector<CModelHomo*>	m_vpModelHomo;
	BoundingBox					m_AABB;

public:
	CModelAssembly() {}
	CModelAssembly(ID3D11Device* pDevice, char* pChar);
	~CModelAssembly() override {}

	CUiObject* CreateUI(const POINT& vMousePos = POINT{ 0, 0 }, UINT element = UiElement::TITLE | UiElement::EXIT | UiElement::MINIMIZE | UiElement::RESIZE | UiElement::ON_TOP, bool bBodyFlag = true);

	void CreateModelHomo();
	void ReleaseModelHomo();

	void Release() override {}

	void LoadObjFile(ID3D11Device *pDevice, WCHAR *pwChar, float fScale, UINT element);
	void LoadFbxFile(ID3D11Device *pDevice, WCHAR *pwChar, float fScale, UINT element);
	void LoadModel(ID3D11Device *pDevice, WCHAR *pwChar, float fScale, UINT element);

	void AddModel(CModel *pModelData);
	void AddToRenderer(const XMFLOAT4X4& mtxWorld);
	void Render(ID3D11DeviceContext *pDeviceContext) override;
	bool ModelAssemblyPicking(const XMVECTOR & vRayPosition, const XMVECTOR & vRayDirection, PickingData& pickData);

	const std::vector<CModel*>* GetModelData() {
		return &m_vpModel;
	}

	const BoundingBox& GetAABB() {
		return m_AABB;
	}
};