#include "stdafx.h"
#include "ConstantBuffer.h"

void UpdateWorldConstant(ID3D11DeviceContext * pDeviceContext, const XMFLOAT4X4 & mtxWorld) {
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pDeviceContext->Map(gCB_World, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	XMFLOAT4X4 *pcbWorldMatrix = (XMFLOAT4X4 *)d3dMappedResource.pData;
	XMMATRIX mtx = XMLoadFloat4x4(&mtxWorld);
	mtx = XMMatrixTranspose(mtx);
	XMStoreFloat4x4(pcbWorldMatrix, mtx);
	pDeviceContext->Unmap(gCB_World, 0);

	pDeviceContext->VSSetConstantBuffers(WORLD_MATRIX, 1, &gCB_World);
//	pDeviceContext->DSSetConstantBuffers(WORLD_MATRIX, 1, &gCB_World);
}
