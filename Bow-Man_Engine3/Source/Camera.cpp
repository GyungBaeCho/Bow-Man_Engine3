#include "stdafx.h"
#include "Camera.h"
#include "Framework.h"
#include "Shader.h"

CCameraObject::CCameraObject()
{
	m_fPitch = 0.0f;             
	m_fRoll = 0.0f;              
	m_fYaw = 0.0f;  

	m_mtxView = m_mtxIdentity;
	m_f4x4Projection = m_mtxIdentity;
}

CCameraObject::~CCameraObject(){	}

void CCameraObject::CreateConstantBuffer(ID3D11Device *pDevice){
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(CameraData);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pDevice->CreateBuffer(&bd, NULL, &m_pcbCamera);
}

void CCameraObject::CreateCameraMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle){
	XMMATRIX m_mtxViewProject;

	//���� ���� ���
	m_mtxViewProject = XMMatrixPerspectiveFovLH(XMConvertToRadians(fFOVAngle), fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
	XMStoreFloat4x4(&m_f4x4Projection, m_mtxViewProject);

	//���� ���� ���
	m_mtxViewProject = XMMatrixOrthographicLH(FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, -fFarPlaneDistance, fFarPlaneDistance);
	XMStoreFloat4x4(&m_mtxOrtho, m_mtxViewProject);
}

void CCameraObject::CreateViewport(ID3D11DeviceContext *pDeviceContext, DWORD xTopLeft, DWORD yTopLeft, DWORD nWidth, DWORD nHeight, float fMinZ, float fMaxZ){
	m_Viewport.TopLeftX = float(xTopLeft);
	m_Viewport.TopLeftY = float(yTopLeft);
	m_Viewport.Width = float(nWidth);
	m_Viewport.Height = float(nHeight);
	m_Viewport.MinDepth = fMinZ;
	m_Viewport.MaxDepth = fMaxZ;
	pDeviceContext->RSSetViewports(1, &m_Viewport);
}

void CCameraObject::CreateUI() {

}

void CCameraObject::Rotate(float xfDgree, float yfDgree, float zfDgree){
	XMMATRIX mtxRotate;
	XMFLOAT4 vAxis; 
	XMStoreFloat4(&vAxis, GetAxisY());

    if (yfDgree != 0.0f) {
		if(vAxis.y > 0){
			mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&XMFLOAT3(0,1,0)), XMConvertToRadians(yfDgree));
			SetAxisX(XMVector3TransformNormal(GetAxisX(), mtxRotate));
			SetAxisY(XMVector3TransformNormal(GetAxisY(), mtxRotate));
			SetAxisZ(XMVector3TransformNormal(GetAxisZ(), mtxRotate));
		}
		else{
			mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&XMFLOAT3(0,-1,0)), XMConvertToRadians(yfDgree));
			SetAxisX(XMVector3TransformNormal(GetAxisX(), mtxRotate));
			SetAxisY(XMVector3TransformNormal(GetAxisY(), mtxRotate));
			SetAxisZ(XMVector3TransformNormal(GetAxisZ(), mtxRotate));
		}
    }
    if (xfDgree != 0.0f) {
		mtxRotate = XMMatrixRotationAxis(GetAxisX(), XMConvertToRadians(xfDgree));
		SetAxisX(XMVector3TransformNormal(GetAxisX(), mtxRotate));
		SetAxisY(XMVector3TransformNormal(GetAxisY(), mtxRotate));
		SetAxisZ(XMVector3TransformNormal(GetAxisZ(), mtxRotate));
    } 
	UpdateViewMatrix();
}

