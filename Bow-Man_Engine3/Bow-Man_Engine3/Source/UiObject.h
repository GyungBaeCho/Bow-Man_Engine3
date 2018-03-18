#pragma once

#include "UiComponent.h"
#include "ChatDataBase.h"
#include "ImeFramework.h"

/*
	--참고사항--
	새로운 기능의 UI를 위해 무분별하게 상속을 하는 대신,
	전략 패턴을 사용하여 원하는 기능을 조합할 수 있도록 구현함.

	사용되는 함수들은, 차후 Template으로 통합하는 과정을 거칠 예정.
*/

const int gElementSizeY = 20;
const int gInterval = 2;

enum UiCoordType : int{
	LEFT_WARD		= 1,
	RIGHT_WARD		= 2,
	DOWN_WARD		= 4,
	UP_WARD			= 8,
	SYNC_X_SIZE		= 16,
	SYNC_Y_SIZE		= 32,
	RIGHT_MOST		= 64,
	DOWN_MOST		= 128,
	NOT_LINKED		= 256,		//이 UI가 다음 UI의 좌표값에 연관하지 않을 때
};

enum UiElement : int{
	TITLE		= 1,
	RESIZE		= 2,
	EXIT		= 4,
	MINIMIZE	= 8,
	ON_TOP		= 16,
};

