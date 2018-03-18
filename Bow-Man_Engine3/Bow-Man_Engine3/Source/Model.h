#pragma once
#include "ResourceData.h"
#include "RenderState.h"

struct PickingData;
class CMesh;
class CMeshBuffer;
class CModelAssembly;
class CShaderPack;
class CMaterial;
class CTexture;
class CShaderPack;

/*
	--참고사항--

	Render State각 Render State를 조합하여 하나의 고유 ID값으로 만들기 위한 열거형 데이터.

	예로 SHADER의 범위는 1~999로 본 프레임워크는 최대 999개의 Shader(Pipeline 단계의 조합)를 가질 수 있다.
	Depth_Stencil_State의 경우는 1000~100000이지만 자리수를 낮춰 1~100으로 인식하여 100개의 종류를 가질 수 있다.

	각 Render State의 ID가 다음과 같다면.
	Shader				= 12,
	Depth_Stencil_State = 43,
	Rasterizer_State	= 3,
	Blend_State			= 7,

	위의 Render State의 값을 조합하면 "70343012"가 된다.
	이를 Renderer ID라고 부른다.

	CModel은 Renderer ID를 통하여 map에서 Renderer를 검색하여, 멤버 변수에 저장한다.
	Renderer는 자신과 동일한 ID를 가진 CModel들을 모아서 Render한다.

	Runtime중 Renderer map에 없는 Renderer ID가 나타나면, 새로운 Renderer를 생성하여 사용한다.
*/

enum class ERendererStateSlot : UINT{
	SHADER		= 0000000001,
	DPTH_STNCL	= 0000001000,
	RSTRZR		= 0000100000,
	BLND		= 0010000000,

	//사용하지 않는다.
	SHADOW		= 0100000000,
	//			= 0000000000	10억
};

class CModel : public CResource{
private:
	CMesh				*m_pMesh			= nullptr;
	CMeshBuffer			*m_pMeshBuffer		= nullptr;
	CTexture			*m_pTexture			= nullptr;
	CMaterial			*m_pMaterial		= nullptr;
	CModelAssembly		*m_pModelAssembly	= nullptr;

	UINT				m_tRenderer			= 0;
	CShaderPack			*m_pShaderPack		= nullptr;
	EDpthStnclState		m_eDpthStnclState	= EDpthStnclState::NONE;
	ERstrzrState		m_eRstrzrSate		= ERstrzrState::NONE;
	EBlndState			m_eBlndState		= EBlndState::NONE;
	bool				m_bShadow			= false;

public:
	CModel(){}
	~CModel() override {}

	void CalModelID();

	void Release() override;

	void CreateUI(const POINT& vMousePos);

	void Render(ID3D11DeviceContext *pDeviceContext) override;
	void Render2(ID3D11DeviceContext *pDeviceContext);

	void SetShaderPack(CShaderPack* pShader);
	void SetShaderPack(UINT uInputLayout);
	void SetShaderPack(UINT uInputLayout, std::string sName);
	
	void SetMesh(CMesh *pMesh);

	void SetModelAssembly(CModelAssembly *pModelAssembly) {
		m_pModelAssembly = pModelAssembly;
	}

	void SetTexture(CTexture *pTexture){
		if(pTexture) m_pTexture = pTexture;
	}

	void SetMaterial(CMaterial *pMaterial){
		if(pMaterial) m_pMaterial = pMaterial;
	}

	void SetRendererType(UINT tRenderer) {
		m_tRenderer = tRenderer;
	}

	void SetDptstnclState(EDpthStnclState eDpthStnclState) {
		m_eDpthStnclState = eDpthStnclState;
	}

	void SetRstrzrState(ERstrzrState eDpthStnclState) {
		m_eRstrzrSate = eDpthStnclState;
	}

	void SetBlndState(EBlndState eDpthStnclState) {
		m_eBlndState = eDpthStnclState;
	}

	bool ModelPicking(const XMVECTOR & vRayPosition, const XMVECTOR & vRayDirection, PickingData& pickData);

	CMesh* GetMesh() {
		return m_pMesh;
	}

	UINT GetShaderPackID();

	CTexture* GetTexture() {
		return m_pTexture;
	}

	CTexture** GetTexureAddress() {
		return &m_pTexture;
	}

	CMaterial* GetMaterial() {
		return m_pMaterial;
	}

	EDpthStnclState* GetDpthStncl() {
		return &m_eDpthStnclState;
	}

	ERstrzrState* GetRstrzr() {
		return &m_eRstrzrSate;
	}

	EBlndState* GetBlnd() {
		return &m_eBlndState;
	}

	UINT GetRendererType() {
		return m_tRenderer;
	}

	CModelAssembly* GetModelAssembly() {
		return m_pModelAssembly;
	}
};