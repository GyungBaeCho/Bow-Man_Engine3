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
	--�������--

	Render State�� Render State�� �����Ͽ� �ϳ��� ���� ID������ ����� ���� ������ ������.

	���� SHADER�� ������ 1~999�� �� �����ӿ�ũ�� �ִ� 999���� Shader(Pipeline �ܰ��� ����)�� ���� �� �ִ�.
	Depth_Stencil_State�� ���� 1000~100000������ �ڸ����� ���� 1~100���� �ν��Ͽ� 100���� ������ ���� �� �ִ�.

	�� Render State�� ID�� ������ ���ٸ�.
	Shader				= 12,
	Depth_Stencil_State = 43,
	Rasterizer_State	= 3,
	Blend_State			= 7,

	���� Render State�� ���� �����ϸ� "70343012"�� �ȴ�.
	�̸� Renderer ID��� �θ���.

	CModel�� Renderer ID�� ���Ͽ� map���� Renderer�� �˻��Ͽ�, ��� ������ �����Ѵ�.
	Renderer�� �ڽŰ� ������ ID�� ���� CModel���� ��Ƽ� Render�Ѵ�.

	Runtime�� Renderer map�� ���� Renderer ID�� ��Ÿ����, ���ο� Renderer�� �����Ͽ� ����Ѵ�.
*/

enum class ERendererStateSlot : UINT{
	SHADER		= 0000000001,
	DPTH_STNCL	= 0000001000,
	RSTRZR		= 0000100000,
	BLND		= 0010000000,

	//������� �ʴ´�.
	SHADOW		= 0100000000,
	//			= 0000000000	10��
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