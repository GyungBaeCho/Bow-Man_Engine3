#include "stdafx.h"
#include "ObjectStrategy.h"
#include "Object.h"
#include "UtilityFunction.h"
#include "HeightMap.h"

void UpdateDefault(CObject * pThis,float fElapsedTime)
{

}

void UpdateRotate(CObject * pThis,float fElapsedTime)
{
	pThis->Rotate(AXIS::Y, 10*fElapsedTime);
}

void UpdateRotateRandom(CObject * pThis, float fElapsedTime)
{
	pThis->Rotate(AXIS::Y, RandomBetween(-0, 100) * fElapsedTime);
}

void UpdateMoveFoward(CObject * pThis,float fElapsedTime)
{
	pThis->Move(pThis->GetAxisZ() * 30 * fElapsedTime);
}

void UpdateRePosition(CObject * pThis, float fElapsedTime)
{
	XMFLOAT3 f3Position = pThis->GetF3Position();

	//XMVECTOR vLook = XMVector3Normalize(XMVector3Cross(pThis->GetAxisX(), XMLoadFloat3(&g_pHeightMap->GetNormal(f3Position.x, f3Position.z))));
	//XMFLOAT3 f3Direction;
	//XMStoreFloat3(&f3Direction, vLook);
	//pThis->SetDirection(f3Direction);

	if (f3Position.x < -(g_fWorldSize / 2) || (g_fWorldSize / 2) < f3Position.x) {
		pThis->SetPosition(XMFLOAT3(RandomBetween(-(g_fWorldSize / 2), (g_fWorldSize / 2)), RandomBetween(1, 10), RandomBetween(-(g_fWorldSize / 2), (g_fWorldSize / 2))));
		return;
	}
	if (f3Position.z < -(g_fWorldSize / 2) || (g_fWorldSize / 2) < f3Position.z) {
		pThis->SetPosition(XMFLOAT3(RandomBetween(-(g_fWorldSize / 2), (g_fWorldSize / 2)), RandomBetween(1, 10), RandomBetween(-(g_fWorldSize / 2), (g_fWorldSize / 2))));
		return;
	}

	//if (f3Position.x < 0 || 60000 < f3Position.x) {
	//	pThis->SetPosition(XMFLOAT3(RandomBetween(0, 60000), RandomBetween(1, 10), RandomBetween(0, 30000)));
	//	return;
	//}
	//if (f3Position.z < 0 || 30000 < f3Position.z) {
	//	pThis->SetPosition(XMFLOAT3(RandomBetween(0, 60000), RandomBetween(1, 10), RandomBetween(0, 30000)));
	//	return;
	//}

	float fHeight = g_pHeightMap->GetHeight(f3Position.x, f3Position.z);
	f3Position.y = fHeight + 3;
	pThis->SetPosition(f3Position);
}


