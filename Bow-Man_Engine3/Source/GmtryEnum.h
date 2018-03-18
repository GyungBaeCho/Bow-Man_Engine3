#pragma once

enum class EVertexElement : UINT {
	POSITION = 1,
	COLOR = 2,
	TEXCOORD1 = 4,
	TEXCOORD2 = 8,
	TANGENT = 16,
	NORMAL = 32,
	BONE = 64,
	INSTANCE = 1024,

	ALL = 1+2+4+8+16+32+64+128+1024,
};

enum class EInputLayout : char{
	ILL_TEX,
	TEXTURED,
	ILLUMINATED,
	DIFFUSED,
};

const UINT gInputLayoutList[] = {
		_INT(EVertexElement::POSITION) | _INT(EVertexElement::TEXCOORD1) | _INT(EVertexElement::TEXCOORD2) | _INT(EVertexElement::TANGENT) | _INT(EVertexElement::NORMAL),
		_INT(EVertexElement::POSITION) | _INT(EVertexElement::TEXCOORD1) | _INT(EVertexElement::TANGENT) | _INT(EVertexElement::NORMAL),
		_INT(EVertexElement::POSITION) | _INT(EVertexElement::TEXCOORD1) | _INT(EVertexElement::NORMAL),
		_INT(EVertexElement::POSITION) | _INT(EVertexElement::TEXCOORD1),
		_INT(EVertexElement::POSITION) | _INT(EVertexElement::NORMAL),
		_INT(EVertexElement::POSITION) | _INT(EVertexElement::COLOR),
	};

////Model�� �Ϻκ�
//class CModel_{
//	//Shader*
//	//Mesh* 
//	//vector<Texture*>
//	//Material
//
//	//Rasterizer State
//	//Depth Stencil State
//	//Blend State
//};
//
//class CModelBundle{
//	//CRenderer* m_pRenderer;
//	//list<CModel> m_lpHomoModel;
//};
//
////�������� ���� Model ������ World ���
//class CRenderData_{
//	//CRenderData_ *m_pRenderData;
//	//XMFloat4X4 m_mtxWorld;
//};
//
////���� ����ǰ(�ϼ�ü)
//class CModelAssembly{
//	//list<CModel> m_lpModelData;
//
//	//m_tRenderer�� �������� �������� ����
//	//set<CModelBundle> m_lpModelPack;
//
//public:
//	void Create() {
//		Release();
//		//for(CModel* pModel : m_lpModelData){
//		//	pModel;
//		//
//		//
//		//}
//	}
//	void Release() {
//
//	}
//};
//
//class CRenderer{
//	//UINT m_tRenderer;
//	//Shader
//	//Model
//	//Rasterizer State
//	//Depth Stencil State
//	//Blend State
//
//	//UINT m_pvtRenderData;
//	//vector<CRenderData_>
//};
//
//class CRendererManager {
//	//list<Renderer*>	m_lpShadow;
//	//list<Renderer*>	m_lpDeferred;
//	//list<Renderer*>	m_lpFoward;
//	//list<Renderer*>	m_lBlend;
//
//};