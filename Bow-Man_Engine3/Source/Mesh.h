#pragma once
#include "ResourceData.h"
#include "GmtryEnum.h"

//Dummy Class
//class CResource;
class CHeightMap;
struct PickingData;
//class CResource;

//--------------------------------------------------
struct VertexBuffer{
	UINT			m_uInputLayout = 0;
	ID3D11Buffer*	m_pVertexBuffer = nullptr;
	UINT			m_nStride = 0;
	UINT			m_nOffset = 0;

	VertexBuffer() {}
	void Release(){
		if(m_pVertexBuffer) 
			m_pVertexBuffer->Release();;
		delete this;
	}
};

struct IndexBuffer{
	DXGI_FORMAT		m_dxgiIndexFormat = DXGI_FORMAT_R32_UINT;
	UINT			m_nIndexOffset = 0;
	ID3D11Buffer	*m_pIndexBuffer = nullptr;
};

class CMeshBuffer {
private:
	UINT						m_uInputLayout = 0;
	D3D11_PRIMITIVE_TOPOLOGY	m_PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	std::vector<ID3D11Buffer*>	m_vpVertexBuffer;
	std::vector<UINT>			m_vnStride;
	std::vector<UINT>			m_vnOffset;

	IndexBuffer					*m_pIndexBuffer = nullptr;
	UINT						m_nPosition = 0;
	UINT						m_nIndex = 0;

public:
	CMeshBuffer(D3D11_PRIMITIVE_TOPOLOGY primitiveTopology, UINT nPosition, UINT nIndex) 
		: m_PrimitiveTopology(primitiveTopology), m_nPosition(nPosition), m_nIndex(nIndex){}
	~CMeshBuffer() {}

	void AddVertexBuffer(VertexBuffer* pVertexBuffer);

	void SetIndexBuffer(IndexBuffer* pIndexBuffer) {
		if (!pIndexBuffer) return;
		m_pIndexBuffer = pIndexBuffer;
	}

	void Render(ID3D11DeviceContext *pDeviceContext);

	UINT GetInputLayout () {
		return m_uInputLayout;
	}
};

enum EMeshElement : UINT{
	INDEX =			1,
	RIGID =			4,
	LOOK_UP =		8,
	BOTH_SIDE =		16,
};

struct MeshData{
	std::vector<XMFLOAT3>	m_vvPosition;
	std::vector<XMFLOAT3>	m_vvTangent;
	std::vector<XMFLOAT3>	m_vvNormal;
	std::vector<XMFLOAT2>	m_vvTexCoord1;
	std::vector<XMFLOAT2>	m_vvTexCoord2;
	std::vector<XMFLOAT4>	m_vvColor;
	std::vector<UINT>		m_vIndex;
	BoundingBox				m_AABB;

	void Clear(){
		m_vvPosition.clear();
		m_vvNormal.clear();
		m_vvTexCoord1.clear();
		m_vvTexCoord2.clear();
		m_vvColor.clear();
		m_vIndex.clear();
	}
};

class CMesh : public CResource{
protected:
	std::wstring					m_wsParentName;
	UINT							m_uInputLayout = 0;
	D3D11_PRIMITIVE_TOPOLOGY		m_PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	std::vector<VertexBuffer*>		m_vpVertexBuffer; //CMesh에 대한 모든 VertexBuffer//CMesh에 대한 모든 VertexBuffer
	IndexBuffer						*m_pIndexBuffer = nullptr;
	std::map<UINT, CMeshBuffer*>	m_mpMeshBuffer;
	std::vector<XMFLOAT3>			m_vvPosition;
	std::vector<UINT>				m_vIndex;
	UINT							m_nStartVertex = 0;
	UINT							m_nStartIndex = 0;
	UINT							m_nBaseVertex = 0;
	BoundingBox						m_AABB;

public:
	CMesh(){}
	CMesh(ID3D11Device *pDevice, const MeshData& meshData){
	//	CreateTBN(meshData);
		CreateMesh(pDevice, meshData);
		CreateAABB();
	}
	
	virtual ~CMesh() {}
	
	virtual void Release() {}

	virtual void Render(ID3D11DeviceContext *pDeviceContext);

	void CreateMesh(ID3D11Device *pDevice, const MeshData& meshData);
	void CreateTBN(MeshData& meshData);
	void CreateAABB();

	template<typename T>
	ID3D11Buffer *CreateBuffer(ID3D11Device *pDevice, T& vData, UINT nBindFlags, D3D11_USAGE usage, UINT nCPUAccessFlags);

	template<typename T>
	void AddVertexBuffer(ID3D11Device *pDevice, T& vData, EVertexElement element);

	void SetIndexBuffer(ID3D11Device *pDevice, std::vector<UINT>vData);

	bool MeshPicking(const XMVECTOR & vRayPosition, const XMVECTOR & vRayDirection, PickingData &pickingData);

	//아무 값도 넣지 않으면 최상위 옵션이 부여된다
	CMeshBuffer* GetMeshBuffer(UINT uInputLayout = _INT(EVertexElement::ALL));

	const BoundingBox &GetAABB() {
		return m_AABB;
	}
};

class CSectorMesh : public CMesh
{
private:
protected:
public:
	CSectorMesh(ID3D11Device *pDevice, float fWidth, float fHeight, float fDepth, int iSectorNum, bool bFlat, XMCOLOR vColor);
	CSectorMesh(ID3D11Device *pDevice, float fWidth, float fHeight, float fDepth, int iSectorNum, CHeightMap *pHeightMap);
	virtual ~CSectorMesh(){}
};

class CSquareMesh : public CMesh
{
public:
	CSquareMesh(ID3D11Device *pDevice, float fWidth, float fHeight, UINT element=0);
	virtual ~CSquareMesh(){}
};

class CWarpSquareMesh : public CMesh
{
public:
	CWarpSquareMesh(ID3D11Device * pDevice, float fWidth, float fHeight, UINT uSplit, UINT element=0);
	virtual ~CWarpSquareMesh(){}
};

class CCubeMesh : public CMesh
{
public:
	CCubeMesh(ID3D11Device *pDevice, float fWidth, float fHeight, float fDepth, UINT element=0);
	virtual ~CCubeMesh(){}
};

class CCrossBillboardMesh : public CMesh
{
public:
	CCrossBillboardMesh(ID3D11Device *pDevice, float fWidth, float fHeight, float fDepth, UINT element = 0);
	virtual ~CCrossBillboardMesh() {}
};

class CSkyboxMesh : public CMesh 
{
public:
	CSkyboxMesh(ID3D11Device *pDevice, float fSizeX, float fSizeY, float fSizeZ);
	virtual ~CSkyboxMesh() {}
};

class CCTerrainMesh : public CMesh {
private:
	XMFLOAT3 m_f3Scale;

public:
	CCTerrainMesh(ID3D11Device *pDevice,int indexX,int indexZ,float fPivotXPos,float fPivotZPos,int fVertexDistant,int nMeshColumn,XMFLOAT3 f3Scale,void *pContext);
	CCTerrainMesh(ID3D11Device *pDevice, float fSizeX, float fSizeZ, CHeightMap *pHeightMap);
	CCTerrainMesh(ID3D11Device *pDevice, int xStart, int zStart, int nWidth, int nLength, CHeightMap *pHeightMap);
	CCTerrainMesh(ID3D11Device *pDevice, CHeightMap *pHeightMap);
	CCTerrainMesh(ID3D11Device *pDevice, int xStart, int zStart, int nWidth, int nLength, int num, CHeightMap *pHeightMap);
	virtual ~CCTerrainMesh() {}
};