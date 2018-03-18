#include "stdafx.h"
#include "HeightMap.h"
#include "Texture.h"

//높이 맵 이미지에서 (i, z) 위치의 법선 벡터를 반환한다.
//높이 맵 이미지에서 (i, z) 위치의 픽셀 값에 기반한 지형의 높이를 반환한다.

CHeightMap::CHeightMap(ID3D11Device *pDevice, LPCTSTR pFileName, int nWidthPoint, int nLengthPoint, XMFLOAT3 xvScale)
{
/*
ReadFile를 통해 텍스트 이미지에서 정보를 받아오는 순서는 다음과 같다
	[0,0]
	   →→→→→
	   |↘
	   |  ↘
	   |    ↘
	   
그러나 2차원 배열에서, Y축이 양으로 증가하는 방식으로 구현하면 Y축이 반전된다.
       |    ↗
       |  ↗  
       |↗    
       →→→→→
	[0,0]

따라서 2차원 배열에 저장할때, Y축이 뒤집히기 때문에, GPU에서 사용할 HeightMap Resource를 생성할때, Y축을 다시 뒤집어서 만들어 줘야 한다.
	[0,0]
	   →→→→→
	   |↘
	   |  ↘
	   |    ↘
*/
	m_nWidth = nWidthPoint + 1;
	m_nLength = nLengthPoint + 1;
//	m_f3Scale = XMFLOAT3((float)(g_fWorldSize) / (nWidthPoint), xvScale.y, (float)(g_fWorldSize) / (nLengthPoint));
	m_f3Scale = xvScale;

	BYTE *pHeightMap = new BYTE[nWidthPoint * nLengthPoint];


	//파일을 열고 읽는다. 높이 맵 이미지는 파일 헤더가 없는 RAW 이미지이다.
	HANDLE hFile = ::CreateFile(pFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, NULL);

	DWORD dwBytesRead;
	::ReadFile(hFile, pHeightMap, (nWidthPoint * nLengthPoint), &dwBytesRead, NULL);
	::CloseHandle(hFile);

	m_pHeightMap = new float*[m_nWidth];
	for (int i = 0; i<m_nWidth; ++i)
		m_pHeightMap[i] = new float[m_nLength];

	for (int z = 0; z < nLengthPoint; ++z) {
		for (int x = 0; x < nWidthPoint; ++x) {
			m_pHeightMap[x][(nLengthPoint-1) - z] = pHeightMap[x + (z*nWidthPoint)] * m_f3Scale.y;
		}
	}

	for (int i = 0; i<m_nLength - 1; ++i) {
		m_pHeightMap[i][nLengthPoint] = m_pHeightMap[i][nLengthPoint - 1];
		m_pHeightMap[nWidthPoint][i] = m_pHeightMap[nWidthPoint - 1][i];
	}
	m_pHeightMap[nWidthPoint][nLengthPoint] = m_pHeightMap[nWidthPoint - 1][nLengthPoint - 1];

	Smoothing();
	CreateTexture(pDevice);

	for (int y = 0; y < nLengthPoint;  ++y) {
		for (int x = 0; x < nWidthPoint; ++x) {
			(m_fMinHeight > m_pHeightMap[x][y]) ? m_fMinHeight=m_pHeightMap[x][y] : NULL;
			(m_fMaxHeight < m_pHeightMap[x][y]) ? m_fMaxHeight=m_pHeightMap[x][y] : NULL;
		}
	}

	if (pHeightMap) delete[] pHeightMap;
}

void CHeightMap::CreateTexture(ID3D11Device *pDevice)
{
	D3D11_TEXTURE2D_DESC texDesc;
	::ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = GetHeightMapWidth();
	texDesc.Height = GetHeightMapHeight();
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	std::vector<float> vfData;
	for (int z = 0; z < GetHeightMapHeight(); ++z) {
		for (int x = 0; x < GetHeightMapWidth(); ++x) {
			vfData.push_back(m_pHeightMap[x][(m_nLength-1)-z]);
		}
	}

	std::vector<HALF> hmap(vfData.size());
	std::transform(vfData.begin(), vfData.end(), hmap.begin(), XMConvertFloatToHalf);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &vfData[0];
	data.SysMemPitch = (m_nWidth - 1) * sizeof(vfData[0]);
	data.SysMemSlicePitch = 0;

	ID3D11Texture2D *pTexture = nullptr;
	pDevice->CreateTexture2D(&texDesc, &data, &pTexture);

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	::ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Format = texDesc.Format;
	SRVDesc.Texture2D.MipLevels = -1;
	SRVDesc.Texture2D.MostDetailedMip = 0;

	ID3D11ShaderResourceView *pSRV;
	pDevice->CreateShaderResourceView(pTexture, &SRVDesc, &pSRV);
	SAFE_RELEASE(pTexture);
	hmap.clear();

	
//	ScratchImage image;
//	LoadFromWICFile(L"original.png", WIC_FLAGS_NONE, nullptr, image);
//	ID3D11ShaderResourceView *psrvTexture;
//	CreateShaderResourceView(pDevice, image.GetImages(), image.GetImageCount(), image.GetMetadata(), &psrvTexture);
//
//	ID3D11Resource *pResource;
//	pSRV->GetResource(&pResource);
//	ID3D11DeviceContext *pDeviceContext;
//	pDevice->GetImmediateContext(&pDeviceContext);
//
//	HRESULT hr_0 = CaptureTexture(  pDevice, pDeviceContext , pResource, image );
//	const Image* img = image.GetImage(0,0,0);
//	SaveToWICFile( *img, WIC_FLAGS_NONE , GUID_ContainerFormatPng , _T("1.png") );
////	SaveToWICFile(*image.GetImage(0, 0, 0), WIC_FLAGS_NONE, GUID_ContainerFormatPng, L"original.png");
//	pDeviceContext->Release();


	m_pTexture = new CTexture;
	m_pTexture->AddTexture(pDevice, _T("../../Resource/Texture/Height_Map/Base_Texture.dds"));
	m_pTexture->AddTexture(pSRV);
	m_pTexture->AddTexture(pDevice, _T("../../Resource/Texture/Height_Map/jungle_mntn_n.dds"));
	m_pTexture->AddTexture(pDevice, _T("../../Resource/Texture/Height_Map/jungle_mntn_d.dds"));
	m_pTexture->AddTexture(pDevice, _T("../../Resource/Texture/Height_Map/jungle_mntn_s.dds"));
}

