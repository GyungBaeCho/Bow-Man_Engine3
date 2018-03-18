#include "stdafx.h"
#include "Player.h"
#include "Camera.h"
#include "InputFramework.h"

#define KM_PER_HOUR_TO_M_PER_SEC(num)			num*5/18

CPlayerObject::CPlayerObject(){
	m_fPitch = 0;
	m_fYaw = 0;
	m_fRoll = 0;
	
	m_pCamera = nullptr;
	m_bFreeCamera = false;
	
	m_fWalkingSpeed = KM_PER_HOUR_TO_M_PER_SEC(30);
	m_fRunningSpeed = KM_PER_HOUR_TO_M_PER_SEC(400);
	m_vShift = XMFLOAT3(0, 0, 0);
}

CPlayerObject::~CPlayerObject() {

}

void CPlayerObject::Move(const XMVECTOR & vVelocity) {
	CObject::Move(vVelocity);
	m_pCamera->Move(vVelocity);
}

void CPlayerObject::Move(const XMFLOAT3 & vVelocity) {
	CObject::Move(vVelocity);
	m_pCamera->Move(vVelocity);
}

void CPlayerObject::Move(float x, float y, float z) {
	CObject::Move(x, y, z);
	m_pCamera->Move(x, y, z);
}

void CPlayerObject::EventInput(float fTimeElapsed){
	DWORD dwPressedKey = 0;
	static UCHAR pKeyBuffer[256];
	if (GetKeyboardState(pKeyBuffer)){
		if (pKeyBuffer[VK_W] & 0xF0)		dwPressedKey |= KEY_FOWARD;
		if (pKeyBuffer[VK_S] & 0xF0)		dwPressedKey |= KEY_BACKWARD;
		if (pKeyBuffer[VK_A] & 0xF0)		dwPressedKey |= KEY_LEFTWARD;
		if (pKeyBuffer[VK_D] & 0xF0)		dwPressedKey |= KEY_RIGHTWARD;
		if (pKeyBuffer[VK_E] & 0xF0)		dwPressedKey |= KEY_UPWARD;
		if (pKeyBuffer[VK_Q] & 0xF0)		dwPressedKey |= KEY_DOWNWARD;
		if (pKeyBuffer[VK_SHIFT] & 0xF0)	dwPressedKey |= KEY_RUN;
		if (pKeyBuffer[VK_SPACE] & 0xF0);
	}

	XMVECTOR vShift = XMLoadFloat3(&m_vShift);
	if (dwPressedKey & KEY_FOWARD)		vShift += XMLoadFloat3(&m_vMoveFoward);
	if (dwPressedKey & KEY_BACKWARD)	vShift -= XMLoadFloat3(&m_vMoveFoward);
	if (dwPressedKey & KEY_LEFTWARD)	vShift -= XMLoadFloat3(&m_vMoveRight);
	if (dwPressedKey & KEY_RIGHTWARD)	vShift += XMLoadFloat3(&m_vMoveRight);
	if (dwPressedKey & KEY_DOWNWARD)	vShift -= XMLoadFloat3(&m_vMoveUp);
	if (dwPressedKey & KEY_UPWARD)		vShift += XMLoadFloat3(&m_vMoveUp);
	vShift = XMVector3Normalize(vShift);
	if (dwPressedKey & KEY_RUN)			vShift *= m_fRunningSpeed * fTimeElapsed;
	else								vShift *= m_fWalkingSpeed * fTimeElapsed;

	XMStoreFloat3(&m_vShift, vShift);
}

