#pragma once
#pragma once

#define InputFramework		CInputFramework::GetInstance()
#define FOR_NOTEBOOK_ERROR

enum MOUSE_STATUS{
	LEFT,
	RIGHT,
	WHEEL,
};

class CInputFramework
{
private:
	static CInputFramework* m_pInstance;

	HWND	m_hWnd;
	POINT	m_ScreenPos;
	POINT	m_PivotMousePos;
	POINT	m_PreMousePos;
	POINT	m_NowMousePos;

	RECT	*m_pWindowRect;

	float	m_fXDelta;
	float	m_fYDelta;
			
	float	m_fMouseSensitivity;

	bool	m_bKeyboardActive;
	bool	m_bMouseActive;
	bool	m_bRightClick;
	bool	m_bLeftClick;
	bool	m_bWheelClick;

	DWORD	m_dwPressedKey;

	ID2D1Bitmap	*m_pBmpAim;

private:
	CInputFramework(){}
	~CInputFramework(){}

public:
	static void CreateInstance(){		m_pInstance = new CInputFramework;		}
	static CInputFramework* GetInstance(){		return m_pInstance;		}

	void Initialize(HWND hWnd, RECT& m_WindowRect);

	void Release(){}

	void AlterMouseSencitivity(float fAmount){
		m_fMouseSensitivity += fAmount;
		if(m_fMouseSensitivity < 1) m_fMouseSensitivity = 1;
		else if(m_fMouseSensitivity > 100) m_fMouseSensitivity = 100;
	}

	//현재 마우스의 위치와 마우스 기준 위치를 비교하여 실수값을 뽑아낸다.
	void Update(float fElapsedTime);
	void Render(ID2D1RenderTarget *pRenderTarget);

	bool IsRightClicked(){
		return m_bRightClick;		
	}
	bool IsLeftClicked(){		
		return m_bLeftClick;		
	}
	bool IsMouseActive(){
		return m_bMouseActive;		
	}
	
	void MouseActive(bool bFlag);
	void MouseOnOff();

	void SetHWND(HWND hWnd){
		m_hWnd = hWnd;
	}
	void SetMousePivot(){
		m_PivotMousePos.x = static_cast<int>((m_pWindowRect->right - m_pWindowRect->left)/2);
		m_PivotMousePos.y = static_cast<int>((m_pWindowRect->bottom - m_pWindowRect->top)/2);
	}
	void SetMouseToCenter();

	const POINT& GetMousePos() const{
		return m_NowMousePos;
	}
	DWORD GetPressedKey(){
		return m_dwPressedKey;		
	}
	float GetXDelta(){
		return m_fXDelta;		
	}
	float GetYDelta(){
		return m_fYDelta;		
	}
	bool MouseInputProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

__declspec(selectany) CInputFramework* CInputFramework::m_pInstance = nullptr;