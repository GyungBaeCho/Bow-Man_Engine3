#include "stdafx.h"
//#include "ResourceData.h"
#include "Material.h"
#include "ConstantBuffer.h"

void CMaterial::CreateConstantBuffer(ID3D11Device * pDevice) {
	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	BufferDesc.ByteWidth = sizeof(XMFLOAT4);
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	pDevice->CreateBuffer(&BufferDesc, NULL, &m_pcbMaterialColors);
}

void CMaterial::SetResourceToDevice(ID3D11DeviceContext * pDeviceContext) {
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	pDeviceContext->Map(m_pcbMaterialColors, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	XMFLOAT4 *pcbMaterial = (XMFLOAT4*)MappedResource.pData;
	memcpy(pcbMaterial, &m_vDiffuseSpecExp, sizeof(XMFLOAT4));
	pDeviceContext->Unmap(m_pcbMaterialColors, 0);

	pDeviceContext->PSSetConstantBuffers(CB_Slot::MATERIAL, 1, &m_pcbMaterialColors);
	//	pDeviceContext->CSSetConstantBuffers(CB_Slot::MATERIAL, 1, &m_pcbMaterialColors);
}
