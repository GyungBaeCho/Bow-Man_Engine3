#pragma once
#include "Picking.h"

//Dummy Declare
class CObject;
class CCameraObject;
class CPlayerObject;
class CShaderPack;

class CScene{
private:

protected:
	CObject						*m_pSelectedObject;
	CPlayerObject				*m_pPlayer;
	std::list<CObject*>			m_lpObject;
	std::list<CShaderPack*>		m_lpShader;

public:
	static PickingData			m_pickData[2];
	static int					m_nRenderedObject;
	static int					m_nRenderedModel;
	static int					m_nHomoModel;
	static int					m_nRenderer;
	static int					m_nPointLight;
	static int					m_nSpotLight;

public:
	CScene(){
	
	}
	virtual ~CScene(){}

	void CreateConstantBuffer(ID3D11Device *pDevice){
	
	}
	void CreateScene(ID3D11Device* pDevice,ID3D11DeviceContext* pDeviceContext);
	void CreateTerrain(ID3D11Device* pDevice,ID3D11DeviceContext* pDeviceContext);

	void DestroyScene();

	void Update(ID3D11DeviceContext *pDeviceContext, float fElapsedTime);
	void PrepareRender(){}
	void Render(ID3D11DeviceContext* pDeviceContext);

	void ObjectPicking(float fElapsedTime);

	void DragDropInputProc(HWND hWnd,UINT nMessageID,WPARAM wParam,LPARAM lParam);
	bool KeyboardInputProc(HWND hWnd,UINT nMessageID,WPARAM wParam,LPARAM lParam);
	void MouseInputProc(HWND hWnd,UINT nMessageID,WPARAM wParam,LPARAM lParam);
};

_declspec(selectany) PickingData CScene::m_pickData[2];
_declspec(selectany) int CScene::m_nRenderedObject = 0;
_declspec(selectany) int CScene::m_nRenderedModel = 0;
_declspec(selectany) int CScene::m_nHomoModel = 0;
_declspec(selectany) int CScene::m_nRenderer = 0;
_declspec(selectany) int CScene::m_nPointLight = 0;
_declspec(selectany) int CScene::m_nSpotLight = 0;