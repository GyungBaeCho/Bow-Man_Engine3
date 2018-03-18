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

	//원근 투영 행렬
	m_mtxViewProject = XMMatrixPerspectiveFovLH(XMConvertToRadians(fFOVAngle), fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
	XMStoreFloat4x4(&m_f4x4Projection, m_mtxViewProject);

	//직교 투영 행렬
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
	//카메라 좌표계 변환에 사용할 행렬.
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

	//FrustumCulling에 사용할 데이터 생성
	UpdateFrustumPlanes();
}

void CCameraObject::UpdateFrustumPlanes(){
/*카메라 변환 행렬과 원근 투영 변환 행렬을 곱한 행렬을 사용하여 절두체 평면들을 구한다. 즉 월드 좌표계에서 절두체 컬링을 한다.*/
	XMMATRIX mtxView = XMLoadFloat4x4(&m_mtxView);
	XMMATRIX mtxProject = XMLoadFloat4x4(&m_f4x4Projection);
	XMMATRIX mtxViewProject = XMMatrixMultiply(mtxView, mtxProject);

	XMStoreFloat4x4(&m_mtxViewProject, mtxViewProject);

//	XMMatrixLookAtLH();

//절두체의 왼쪽 평면
	m_f4FrustumPlanes[0].x = -(m_mtxViewProject._14 + m_mtxViewProject._11);
	m_f4FrustumPlanes[0].y = -(m_mtxViewProject._24 + m_mtxViewProject._21);
	m_f4FrustumPlanes[0].z = -(m_mtxViewProject._34 + m_mtxViewProject._31);
	m_f4FrustumPlanes[0].w = -(m_mtxViewProject._44 + m_mtxViewProject._41);

//절두체의 오른쪽 평면
	m_f4FrustumPlanes[1].x = -(m_mtxViewProject._14 - m_mtxViewProject._11);
	m_f4FrustumPlanes[1].y = -(m_mtxViewProject._24 - m_mtxViewProject._21);
	m_f4FrustumPlanes[1].z = -(m_mtxViewProject._34 - m_mtxViewProject._31);
	m_f4FrustumPlanes[1].w = -(m_mtxViewProject._44 - m_mtxViewProject._41);

//절두체의 위쪽 평면
	m_f4FrustumPlanes[2].x = -(m_mtxViewProject._14 - m_mtxViewProject._12);
	m_f4FrustumPlanes[2].y = -(m_mtxViewProject._24 - m_mtxViewProject._22);
	m_f4FrustumPlanes[2].z = -(m_mtxViewProject._34 - m_mtxViewProject._32);
	m_f4FrustumPlanes[2].w = -(m_mtxViewProject._44 - m_mtxViewProject._42);

//절두체의 아래쪽 평면
	m_f4FrustumPlanes[3].x = -(m_mtxViewProject._14 + m_mtxViewProject._12);
	m_f4FrustumPlanes[3].y = -(m_mtxViewProject._24 + m_mtxViewProject._22);
	m_f4FrustumPlanes[3].z = -(m_mtxViewProject._34 + m_mtxViewProject._32);
	m_f4FrustumPlanes[3].w = -(m_mtxViewProject._44 + m_mtxViewProject._42);

//절두체의 근평면
	m_f4FrustumPlanes[4].x = -(m_mtxViewProject._13);
	m_f4FrustumPlanes[4].y = -(m_mtxViewProject._23);
	m_f4FrustumPlanes[4].z = -(m_mtxViewProject._33);
	m_f4FrustumPlanes[4].w = -(m_mtxViewProject._43);

//절두체의 원평면
	m_f4FrustumPlanes[5].x = -(m_mtxViewProject._14 - m_mtxViewProject._13);
	m_f4FrustumPlanes[5].y = -(m_mtxViewProject._24 - m_mtxViewProject._23);
	m_f4FrustumPlanes[5].z = -(m_mtxViewProject._34 - m_mtxViewProject._33);
	m_f4FrustumPlanes[5].w = -(m_mtxViewProject._44 - m_mtxViewProject._43);

/*절두체의 각 평면의 법선 벡터 (a, b. c)의 크기로 a, b, c, d를 나눈다. 즉, 법선 벡터를 정규화하고 원점에서 평면까지의 거리를 계산한다.*/
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