void CHeightMap::Smoothing() {
	float **pHeightMapTemp;
	pHeightMapTemp = new float*[m_nWidth];
	for (int i = 0; i<m_nWidth; ++i)
		pHeightMapTemp[i] = new float[m_nLength];

	for(int i=0 ; i<m_nWidth ; ++i)
		CopyMemory(pHeightMapTemp[i], m_pHeightMap[i], m_nLength*sizeof(float));

	int nWidthPoint = m_nWidth - 1;
	int nLengthPoint = m_nLength - 1;

	for (int i = 0; i<nLengthPoint; ++i) {
		for (int j = 0; j < nWidthPoint; ++j) {
			int nCount = 0;
			float fHeight = 0;
			for (int y = -1; y < 2; ++y) {
				if (i + y < 0 || i + y > nLengthPoint) continue;
				for (int x = -1; x<2; ++x) {
					if (j + x < 0 || j + x > nWidthPoint) continue;
					nCount++;
					fHeight += pHeightMapTemp[j + x][i + y];
				}
			}
			m_pHeightMap[j][i] = fHeight / nCount;
		}
	}

	for (int i = 0; i < m_nWidth; ++i)
		delete pHeightMapTemp[i];
	delete[] pHeightMapTemp;

}

float CHeightMap::GetHeight(float fx, float fz)
{
	fx = (fx + g_fWorldSize / 2) / m_f3Scale.x;
	fz = (fz + g_fWorldSize / 2) / m_f3Scale.z;
//	fx /= m_f3Scale.x;
//	fz /= m_f3Scale.z;

	if ((fx < 0) || (fz < 0) || (fx >= m_nWidth - 1) || (fz >= m_nWidth - 1)) return(0.0f);

	int x = static_cast<int>(fx);
	int z = static_cast<int>(fz);
	float fxPercent = fx - x;
	float fzPercent = fz - z;

	float fTopLeft = m_pHeightMap[x][z + 1];
	float fTopRight = m_pHeightMap[x + 1][z + 1];
	float fBottomLeft = m_pHeightMap[x][z];
	float fBottomRight = m_pHeightMap[x + 1][z];

	if( ( (int)(fz / m_f3Scale.z) % 2 ) != 0 )
	{
		if (fzPercent >= fxPercent)									//a
			fBottomRight = fBottomLeft + (fTopRight - fTopLeft);
		else														//b
			fTopLeft = fTopRight + (fBottomLeft - fBottomRight);
	}
	else
	{
		if (fzPercent < (1.0f - fxPercent))							//d
			fTopRight = fTopLeft + (fBottomRight - fBottomLeft);
		else														//c
			fBottomLeft = fTopLeft + (fBottomRight - fTopRight);
	}


	//사각형의 네 점을 보간하여 높이(픽셀 값)를 계산한다.
	float fTopHeight = fTopLeft * (1 - fxPercent) + fTopRight * fxPercent;
	float fBottomHeight = fBottomLeft * (1 - fxPercent) + fBottomRight * fxPercent;
	float fHeight = fBottomHeight * (1 - fzPercent) + fTopHeight * fzPercent;

	return fHeight;
}

XMFLOAT3 CHeightMap::GetNormal(float x, float z)
{
	float fx = (x + g_fWorldSize / 2) / m_f3Scale.x;
	float fz = (z + g_fWorldSize / 2) / m_f3Scale.z;
	if ((fx < 0) || (fz < 0) || (fx >= m_nWidth - 1) || (fz >= m_nWidth - 1)) return(XMFLOAT3(0.0f, 1.0f, 0.0f));

	int idxX = static_cast<int>(fx);
	int idxZ = static_cast<int>(fz);

	int xHeightMapAdd = (fx < (m_nWidth - 1)) ? 1 : -1;
	int zHeightMapAdd = (fz < (m_nLength - 1)) ? 1 : -1;

	float y1 = m_pHeightMap[idxX][idxZ] * m_f3Scale.y;
	float y2 = m_pHeightMap[idxX + xHeightMapAdd][idxZ] * m_f3Scale.y;
	float y3 = m_pHeightMap[idxX][idxZ + zHeightMapAdd] * m_f3Scale.y;

	XMVECTOR vEdge1 = XMVECTOR{ 0.0f, y3 - y1, m_f3Scale.z, 0 };
	XMVECTOR vEdge2 = XMVECTOR{ m_f3Scale.x, y2 - y1, 0.0f };
	XMVECTOR vNormal;
	vNormal = XMVector3Normalize(XMVector3Cross(vEdge1, vEdge2));
	XMFLOAT3 f3Normal;
	XMStoreFloat3(&f3Normal, vNormal);
	return(f3Normal);
}