class CUiObject{
private:
	std::wstring			m_wsTitle;
	std::wstring			m_wsContent;
	void					*m_pArrData[4];
	XMFLOAT2				m_vPosition		= XMFLOAT2(0, 0);
	XMFLOAT2				m_vSize			= XMFLOAT2(20, 20);
	CUiObject				*m_pSuperParent	= this;
	CUiObject				*m_pParent		= this;
	CUiObject				**m_ppOuterInterface = nullptr;
	std::list<CUiObject*>	m_lpChild;
	int						(*FuncCoordUpdate)(CUiObject* pThis, XMFLOAT2& pvPos);
	void					(*FuncUpdate)(CUiObject* pThis);
	void					(*FuncRender)(CUiObject* pThis, ID2D1RenderTarget *pD2DRenderTarget);
	bool					(*FuncMouseInput)(CUiObject* pThis, HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
	void					(*FuncKeyInput)(CUiObject* pThis, HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
public:
	UINT					m_uCoordAttri	= 0; 
	static CUiObject*		m_pSelected;
	static CUiObject*		m_pDragged;
	static CUiObject*		m_pSelectedChild;
	static CUiObject*		m_pMouseOnObject;
	static int				m_nMinSizeX;
	static int				m_nMinSizeY;
	static int				m_nIntervalX;
	static int				m_nIntervalY;
public:
	CUiObject(){
		FuncCoordUpdate		= UpdateCoord;
		FuncUpdate			= UpdateDefault;
		FuncRender			= RenderDefault;
		FuncMouseInput		= MouseInputDefault;
		FuncKeyInput		= KeyInputDefault;
	}
	CUiObject(const std::wstring& wsTitle) : CUiObject(){	
		m_wsTitle = wsTitle;	
	}
	CUiObject(UINT uiCoordAttribute) : CUiObject(){
		m_uCoordAttri = uiCoordAttribute;
	}
	~CUiObject(){}

	void Create(){}
	void Release(){
		for(CUiObject* pChild : m_lpChild) 
			pChild->Release();
		(m_ppOuterInterface) ? (*m_ppOuterInterface) = nullptr : NULL;
		delete this;
	}

	void MouseInput(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam){
		FuncMouseInput(this, hWnd, message, wParam, lParam);
	}

	void KeyInput(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam){
		FuncKeyInput(this, hWnd, message, wParam, lParam);
	}

	void RearrangeCoord();

	void Update(){
		FuncUpdate(this);
		for(CUiObject* pChild : m_lpChild) pChild->Update();
	}
	void Render(ID2D1RenderTarget *pD2DRenderTarget);

	void Move(XMFLOAT2 vPosition);

	bool Collision(const XMFLOAT2& vPos){
		if(vPos.x<m_vPosition.x || m_vPosition.x+m_vSize.x<vPos.x) return false;
		if(vPos.y<m_vPosition.y || m_vPosition.y+m_vSize.y<vPos.y) return false;
		return true;
	}

	void AddChild(CUiObject* pChild, bool bAddSize=true){
		pChild->SetParent(this);
		pChild->SetSuperParent(m_pSuperParent);
		m_lpChild.push_back(pChild);
		if(bAddSize)
			AddSize(pChild);
		else
			RearrangeCoord();
	}
	void AddSize(CUiObject* pChild) {
		XMFLOAT2 vChildSize = pChild->GetSize();
		XMFLOAT2 vSize = GetSize();
	//	vSize.x += vChildSize.x;
		vSize.y += (vChildSize.y + gInterval);
		m_pParent->SetSize(vSize);
	}

	//Setter
	void SetTitle(const std::wstring & wsTitle){ 
		m_wsTitle = wsTitle; 
	}
	void SetContent(const std::wstring & wsContent) {
		m_wsContent = wsContent;
	}
	template<class T> void SetData(T *pData, int index = 0){
		m_pArrData[index] = static_cast<void*>(pData);	
	}
	void SetSuperParent(CUiObject* pSuperParent) {
		m_pSuperParent = pSuperParent;
		for (CUiObject* pChild : m_lpChild)
			pChild->SetSuperParent(pSuperParent);
	}
	void SetParent(CUiObject* pParent){
		m_pParent = pParent;
	}
	void SetOuterInterface(CUiObject** ppOuterInterface) {
		m_ppOuterInterface = ppOuterInterface;
	}
	void SetPosition(const XMFLOAT2 & vPosition){
		m_vPosition = vPosition;	
		RearrangeCoord();
	}
	void SetPosition(const int nPosX, const int nPosY){
		SetPosition(XMFLOAT2(nPosX, nPosY));	
	}
	void SetSize(const XMFLOAT2& vSize){	
		m_vSize = vSize;
		RearrangeCoord();
	}
	void SetSize(const int nSizeX, const int nSizeY){
		SetSize(XMFLOAT2(nSizeX, nSizeY));	
	}
	void SetSizeX(const int nSizeX) {
		m_vSize.x = nSizeX;
	}
	void SetCoordFuncUpdate(int (*pFunc)(CUiObject* pUiObject, XMFLOAT2& pvPos)) {
		FuncCoordUpdate = pFunc;
	}
	void SetUpdateFunc(void (*pFunc)(CUiObject* pUiObject)){
		FuncUpdate = pFunc;
	}
	void SetRenderFunc(void (*pFunc)(CUiObject* pUiObject, ID2D1RenderTarget *pD2DRenderTarget)){
		FuncRender = pFunc;
	}
	void SetMouseInputFunc(bool (*pFunc)(CUiObject* pUiObject, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)) {
		FuncMouseInput = pFunc;
	}
	void SetKeyInputFunc(void (*pFunc)(CUiObject* pUiObject, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)) {
		FuncKeyInput = pFunc;
	}

	//Normal Getter
	void* GetData(int index = 0){
		return m_pArrData[index];	
	}
	CUiObject* GetSuperParent(){
		return m_pSuperParent;
	}
	CUiObject* GetParent(){
		return m_pParent;	
	}
	const XMFLOAT2& GetPosition(){
		return m_vPosition;
	}
	const XMFLOAT2& GetSize(){
		return m_vSize;
	}
	const std::wstring& GetTitle(){
		return m_wsTitle;
	}
	const std::wstring& GetContent() {
		return m_wsContent;
	}

	bool MouseInputProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, bool bCollision = true) {
		if (bCollision) {
			XMFLOAT2 vMousePos(LOWORD(lParam), HIWORD(lParam));

			if (Collision(vMousePos) == false)
				return false;
		}

		for (CUiObject* pChild : m_lpChild)
			if (pChild->MouseInputProc(hWnd, message, wParam, lParam))
				return true;

		return FuncMouseInput(this, hWnd, message, wParam, lParam);
	}
};
__declspec(selectany) CUiObject* CUiObject::m_pSelected			= nullptr;
__declspec(selectany) CUiObject* CUiObject::m_pDragged			= nullptr;
__declspec(selectany) CUiObject* CUiObject::m_pSelectedChild	= nullptr;
__declspec(selectany) CUiObject* CUiObject::m_pMouseOnObject	= nullptr;

__declspec(selectany) int CUiObject::m_nMinSizeX				= 200;
__declspec(selectany) int CUiObject::m_nMinSizeY				= 20;
__declspec(selectany) int CUiObject::m_nIntervalX				= 2;
__declspec(selectany) int CUiObject::m_nIntervalY				= 2;

class CTexture;
class CShaderPack;

__declspec(selectany) bool			gbLMouseDown = false;
__declspec(selectany) XMFLOAT2		gvMousePrev;
__declspec(selectany) CTexture*		gpDraggedTexture = nullptr;
__declspec(selectany) CShaderPack*	gpDraggedShaderPack = nullptr;

#define UiManager				CUiManager::GetInstance()
#define SystemMessage(wText)	CUiManager::GetInstance()->AddSystemMessage(wText)

#define WM_MYMSG	0x1000

class CUiManager{
private:
	static CUiManager		*m_pInstance;

