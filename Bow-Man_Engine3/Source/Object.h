#pragma once
#include "ObjectStrategy.h"

//Dummy Class
struct PickingData;
class CHeightMap;
class CCameraObject;
class CModelAssembly;
class CMesh;
class CTexture;
class CUiObject;
enum class EDpthStnclState : char;
enum class ERstrzrState : char;
enum class EBlndState : char;
enum class ESmplrState : char;

_declspec(selectany) CHeightMap *g_pHeightMap = nullptr;

enum class EObjectAttribute : char{
	//Render Attribute
	VISIBLE,
	SHADOW,
	FOWARD,
	AABB,

	//Visible Test
	VISIBLE_TEST,
	RANGE_TEST,
	FRUSTUM_TEST,

	//Collision Attribute
	COLLIDE,
	PICK,
	SELECT,
};

enum class E : char{
	ACTIVE,
	PASSIVE,
};

struct ObjectAttribute
{
	bool m_bVisible				= true;
	bool m_bShadow				= false;
	bool m_bAAABB				= false;

	bool m_bVisibleTest			= true;
	bool m_bRangeVisibleTest	= true;
	bool m_bFrustumVisibleTest	= true;

	bool m_bCollidAble			= false;
	bool m_bPickAble			= false;
	bool m_bSelectAble			= false;

	ObjectAttribute() {}

	void SetAtrribute(EObjectAttribute attribhute, bool bFlag){
		switch(attribhute){
		case EObjectAttribute::VISIBLE:
			m_bVisible = bFlag;
			break;
		case EObjectAttribute::SHADOW:
			m_bShadow = bFlag;
			break;
		case EObjectAttribute::AABB:
			m_bAAABB = bFlag;
			break;
		case EObjectAttribute::VISIBLE_TEST:
			m_bVisibleTest = bFlag;
			break;
		case EObjectAttribute::RANGE_TEST:
			m_bRangeVisibleTest = bFlag;
			break;
		case EObjectAttribute::FRUSTUM_TEST:
			m_bFrustumVisibleTest = bFlag;
			break;
		case EObjectAttribute::COLLIDE:
			m_bCollidAble = bFlag;
			break;
		case EObjectAttribute::PICK:
			m_bPickAble = bFlag;
			break;
		case EObjectAttribute::SELECT:
			m_bSelectAble = bFlag;
			break;
		}
	}
	bool GetAtrribute(EObjectAttribute attribhute){
		switch(attribhute){
		case EObjectAttribute::VISIBLE:			return m_bVisible;
		case EObjectAttribute::SHADOW:			return m_bShadow;
		case EObjectAttribute::AABB:			return m_bAAABB;

		case EObjectAttribute::VISIBLE_TEST:	return m_bVisibleTest;
		case EObjectAttribute::RANGE_TEST:		return m_bRangeVisibleTest;
		case EObjectAttribute::FRUSTUM_TEST:	return m_bFrustumVisibleTest;

		case EObjectAttribute::COLLIDE:			return m_bCollidAble;
		case EObjectAttribute::PICK:			return m_bPickAble;
		case EObjectAttribute::SELECT:			return m_bSelectAble;
		default: MessageBox(NULL, L"존재하지 않는 Attribute 삽입", L"GetAttribute", NULL);
			exit(-1);	
		}
	}
};

class CObject
{
private:

protected:
//Render
	CModelAssembly				*m_pModelAssembly;
	XMFLOAT4X4					m_mtxWorld;

	ID3D11RasterizerState		*m_pRasterizerState;
	ID3D11DepthStencilState		*m_pDepthStencilState;
	ID3D11BlendState			*m_pBlendState;

//Update
	XMFLOAT3					m_vMoveRight;
	XMFLOAT3					m_vMoveUp;
	XMFLOAT3					m_vMoveFoward;

	ObjectAttribute				m_ObjectAttribute;
	BoundingBox					m_AABB;

public:
	static XMFLOAT4X4			m_mtxIdentity;
	std::list<void (*)(CObject* pThis, float fElapsedTime)>		UpdateList;  

public:
	CObject(){
		m_mtxWorld = m_mtxIdentity;
		m_vMoveRight = XMFLOAT3(1,0,0);
		m_vMoveUp = XMFLOAT3(0,1,0);
		m_vMoveFoward = XMFLOAT3(0,0,1);
		m_pRasterizerState = nullptr;
		m_pDepthStencilState = nullptr;
		m_pBlendState = nullptr;
		m_AABB = BoundingBox(XMFLOAT3(0,0,0), XMFLOAT3(0,0,0));
	}
	virtual ~CObject(){}

	void Release(){
		delete this;
	}

	virtual void CreateUI(const POINT& vMousePos);

	virtual void Move(const XMVECTOR& vVelocity){
		SetPosition(GetPosition() + vVelocity);
	}
	virtual void Move(const XMFLOAT3& vVelocity){
		Move(XMLoadFloat3(&vVelocity));
	}
	virtual void Move(float x,float y,float z){
		Move(XMFLOAT3(x, y, z));
	}

	virtual void Rotate(const XMFLOAT3& vAxis, float fDegree);
	virtual void Rotate(AXIS axis, float fDegree);
	virtual void Rotate(float xfDgree, float yfDgree, float zfDgree);

	void OrthogonolizeWorldMatrix();

//Resource
	void SetShaderPack(){}
	void SetModelAssembly(CModelAssembly* modelAssembly);

//Framework
	virtual void Update(float fElapsedTime){
		for(auto pFunction : UpdateList)
			pFunction(this, fElapsedTime);
	}

	virtual void AddToRenderer(CCameraObject * pCamera);

