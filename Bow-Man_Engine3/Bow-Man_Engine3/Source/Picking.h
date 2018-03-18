#pragma once

//Dummy Class
class CObject;
class CModelAssembly;
class CModel;

struct PickingData {
	CObject			*m_pObject = nullptr;
	CModelAssembly	*m_pModelAssembly = nullptr;
	CModel			*m_pModel = nullptr;
	XMMATRIX		m_mtxWorld;
	float			m_fNearestDistance = FLT_MAX;
	XMFLOAT3		m_vPickedPosition = XMFLOAT3(-FLT_MAX,-FLT_MAX, -FLT_MAX);
	bool			m_bPicked = false;
};