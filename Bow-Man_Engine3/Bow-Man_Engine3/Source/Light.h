#pragma once
#include "Object.h"
#include "ParallelPivot.h"

class CCameraObject;
class CShaderPack;

struct DirLight{
	XMFLOAT4		m_vUpperColor;
	XMFLOAT4		m_vDownColor;
	XMFLOAT4		m_vColorSpecular;
	XMFLOAT3		m_vDirection;
	float			m_fPower;

	DirLight(){
		m_vUpperColor		= XMFLOAT4(0,0,0,0);
		m_vDownColor		= XMFLOAT4(0,0,0,0);
		m_vColorSpecular	= XMFLOAT4(0,0,0,0);
		m_vDirection		= XMFLOAT3(0,-1,0);
		m_fPower			= 1;
	}
	DirLight(const XMFLOAT4& vUpperColor, const XMFLOAT4& vDownColor, const XMFLOAT4& vColorSpecular, const float& fPower){
		m_vUpperColor = vUpperColor;
		m_vDownColor = vDownColor;
		m_vColorSpecular = vColorSpecular;
		m_fPower = fPower;
	}
};

struct PointLight{
	XMFLOAT4		m_vColorSpecular;
	XMFLOAT3		m_vPosition = { 0, 0, 0 };
	float			m_fPower;
	float			m_fRange;
	float			m_padding1;
	float			m_padding2;
	float			m_padding3;

	PointLight(){}
	PointLight(const XMFLOAT4& vColorSpecular, const float& fPower, const float& fRange){
		m_vColorSpecular = vColorSpecular;
		m_fPower = fPower;
		m_fRange = fRange;
	}
};

struct SpotLight{
	XMFLOAT4		m_vColorSpecular;
	XMFLOAT3		m_vPosition;
	float			m_fPower;
	XMFLOAT3		m_vDirection;
	float			m_fRange;
	float			m_fCosOuter;			
	float			m_fSinOuter;;
	float			m_fCosAttRcp;
	float			m_padding1;

	SpotLight(){}
	SpotLight(const XMFLOAT4& vColorSpecular, const float& fPower, const float& fRange, const float& fInnerDegree, const float& fOuterDegree){
		m_vColorSpecular = vColorSpecular;
		m_vPosition = XMFLOAT3(0,0,0);
		m_vDirection = XMFLOAT3(0, 0, 1);
		m_fPower = fPower;
		m_fRange = fRange;

		float fCosInnerCone = cosf(XMConvertToRadians(fInnerDegree/2));
		float fCosOuterCone = cosf(XMConvertToRadians(fOuterDegree/2));
		float fSinOuterCone = sinf(XMConvertToRadians(fOuterDegree/2));

		m_fCosOuter = fCosOuterCone;
		m_fSinOuter = fSinOuterCone;
		m_fCosAttRcp = 1.0/(fCosInnerCone - fCosOuterCone);
	}
};

class CDirLightObject : public CObject{
private:
	DirLight		m_LightData;
//	CCameraObject	m_CameraObject;
public:
	CDirLightObject(){}
	CDirLightObject(DirLight& dirLight){
		m_LightData = dirLight;
		m_LightData.m_vDirection;
	}


	virtual void AddToRenderer(CCameraObject * pCamera);

	virtual void SetDirection(const XMFLOAT3& f3Direction) {
		CObject::SetDirection(f3Direction);
		m_LightData.m_vDirection = f3Direction;
	}
	virtual void SetDirection(float x, float y, float z) {
		SetDirection(XMFLOAT3(x,y,z));
	}

	const DirLight& GetLight(){
		return m_LightData;
	}
};

class CPointLightObject : public CObject{
private:
	PointLight		m_LightData;
public:
	CPointLightObject(){}
	CPointLightObject(PointLight& pointLight){
		m_LightData = pointLight;
	}

	virtual void CreateUI(const POINT& vMousePos);

	virtual void AddToRenderer(CCameraObject * pCamera);

	virtual void Move(const XMVECTOR& vVelocity) {
		SetPosition(GetPosition() + vVelocity);
	}
	virtual void Move(const XMFLOAT3& vVelocity) {
		Move(XMLoadFloat3(&vVelocity));
	}
	virtual void Move(float x, float y, float z) {
		Move(XMFLOAT3(x, y, z));
	}

	void SetLight(PointLight& pointLight){
		m_LightData = pointLight;
	}
	virtual void SetPosition(const XMFLOAT3& f3Position) {
		CObject::SetPosition(f3Position);
		m_LightData.m_vPosition = f3Position;
	}
	virtual void SetPosition(const XMVECTOR& vVector) {
		XMFLOAT3 vPosition;
		XMStoreFloat3(&vPosition, vVector);
		CObject::SetPosition(vPosition);
		m_LightData.m_vPosition = vPosition;
	}
	virtual void SetPosition(float x, float y, float z) {
		CObject::SetPosition(x, y, z);
		m_LightData.m_vPosition = { x, y, z };
	}

	const PointLight& GetLight(){
		return m_LightData;
	}
};

class CSpotLightObject : public CObject{
private:
	SpotLight		m_LightData;
public:
	CSpotLightObject(){}
	CSpotLightObject(SpotLight& spotLight){
		m_LightData = spotLight;
	}

	virtual void AddToRenderer(CCameraObject * pCamera);

