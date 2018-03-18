#pragma once

#define HIGHTMAP_SIZE	257
_declspec(selectany)	float g_fWorldSize = 4096;

//Dummy Class
class CTexture;

class CHeightMap
{
private:
	//���� �� �̹��� �ȼ�(8-��Ʈ)���� ������ �迭�̴�. �� �ȼ��� 0~255�� ���� ���´�.
	float **m_pHeightMap;
	int m_nByteWidth	= 0;
	int m_nWidth		= 0;
	int m_nLength		= 0;
	float m_fMinHeight	= +FLT_MAX;
	float m_fMaxHeight	= -FLT_MAX;
	XMFLOAT3 m_f3Scale	= XMFLOAT3(0,0,0);

	CTexture *m_pTexture = nullptr;

public:
	CHeightMap(ID3D11Device *pDevice, LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xvScale);
	~CHeightMap()
	{
		if (m_pHeightMap) delete[] m_pHeightMap;
		m_pHeightMap = NULL;
	}

	void CreateTexture(ID3D11Device *pDevice);
	void Smoothing();

	//���� �� �̹������� (x, z) ��ġ�� �ȼ� ���� ����� ������ ���̸� ��ȯ�Ѵ�.
	float GetHeight(float fx, float fz);
	float GetHeight(XMFLOAT3 pos){
		return GetHeight(pos.x, pos.z);		
	}

	//���� �� �̹������� (x, z) ��ġ�� ���� ���͸� ��ȯ�Ѵ�.
	XMFLOAT3 GetNormal(float x, float z);
	XMFLOAT3 GetNormal(XMFLOAT3 pos) { 
		return GetNormal(pos.x, pos.z); 
	}

	XMFLOAT3 GetScale() { 
		return m_f3Scale; 
	}

	float **GetHeightMap() { 
		return m_pHeightMap;
	}
	int GetHeightMapWidth() { 
		return m_nWidth-1; 
	}
	int GetHeightMapHeight() {
		return m_nLength-1; 
	}
	float GetMaxHeight(){
		return m_fMaxHeight;
	}
	float GetMinHeight(){
		return m_fMinHeight;
	}
	CTexture* GetTexture() const {
		return m_pTexture;
	}
};