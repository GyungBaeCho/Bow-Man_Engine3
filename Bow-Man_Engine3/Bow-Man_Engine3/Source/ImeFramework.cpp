#include "stdafx.h"
#include "ImeFramework.h"
#include "UtilityFunction.h"
#include "ChatDataBase.h"


int CImeTextSystem::TextProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam){
	switch(message)
	{
	case WM_IME_SETCONTEXT:
		std::cout << "WM_IME_SETCONTEXT" << std::endl;
		break;
	case WM_IME_STARTCOMPOSITION:
		m_wcText[m_nPivot] = '\0';
		m_bIme = true;
		break;
	case WM_IME_ENDCOMPOSITION:
		m_wcText[m_nPivot] = '\0';
		m_bIme = false;
		break;
	case WM_IME_CONTROL:
		std::cout << "WM_IME_CONTROL" << std::endl;
		break;
	case WM_IME_COMPOSITION:
		//배열 초과 제한
		if(MAX_LENGTH < m_nPivot+2) break;

		//조합 완료 이벤트
		if(lParam & GCS_RESULTSTR)
		{
			int len = ImmGetCompositionString(m_hIMC,GCS_RESULTSTR,NULL,0);
			ImmGetCompositionString(m_hIMC,GCS_RESULTSTR,&m_wcText[m_nPivot],len);
			m_nPivot++;
			m_wcText[m_nPivot] = '\0';
		}
		//조합 중 이벤트
		else if(lParam & GCS_COMPSTR)
		{
			int len = ImmGetCompositionString(m_hIMC,GCS_COMPSTR,NULL,0);
			ImmGetCompositionString(m_hIMC,GCS_COMPSTR,&m_wcText[m_nPivot],len);
			m_wcText[m_nPivot+1] = '\0';
		}
		break;
	case WM_IME_CHAR:
		break;
	case WM_CHAR:
		switch(wParam){
		case VK_BACK:
			if(m_bIme) break;
			if(m_nPivot < 1) break;
			m_nPivot--;
			m_wcText[m_nPivot] = '\0';
			break;
		case VK_RETURN:{
			if(m_nPivot == 0) break;
			if(!m_pChatData) break;;

			m_pChatData->AddUserMessage(m_wcText);
			m_nPivot = 0;
			m_wcText[m_nPivot] = '\0';
			break;
		}
		default:
			if(MAX_LENGTH < m_nPivot+2) break;

			m_wcText[m_nPivot] = static_cast<WCHAR>(LOWORD(wParam));
			m_nPivot++;
			m_wcText[m_nPivot] = '\0';
			break;
		}
		break;
	case WM_IME_NOTIFY:		//한자입력
		break;
	}

	return 0;
}