	virtual void Render(ID3D11DeviceContext *pDeviceContext, CCameraObject *pCamera); 

	bool ObjectPicking(const XMVECTOR& vRayPosition,const XMVECTOR& vRayDirection,PickingData& pickData);

	void AddUpdate(void(*FuncUpdate)(CObject* pThis, float fElapsedTime)){
		UpdateList.push_back(FuncUpdate);
	}
	void SetRState(ERstrzrState RS);
	void SetRState(ID3D11RasterizerState *pRasterizerState){
		m_pRasterizerState = pRasterizerState;
	}
	void SetDSState(EDpthStnclState DSS);
	void SetDSState(ID3D11DepthStencilState *pDepthStencilState){
		m_pDepthStencilState = pDepthStencilState;
	}
	void SetBState(EBlndState BS);
	void SetBState(ID3D11BlendState *pBlendState){
		m_pBlendState = pBlendState;
	}
	virtual void SetPosition(const XMFLOAT3& vPosition);
	virtual void SetPosition(const XMVECTOR& vVector);
	virtual void SetPosition(float x, float y, float z);
	virtual void SetDirection(const XMFLOAT3& vDirection) {
		XMVECTOR vLook = GetAxisZ();
		XMVECTOR vNewDirection = XMVector3Normalize(XMLoadFloat3(&vDirection));

		if (XMVector3Equal(vNewDirection, vLook)) return;

		XMVECTOR vAxis = XMVector3Cross(vLook, vNewDirection);

		XMFLOAT3 f3Dot;
		XMStoreFloat3(&f3Dot, XMVector3Dot(vLook, vNewDirection));
		float fRadian = XMScalarACos(f3Dot.x);

		XMFLOAT3 f3Axis2;
		XMStoreFloat3(&f3Axis2, vAxis);
		fRadian = XMConvertToDegrees(fRadian);
		Rotate(f3Axis2, fRadian);
	}
	virtual void SetDirection(float x, float y, float z) {
		SetDirection(XMFLOAT3{x,y,z});
	}
	void SetAxisX(const XMVECTOR& vVector){
		XMFLOAT3 vAxis;
		XMStoreFloat3(&vAxis, vVector);
		SetAxisX(vAxis);
	}
	void SetAxisX(XMFLOAT3 vVector){
		m_mtxWorld._11 = vVector.x, m_mtxWorld._12 = vVector.y, m_mtxWorld._13 = vVector.z;
	}
	void SetAxisY(const XMVECTOR& vVector){
		XMFLOAT3 vAxis;
		XMStoreFloat3(&vAxis, vVector);
		SetAxisY(vAxis);
	}
	void SetAxisY(XMFLOAT3 vVector){
		m_mtxWorld._21 = vVector.x, m_mtxWorld._22 = vVector.y, m_mtxWorld._23 = vVector.z;
	}
	void SetAxisZ(const XMVECTOR& vVector){
		XMFLOAT3 vAxis;
		XMStoreFloat3(&vAxis, vVector);
		SetAxisZ(vAxis);
	}
	void SetAxisZ(XMFLOAT3 vVector){
		m_mtxWorld._31 = vVector.x, m_mtxWorld._32 = vVector.y, m_mtxWorld._33 = vVector.z;
	}
	void SetObjectAttribute(const ObjectAttribute& objAttribute){
		m_ObjectAttribute = objAttribute;
	}
	void SetObjectAttribute(EObjectAttribute objAtrribute, bool bFlag){
		m_ObjectAttribute.SetAtrribute(objAtrribute, bFlag);
	}

	const XMFLOAT3* GetRightRef(){
		return &m_vMoveRight;
	}
	XMVECTOR GetRight(){
		return XMLoadFloat3(&m_vMoveRight);
	}
	const XMFLOAT3* GetUpRef(){
		return &m_vMoveUp;
	}
	XMVECTOR GetUp(){
		return XMLoadFloat3(&m_vMoveUp);
	}
	const XMFLOAT3* GetFowardRef(){
		return &m_vMoveFoward;
	}
	XMVECTOR GetFoward(){
		return XMLoadFloat3(&m_vMoveFoward);
	}
	XMVECTOR GetAxisX(){
		return XMLoadFloat3(&XMFLOAT3(m_mtxWorld._11,m_mtxWorld._12,m_mtxWorld._13));
	}
	XMVECTOR GetAxisY(){
		return XMLoadFloat3(&XMFLOAT3(m_mtxWorld._21,m_mtxWorld._22,m_mtxWorld._23));
	}
	XMVECTOR GetAxisZ(){
		return XMLoadFloat3(&XMFLOAT3(m_mtxWorld._31,m_mtxWorld._32,m_mtxWorld._33));
	}
	XMFLOAT3 GetF3Position(){
		return XMFLOAT3(m_mtxWorld._41, m_mtxWorld._42, m_mtxWorld._43);
	}
	XMVECTOR GetPosition(){	
		return XMLoadFloat3(&XMFLOAT3(m_mtxWorld._41, m_mtxWorld._42, m_mtxWorld._43));	
	}
	const XMFLOAT4X4* GetWorldMatrixRef(){		
		return &m_mtxWorld;		
	}
	XMMATRIX GetWorldMatrix(){		
		return XMLoadFloat4x4(&m_mtxWorld);		
	}
	bool GetObjectAttribute(EObjectAttribute objAttribute){
		return m_ObjectAttribute.GetAtrribute(objAttribute);
	}
	CModelAssembly* GetModel() {
		return m_pModelAssembly;
	}
};

__declspec(selectany) XMFLOAT4X4 CObject::m_mtxIdentity;