void CCameraObject::UpdateViewMatrix(){
	//ī�޶� ��ǥ�� ��ȯ�� ����� ���.
	m_mtxView._11 = m_mtxWorld._11; 
	m_mtxView._12 = m_mtxWorld._21; 
	m_mtxView._13 = m_mtxWorld._31;
	m_mtxView._21 = m_mtxWorld._12; 
	m_mtxView._22 = m_mtxWorld._22; 
	m_mtxView._23 = m_mtxWorld._32;
	m_mtxView._31 = m_mtxWorld._13; 
	m_mtxView._32 = m_mtxWorld._23; 
	m_mtxView._33 = m_mtxWorld._33;
	XMVECTOR vVector;
	vVector = -XMVector3Dot(GetPosition(), GetAxisX());
	XMStoreFloat(&m_mtxView._41, vVector);
	vVector = -XMVector3Dot(GetPosition(), GetAxisY());
	XMStoreFloat(&m_mtxView._42, vVector);
	vVector = -XMVector3Dot(GetPosition(), GetAxisZ());
	XMStoreFloat(&m_mtxView._43, vVector);
	m_mtxView._44 = 1;

	//FrustumCulling�� ����� ������ ����
	UpdateFrustumPlanes();
}

void CCameraObject::UpdateFrustumPlanes(){
/*ī�޶� ��ȯ ��İ� ���� ���� ��ȯ ����� ���� ����� ����Ͽ� ����ü ������ ���Ѵ�. �� ���� ��ǥ�迡�� ����ü �ø��� �Ѵ�.*/
	XMMATRIX mtxView = XMLoadFloat4x4(&m_mtxView);
	XMMATRIX mtxProject = XMLoadFloat4x4(&m_f4x4Projection);
	XMMATRIX mtxViewProject = XMMatrixMultiply(mtxView, mtxProject);

	XMStoreFloat4x4(&m_mtxViewProject, mtxViewProject);

//	XMMatrixLookAtLH();

//����ü�� ���� ���
	m_f4FrustumPlanes[0].x = -(m_mtxViewProject._14 + m_mtxViewProject._11);
	m_f4FrustumPlanes[0].y = -(m_mtxViewProject._24 + m_mtxViewProject._21);
	m_f4FrustumPlanes[0].z = -(m_mtxViewProject._34 + m_mtxViewProject._31);
	m_f4FrustumPlanes[0].w = -(m_mtxViewProject._44 + m_mtxViewProject._41);

//����ü�� ������ ���
	m_f4FrustumPlanes[1].x = -(m_mtxViewProject._14 - m_mtxViewProject._11);
	m_f4FrustumPlanes[1].y = -(m_mtxViewProject._24 - m_mtxViewProject._21);
	m_f4FrustumPlanes[1].z = -(m_mtxViewProject._34 - m_mtxViewProject._31);
	m_f4FrustumPlanes[1].w = -(m_mtxViewProject._44 - m_mtxViewProject._41);

//����ü�� ���� ���
	m_f4FrustumPlanes[2].x = -(m_mtxViewProject._14 - m_mtxViewProject._12);
	m_f4FrustumPlanes[2].y = -(m_mtxViewProject._24 - m_mtxViewProject._22);
	m_f4FrustumPlanes[2].z = -(m_mtxViewProject._34 - m_mtxViewProject._32);
	m_f4FrustumPlanes[2].w = -(m_mtxViewProject._44 - m_mtxViewProject._42);

//����ü�� �Ʒ��� ���
	m_f4FrustumPlanes[3].x = -(m_mtxViewProject._14 + m_mtxViewProject._12);
	m_f4FrustumPlanes[3].y = -(m_mtxViewProject._24 + m_mtxViewProject._22);
	m_f4FrustumPlanes[3].z = -(m_mtxViewProject._34 + m_mtxViewProject._32);
	m_f4FrustumPlanes[3].w = -(m_mtxViewProject._44 + m_mtxViewProject._42);

//����ü�� �����
	m_f4FrustumPlanes[4].x = -(m_mtxViewProject._13);
	m_f4FrustumPlanes[4].y = -(m_mtxViewProject._23);
	m_f4FrustumPlanes[4].z = -(m_mtxViewProject._33);
	m_f4FrustumPlanes[4].w = -(m_mtxViewProject._43);

//����ü�� �����
	m_f4FrustumPlanes[5].x = -(m_mtxViewProject._14 - m_mtxViewProject._13);
	m_f4FrustumPlanes[5].y = -(m_mtxViewProject._24 - m_mtxViewProject._23);
	m_f4FrustumPlanes[5].z = -(m_mtxViewProject._34 - m_mtxViewProject._33);
	m_f4FrustumPlanes[5].w = -(m_mtxViewProject._44 - m_mtxViewProject._43);

/*����ü�� �� ����� ���� ���� (a, b. c)�� ũ��� a, b, c, d�� ������. ��, ���� ���͸� ����ȭ�ϰ� �������� �������� �Ÿ��� ����Ѵ�.*/
	XMVECTOR vPlane;
	for (int i = 0; i < 6; i++){
		vPlane = XMLoadFloat4(&m_f4FrustumPlanes[i]);
		vPlane = XMPlaneNormalize(vPlane);
		XMStoreFloat4(&m_f4FrustumPlanes[i], vPlane);
	}
}

