#pragma once

#define MAX_LENGTH	128

struct ChatData;

class CImeTextSystem{
private:
	HIMC		m_hIMC;
	int			m_nPivot				= 0;
	bool		m_bIme					= false;
		
	WCHAR		m_wcText[MAX_LENGTH];

	ChatData	*m_pChatData			= nullptr;

public:
	CImeTextSystem(HWND hWnd){
		m_hIMC = ImmGetContext(hWnd);
	}
	~CImeTextSystem(){}

	void Initialize(HWND hWnd){
		m_hIMC = ImmGetContext(hWnd);
	//	CreateCaret(hWnd, NULL, 2, 15);
	//	ShowCaret(hWnd);
	}
	void Destroy(HWND hWnd){
		ImmReleaseContext(hWnd, m_hIMC);
	}

	int TextProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

	void SetChatList(ChatData *pChatList){
		m_pChatData = pChatList;	
	}

	WCHAR* GetStr(){
		return m_wcText;		
	}
};