void CPlayerObject::Rotate(float xfDegree, float yfDegree, float zfDegree){
	if(xfDegree==0 && yfDegree==0 && zfDegree==0) return;
	XMMATRIX mtxRotate;

	////FPS ¸ðµå
	if (xfDegree != 0.0f){
		m_fPitch += xfDegree;
		if (m_fPitch > +89.0f) { xfDegree -= (m_fPitch - 89.0f); m_fPitch = +89.0f; }
		if (m_fPitch < -89.0f) { xfDegree -= (m_fPitch + 89.0f); m_fPitch = -89.0f; }
	}
	if (yfDegree != 0.0f){
		m_fYaw += yfDegree;
		if (m_fYaw > 360.0f) m_fYaw -= 360.0f;
		if (m_fYaw < 0.0f) m_fYaw += 360.0f;
	}
	if (zfDegree != 0.0f){
		m_fRoll += zfDegree;
		if (m_fRoll > +70.0f) { zfDegree -= (m_fRoll - 70.0f); m_fRoll = +70.0f; }
		if (m_fRoll < -70.0f) { zfDegree -= (m_fRoll + 70.0f); m_fRoll = -70.0f; }
	}

	if (xfDegree != 0.0f){
		mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_vMoveRight), XMConvertToRadians(xfDegree));
	//	if(gbFPSMovement == false){
			XMStoreFloat3(&m_vMoveRight, XMVector3TransformNormal(XMLoadFloat3(&m_vMoveRight), mtxRotate));
			XMStoreFloat3(&m_vMoveUp, XMVector3TransformNormal(XMLoadFloat3(&m_vMoveUp), mtxRotate));
			XMStoreFloat3(&m_vMoveFoward, XMVector3TransformNormal(XMLoadFloat3(&m_vMoveFoward), mtxRotate));
	//	}
	}
	if (yfDegree != 0.0f){
		XMFLOAT3 f3Up;
		if (m_vMoveUp.y > 0)
			f3Up = XMFLOAT3 (0, 1, 0);
		else
			f3Up = XMFLOAT3 (0, -1, 0);
		mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&f3Up), XMConvertToRadians(yfDegree));
		XMStoreFloat3(&m_vMoveRight, XMVector3TransformNormal(XMLoadFloat3(&m_vMoveRight), mtxRotate));
		XMStoreFloat3(&m_vMoveUp, XMVector3TransformNormal(XMLoadFloat3(&m_vMoveUp), mtxRotate));
		XMStoreFloat3(&m_vMoveFoward, XMVector3TransformNormal(XMLoadFloat3(&m_vMoveFoward), mtxRotate));
	}
	m_pCamera->Rotate(xfDegree, yfDegree, zfDegree);
}

void CPlayerObject::Update(float fElapsedTime)
{
	Rotate(InputFramework->GetYDelta(), InputFramework->GetXDelta(), 0);

//	if(!InputFramework->IsMouseActive()) return;

	XMVECTOR vNewShift = XMVECTOR{0,0,0,0};
	DWORD dwPressedKey = InputFramework->GetPressedKey();
	if (dwPressedKey & KEY_FOWARD)		vNewShift += XMLoadFloat3(&m_vMoveFoward);
	if (dwPressedKey & KEY_BACKWARD)	vNewShift -= XMLoadFloat3(&m_vMoveFoward);
	if (dwPressedKey & KEY_LEFTWARD)	vNewShift -= XMLoadFloat3(&m_vMoveRight);
	if (dwPressedKey & KEY_RIGHTWARD)	vNewShift += XMLoadFloat3(&m_vMoveRight);
	if (dwPressedKey & KEY_DOWNWARD)	vNewShift -= XMLoadFloat3(&m_vMoveUp);
	if (dwPressedKey & KEY_UPWARD)		vNewShift += XMLoadFloat3(&m_vMoveUp);
//	vNewShift = XMVector3Normalize(vNewShift);
	if(dwPressedKey != 0){
		if (dwPressedKey & KEY_RUN)			vNewShift *= m_fRunningSpeed * fElapsedTime;
		else								vNewShift *= m_fWalkingSpeed * fElapsedTime;
	}

	XMVECTOR vShift;
	vShift = vNewShift;
	Move(vShift);
	m_pCamera->Update(fElapsedTime);
}

void CPlayerObject::SetPosition(const XMVECTOR & vVector) {
	CObject::SetPosition(vVector);
	m_pCamera->SetPosition(vVector);
}

void CPlayerObject::SetPosition(const XMFLOAT3 & vPosition) {
	CObject::SetPosition(vPosition);
	m_pCamera->SetPosition(vPosition);
}

void CPlayerObject::SetPosition(float x, float y, float z) {
	CObject::SetPosition(x, y, z);
	m_pCamera->SetPosition(XMFLOAT3(x, y, z));
}