	virtual void Rotate(AXIS axis, float fDegree) {
		CObject::Rotate(axis, fDegree);

		XMMATRIX mtxRotate;
		XMFLOAT3 d3dxvAxis;
		switch(axis){
		case AXIS::X: d3dxvAxis = XMFLOAT3(1, 0, 0); break;
		case AXIS::Y: d3dxvAxis = XMFLOAT3(0, 1, 0); break;
		case AXIS::Z: d3dxvAxis = XMFLOAT3(0, 0, 1); break;
		}
		mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&d3dxvAxis), XMConvertToRadians(fDegree));

		XMVECTOR vLook = XMVector3TransformNormal(XMLoadFloat3(&m_LightData.m_vDirection), mtxRotate);
		XMStoreFloat3(&m_LightData.m_vDirection, vLook);
	}

	virtual void SetPosition(const XMFLOAT3& f3Position) {
		CObject::SetPosition(f3Position);
		m_LightData.m_vPosition = f3Position;
	}
	virtual void SetPosition(const XMVECTOR& vVector) {
		XMFLOAT3 vPosition;
		XMStoreFloat3(&vPosition, vVector);
		CObject::SetPosition(vPosition);
		m_LightData.m_vPosition = vPosition;
	}
	virtual void SetPosition(float x, float y, float z) {
		CObject::SetPosition(x, y, z);
		m_LightData.m_vPosition = { x, y, z };
	}
	virtual void SetDirection(const XMFLOAT3& f3Direction) {
		CObject::SetDirection(f3Direction);
		m_LightData.m_vDirection = f3Direction;
	}
	virtual void SetDirection(float x, float y, float z) {
		SetDirection(XMFLOAT3(x, y, z));
	}

	const SpotLight& GetLight(){
		return m_LightData;
	}
};

const int MAX_DIR_LIGHTS =		5;
const int MAX_POINT_LIGHTS =	500;
const int MAX_SPOT_LIGHTS =		500;

#define NUM_OF_DIR			x
#define	NUM_OF_POINT		y
#define NUM_OF_SPOT			z

class CLightData{
private:

public:
	DirLight		m_arrDirLight[MAX_DIR_LIGHTS];
	PointLight		m_arrPointLight[MAX_POINT_LIGHTS];
	SpotLight		m_arrSpotLight[MAX_SPOT_LIGHTS];
	XMINT4			m_nLight;

	CLightData(){
		ZeroMemory(m_arrDirLight, sizeof(DirLight)*MAX_DIR_LIGHTS);
		ZeroMemory(m_arrPointLight, sizeof(PointLight)*MAX_POINT_LIGHTS);
		ZeroMemory(m_arrSpotLight, sizeof(SpotLight)*MAX_SPOT_LIGHTS);
		m_nLight = XMINT4(0, 0, 0, 0);
	}

	void AddLight(const DirLight& Light){	
		if((int)(m_nLight.NUM_OF_DIR) == MAX_DIR_LIGHTS) return;
		m_arrDirLight[(int)(m_nLight.NUM_OF_DIR)++] = Light;	
	}

	void AddLight(CDirLightObject* pLight){	
		if((int)(m_nLight.NUM_OF_DIR) == MAX_DIR_LIGHTS) return;
		m_arrDirLight[(int)(m_nLight.NUM_OF_DIR)++] = pLight->GetLight();	
	}

	void AddLight(const PointLight& Light){	
		if((int)(m_nLight.NUM_OF_POINT) == MAX_POINT_LIGHTS) return;
		m_arrPointLight[(int)(m_nLight.NUM_OF_POINT)++] = Light;	
	}

	void AddLight(CPointLightObject* pPointLight){	
		if((int)(m_nLight.NUM_OF_POINT) == MAX_POINT_LIGHTS) return;
		m_arrPointLight[(int)(m_nLight.NUM_OF_POINT)++] = pPointLight->GetLight();	
	}

	void AddLight(const SpotLight& Light){	
		if((int)(m_nLight.NUM_OF_SPOT) == MAX_SPOT_LIGHTS) return;
		m_arrSpotLight[(int)(m_nLight.NUM_OF_SPOT)++] = Light;	
	}

	void AddLight(CSpotLightObject* pSpotLight){	
		if((int)(m_nLight.NUM_OF_SPOT) == MAX_SPOT_LIGHTS) return;
		m_arrSpotLight[(int)(m_nLight.NUM_OF_SPOT)++] = pSpotLight->GetLight();	
	}

	void FlushLight(){		m_nLight = XMINT4(0, 0, 0, 0);		}
};

#define LightManager		CLightManager::GetInstance()
class CLightManager{
private:
	static CLightManager*	m_pInstance;
	CLightData				m_arrLightData[2];
	ID3D11Buffer*			m_pcbLight = nullptr;

	CShaderPack				*m_pDirLightShaderPack = nullptr;
	CShaderPack				*m_pPointLightShaderPack = nullptr;
	CShaderPack				*m_pSpotLightShaderPack = nullptr;
	CShaderPack				*m_pCapsuleLightShaderPack = nullptr;

private:
	CLightManager(){}
	~CLightManager(){}

public:
	static void CreateInstance();
	static CLightManager* GetInstance(){		return m_pInstance;		}

	void Initialize(ID3D11Device* pDevice);

	void AddLight(CDirLightObject* pLight){
		m_arrLightData[UpdatePivot].AddLight(pLight);
	}

	void AddLight(CPointLightObject* pLight){
		m_arrLightData[UpdatePivot].AddLight(pLight);
	}

	void AddLight(CSpotLightObject* pLight){
		m_arrLightData[UpdatePivot].AddLight(pLight);		
	}

	void UpdateConstantBuffer(ID3D11DeviceContext* pDeviceContext);

	void LightProcess(ID3D11DeviceContext* pDeviceContext);
};
__declspec(selectany) CLightManager* CLightManager::m_pInstance = nullptr;
