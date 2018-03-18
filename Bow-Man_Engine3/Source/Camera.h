#pragma once
#include "Object.h"

#define ASPECT_RATIO (float(FRAME_BUFFER_WIDTH) / float(FRAME_BUFFER_HEIGHT))
_declspec(selectany) float g_fNearPlaneDistant	= 1.0f;
_declspec(selectany) float g_fFarPlaneDistant	= 7000.0f;
_declspec(selectany) float g_fFOV				= 60.0f;

struct CameraData
{
	XMFLOAT4X4			m_mtxView;
	XMFLOAT4X4			m_mtxViewInversed;
	XMFLOAT4X4			m_mtxOrtho;
	XMFLOAT4X4			m_mtxProjection;
	XMFLOAT4X4			m_mtxVP;
	XMFLOAT4X4			m_mtxShadowVP;
	XMFLOAT4			m_vPerspectiveValue;
	XMFLOAT3			m_vCameraPosition;
	float				m_fNearPlaneDistant;
	XMFLOAT3			m_vCameraDirection;
	float				m_fFarPlaneDistant;
	XMFLOAT4			m_arrFrustum[6];
};

class CCameraObject : public CObject
{
private:

protected:
	XMFLOAT4X4			m_mtxView;         
	XMFLOAT4X4			m_f4x4Projection;  
	XMFLOAT4X4			m_mtxOrtho;  
	XMFLOAT4X4			m_mtxViewProject;

	D3D11_VIEWPORT		m_Viewport;

	//Frustum Culling�� ���� 
	XMFLOAT4			m_f4FrustumPlanes[6]; 

	//Parallel Framework Data Race�� ���ϱ� ���Ͽ� 2��
	CameraData			m_CameraData[2];

	float				m_fPitch;             
	float				m_fRoll;              
	float				m_fYaw;   

public:
	static ID3D11Buffer	*m_pcbCamera;

public:
	CCameraObject();
	~CCameraObject();

	static void CreateConstantBuffer(ID3D11Device *pDevice);
	void CreateCameraMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle);
	void CreateViewport(ID3D11DeviceContext *pDeviceContext, DWORD xTopLeft, DWORD yTopLeft, DWORD nWidth, DWORD nHeight, float fMinZ, float fMaxZ);

	virtual void CreateUI();

	void Rotate(float xfDgree, float yfDgree, float zfDgree);

	//ī�޶� �������� �������� ȣ��Ǵ� Update �Լ�
	void UpdateViewMatrix();
	void UpdateFrustumPlanes();
	void UpdateConstantBuffer(ID3D11DeviceContext *pDeviceContext);

	bool IsInRange(XMVECTOR vPosition) {
		float fDistance;
		XMStoreFloat(&fDistance, XMVector3Length(GetPosition() - vPosition));
		if (fDistance > g_fFarPlaneDistant) 
			return false;
		else
			return true;
	}

	bool IsInFrustum(const BoundingBox& pAABB){
		XMVECTOR vvFrustumPlane[6];
		for(int i=0 ; i<6 ; ++i) vvFrustumPlane[i] = XMLoadFloat4(&m_f4FrustumPlanes[i]);
		return pAABB.ContainedBy(vvFrustumPlane[0], vvFrustumPlane[1], vvFrustumPlane[2], vvFrustumPlane[3], vvFrustumPlane[4], vvFrustumPlane[5]);
	}
	bool IsInFrustum(const BoundingSphere& BoundingSphere) {
		XMVECTOR vvFrustumPlane[6];
		for(int i=0 ; i<6 ; ++i) vvFrustumPlane[i] = XMLoadFloat4(&m_f4FrustumPlanes[i]);
		return BoundingSphere.ContainedBy(vvFrustumPlane[0], vvFrustumPlane[1], vvFrustumPlane[2], vvFrustumPlane[3], vvFrustumPlane[4], vvFrustumPlane[5]);
	}

	virtual void Update(float fElapsedTime) override;

	const D3D11_VIEWPORT& GetViewport(){
		return m_Viewport;		
	}
	const XMMATRIX GetViewMatrix(){
		return XMLoadFloat4x4(&m_mtxView);
	}
	const XMMATRIX GetProjectionMatrix(){
		return XMLoadFloat4x4(&m_f4x4Projection);
	}
	const XMMATRIX GetVPMatrix(){
		return  XMLoadFloat4x4(&m_mtxViewProject);;
	}
	const XMVECTOR* GetFrustumPlane(){

	}
	const XMVECTOR GetPickDirection(int posX, int posY){
		//ȭ�� ��ǥ���� �� (xClient, yClient)�� ȭ�� ��ǥ ��ȯ�� ����ȯ�� ���� ��ȯ�� ����ȯ�� �Ѵ�. 
		//�� ����� ī�޶� ��ǥ���� ���̴�. ���� ����� ī�޶󿡼� z-������ �Ÿ��� 1�̹Ƿ� z-��ǥ�� 1�� �����Ѵ�.
		XMFLOAT3 vPickDirection;
		vPickDirection.x = +(((2.0f * posX) / m_Viewport.Width) - 1) / m_f4x4Projection._11;
		vPickDirection.y = -(((2.0f * posY) / m_Viewport.Height) - 1) / m_f4x4Projection._22;
		vPickDirection.z = 1.0f;

		XMVECTOR vResult = XMVector3TransformNormal(XMLoadFloat3(&vPickDirection), GetWorldMatrix());
		vResult = XMVector3Normalize(vResult);
		
		return vResult;
	}
};

__declspec(selectany) ID3D11Buffer *CCameraObject::m_pcbCamera = nullptr;