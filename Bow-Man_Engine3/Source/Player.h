#pragma once
#include "Object.h"

#define STAND				0
#define WALK				1
#define RUN					2

//Dummy Class
class CCamera;

class CPlayerObject : public CObject{
private:

protected:
	float			m_fPitch;
	float			m_fYaw;
	float			m_fRoll;
	
	CCameraObject*	m_pCamera;
	bool			m_bFreeCamera;

	float			m_fRunningSpeed;
	float			m_fWalkingSpeed;
	XMFLOAT3		m_vShift;

public:
	CPlayerObject();
	~CPlayerObject();

	void Move(const XMVECTOR& vVelocity) override;
	void Move(const XMFLOAT3& vVelocity) override;
	void Move(float x, float y, float z) override;

	void EventInput(float fTimeElapsed);
	virtual void Rotate(float xfDegree, float yfDegree, float zfDegree);
	virtual void Update(float fElapsedTime = 0);

	void SetCamera(CCameraObject* pCamera){
		m_pCamera = pCamera;		
	}

	void SetPosition(const XMVECTOR& vVector) override;
	virtual void SetPosition(const XMFLOAT3& vPosition) override;
	virtual void SetPosition(float x, float y, float z) override;

	CCameraObject* GetCamera(){
		return m_pCamera;		
	}
};