	std::list<CUiObject*>	m_lpUiObject;
	CUiObject*				m_pTextureDataBase;

	ChatData				m_SystemChatData;
	CImeTextSystem			*m_pSystemChat;

private:
	CUiManager(){}
	~CUiManager(){}

public:
	static void CreateInstance(){
		if(m_pInstance) return;
		m_pInstance = new CUiManager;
	}
	static CUiManager *GetInstance(){
		return m_pInstance;
	}

	void Initialize(HWND hWnd);

	void Release(){
		CUiObject::m_pSelected = nullptr;
		CUiObject::m_pSelectedChild = nullptr;
		CUiObject::m_pMouseOnObject = nullptr;
		for(CUiObject *pObject : m_lpUiObject) SAFE_RELEASE(pObject);
	}

	CUiObject* CreateUi(std::wstring wsTitle, UINT UiElement, bool bParentFlag = true);

	void AddSystemMessage(const std::wstring & sysMsg){
		m_SystemChatData.AddSystemMessage(sysMsg);
	}

	void AddUi(CUiObject* pUiObject){
		if(!pUiObject) return;
		m_lpUiObject.push_back(pUiObject);
	}

	bool RemoveUi(CUiObject* pUiObject){
		if(!pUiObject) return false;

		CUiObject *pSuperParent = pUiObject->GetSuperParent();
		m_lpUiObject.remove(pSuperParent);
		pSuperParent->Release();
		CUiObject::m_pSelected = nullptr;

		return true;
	}

	void SetUiOnTop(CUiObject* pUiObject) {
		if (pUiObject == nullptr)
			return;

		CUiObject* pSelectedSuperUi = pUiObject->GetSuperParent();
		if (pSelectedSuperUi != m_lpUiObject.front()) {
			m_lpUiObject.remove(pSelectedSuperUi);
			m_lpUiObject.push_front(pSelectedSuperUi);
		}
	}

//		static_cast<WPARAM>(CUiObject::m_pMouseOnObject);
//		SendMessage(hWnd, WM_MYMSG, NULL, NULL);

	void Update(float fElapsedTime){

	}
	void Render(ID2D1RenderTarget *pD2DRenderTarget);

	bool MouseInputProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	bool KeyInputProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

};
__declspec(selectany) CUiManager *CUiManager::m_pInstance = nullptr;