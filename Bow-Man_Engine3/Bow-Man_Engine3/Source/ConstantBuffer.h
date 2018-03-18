#pragma once

//	D3D11_BUFFER_DESC d3dBufferDesc;
//	ZeroMemory(&d3dBufferDesc,sizeof(D3D11_BUFFER_DESC));
//
//	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
//	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//	d3dBufferDesc.ByteWidth = sizeof(ObjectMatrix);
//	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//	pDevice->CreateBuffer(&d3dBufferDesc,NULL,&m_pcbWorldMatrix);

enum CB_Slot {
	ETC,
	CAMERA,
	WORLD_MATRIX,
	LIGHT,
	MATERIAL,
	FOG,
	BONE_MATRIX,
};

struct ObjectMatrix
{
	XMFLOAT4X4 m_xmtxWorld;
//	XMFLOAT4X4 m_mtxWorldViewProjection;
};

struct DownScaleFactor
{
	UINT nWidth				= 0;
	UINT nHeight			= 0;
	UINT nTotalPixels		= 0;
	UINT nGroupSize			= 0;
	float fAdaption			= 0;
	float fBloomThreshold	= 0;
	UINT padding[2];
};

struct ToneFactor
{
	float fWhite		= 0;
	float fMiddleGrey	= 0;
	float fLumWhiteSqr	= 0;
	float fBloomScale	= 0;
};

__declspec(selectany) ID3D11Buffer *gCB_World = nullptr;
__declspec(selectany) ID3D11Buffer *gCB_Bone = nullptr;

template<class T>
void CreateConstantBuffers(ID3D11Device * pDevice, ID3D11Buffer **pcbBuffer) {
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));

	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dBufferDesc.ByteWidth = sizeof(T);
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pDevice->CreateBuffer(&d3dBufferDesc, NULL, pcbBuffer);
}

template<class T>
void UpdateConstantBuffer(ID3D11DeviceContext *pDeviceContext, ID3D11Buffer *pcbBuffer, const T& Data) {
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	pDeviceContext->Map(pcbBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
		T* pData = (T*)MappedResource.pData;
		memcpy(pData, &Data, sizeof(T));
	pDeviceContext->Unmap(pcbBuffer, 0);
}

void UpdateWorldConstant(ID3D11DeviceContext * pDeviceContext, const XMFLOAT4X4 & mtxWorld);