void CCameraObject::UpdateConstantBuffer(ID3D11DeviceContext *pDeviceContext)
{
	::UpdateConstantBuffer(pDeviceContext, m_pcbCamera, m_CameraData[RenderPivot]);
	pDeviceContext->VSSetConstantBuffers(CAMERA, 1, &m_pcbCamera);
	pDeviceContext->HSSetConstantBuffers(CAMERA, 1, &m_pcbCamera);
	pDeviceContext->DSSetConstantBuffers(CAMERA, 1, &m_pcbCamera);
	pDeviceContext->PSSetConstantBuffers(CAMERA, 1, &m_pcbCamera);

	gpImmediateContext->RSSetViewports(1, &m_Viewport);
}

void CCameraObject::Update(float fElapsedTime){
	UpdateViewMatrix();

	XMMATRIX mtxMatrix;
	mtxMatrix = XMLoadFloat4x4(&m_mtxView);
	mtxMatrix = XMMatrixTranspose(mtxMatrix);
	XMStoreFloat4x4(&(m_CameraData[UpdatePivot].m_mtxView), mtxMatrix);

	mtxMatrix = XMLoadFloat4x4(&m_mtxWorld);
	mtxMatrix = XMMatrixTranspose(mtxMatrix);
	XMStoreFloat4x4(&(m_CameraData[UpdatePivot].m_mtxViewInversed), mtxMatrix);

	m_CameraData[UpdatePivot].m_mtxOrtho;

	XMMATRIX mtxProjection = XMLoadFloat4x4(&m_f4x4Projection);
	mtxProjection = XMMatrixTranspose(mtxProjection);
	XMStoreFloat4x4(&(m_CameraData[UpdatePivot].m_mtxProjection), mtxProjection);

	mtxMatrix = XMLoadFloat4x4(&m_mtxViewProject);
	mtxMatrix = XMMatrixTranspose(mtxMatrix);
	XMStoreFloat4x4(&(m_CameraData[UpdatePivot].m_mtxVP), mtxMatrix);
		
	m_CameraData[UpdatePivot].m_mtxShadowVP;
	m_CameraData[UpdatePivot].m_vPerspectiveValue = XMFLOAT4(1.0f/m_f4x4Projection._11, 1.0f/m_f4x4Projection._22, m_f4x4Projection._43, -m_f4x4Projection._33);
	m_CameraData[UpdatePivot].m_vCameraPosition = XMFLOAT3(m_mtxWorld._41, m_mtxWorld._42, m_mtxWorld._43);
	m_CameraData[UpdatePivot].m_fNearPlaneDistant = g_fNearPlaneDistant;
	m_CameraData[UpdatePivot].m_vCameraDirection = XMFLOAT3(m_mtxWorld._31, m_mtxWorld._32, m_mtxWorld._33);
	m_CameraData[UpdatePivot].m_fFarPlaneDistant = g_fFarPlaneDistant;
	memcpy(m_CameraData[UpdatePivot].m_arrFrustum, m_f4FrustumPlanes, sizeof(m_f4FrustumPlanes));
}