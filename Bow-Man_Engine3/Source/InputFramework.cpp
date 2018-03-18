#include "stdafx.h"
#include "InputFramework.h"
#include "Framework.h"
#include "resource.h"
#include "ParallelPivot.h"
#include "Scene.h"
#include "D2DFramework.h"
#include "UiObject.h"
#include "UtilityFunction.h"
//현재 마우스의 위치와 마우스 기준 위치를 비교하여 실수값을 뽑아낸다.

void CInputFramework::Initialize(HWND hWnd, RECT & m_WindowRect) {
	SetHWND(hWnd);

	m_pWindowRect = &m_WindowRect;
	m_PivotMousePos.x = static_cast<int>((m_pWindowRect->right - m_pWindowRect->left) / 2);
	m_PivotMousePos.y = static_cast<int>((m_pWindowRect->bottom - m_pWindowRect->top) / 2);

	m_bKeyboardActive = true;
	m_bMouseActive = true;
	m_bRightClick = false;
	m_bLeftClick = false;
	m_bWheelClick = false;

	m_fMouseSensitivity = 50;

	m_pBmpAim = LoadTexture(_T("../../Resource/Default/aim_01.png"));

//	MouseActive(true);
}

void CInputFramework::Update(float fElapsedTime) {
	m_fXDelta = 0;
	m_fYDelta = 0;

	if (!m_bKeyboardActive) return;
	m_dwPressedKey = 0;
	UCHAR pKeyBuffer[256];
	ZeroMemory(pKeyBuffer, 256);
	if (GetKeyboardState(pKeyBuffer)) {
		if (pKeyBuffer[VK_W] & 0xF0)		m_dwPressedKey |= KEY_FOWARD;
		if (pKeyBuffer[VK_S] & 0xF0)		m_dwPressedKey |= KEY_BACKWARD;
		if (pKeyBuffer[VK_A] & 0xF0)		m_dwPressedKey |= KEY_LEFTWARD;
		if (pKeyBuffer[VK_D] & 0xF0)		m_dwPressedKey |= KEY_RIGHTWARD;
		if (pKeyBuffer[VK_E] & 0xF0)		m_dwPressedKey |= KEY_UPWARD;
		if (pKeyBuffer[VK_Q] & 0xF0)		m_dwPressedKey |= KEY_DOWNWARD;
		if (pKeyBuffer[VK_SHIFT] & 0xF0)	m_dwPressedKey |= KEY_RUN;
		if (pKeyBuffer[VK_SPACE] & 0xF0);
	}

//	GetCursorPos(&m_NowMousePos);
//	m_NowMousePos.x -= FRAME_BUFFER_POS_X;
//	m_NowMousePos.y -= FRAME_BUFFER_POS_Y;
//
//	switch (m_bMouseActive) {
//	case true:
//		if (m_bLeftClick) {
//#ifdef FOR_NOTEBOOK_ERROR
//			if (abs(m_PreMousePos.y - m_NowMousePos.y) < 2 && abs(m_PreMousePos.x - m_NowMousePos.x) < 2) break;
//#else
//			if (m_PreMousePos.x == m_NowMousePos.x && m_PreMousePos.y == m_NowMousePos.y) break;
//#endif
//			m_fXDelta = static_cast<float>(m_NowMousePos.x - m_PreMousePos.x) / m_fMouseSensitivity;
//			m_fYDelta = static_cast<float>(m_NowMousePos.y - m_PreMousePos.y) / m_fMouseSensitivity;
//			SetCursorPos(m_PreMousePos.x+FRAME_BUFFER_POS_X, m_PreMousePos.y+FRAME_BUFFER_POS_Y);
//			SetCapture(m_hWnd);
//			SetCursor(NULL);
//		}
//		break;
//	case false:
//#ifdef FOR_NOTEBOOK_ERROR
//		if (abs(m_PivotMousePos.y - m_NowMousePos.y) < 2 && abs(m_PivotMousePos.x - m_NowMousePos.x) < 2) break;
//#else
//		if (m_NowMousePos.x == m_PivotMousePos.x && m_NowMousePos.y == m_PivotMousePos.y) break;
//#endif
//		m_fXDelta = static_cast<float>(m_NowMousePos.x - (m_PivotMousePos.x)) / m_fMouseSensitivity;
//		m_fYDelta = static_cast<float>(m_NowMousePos.y - (m_PivotMousePos.y)) / m_fMouseSensitivity;
//		SetCursorPos(m_PivotMousePos.x+FRAME_BUFFER_POS_X, m_PivotMousePos.y+FRAME_BUFFER_POS_Y);
//		m_PreMousePos = m_NowMousePos;
//		break;
//	}
}

void CInputFramework::Render(ID2D1RenderTarget * pRenderTarget) {
	pRenderTarget->BeginDraw();
	if(m_bMouseActive) {
		POINT pos;
		GetCursorPos(&pos);
		XMFLOAT2 vPos(pos.x-FRAME_BUFFER_POS_X, pos.y-FRAME_BUFFER_POS_Y);
		
		if (!m_bLeftClick) {
			if (CScene::m_pickData[RenderPivot].m_pObject) {
				WCHAR wsText[50];
				swprintf_s(wsText, _T("%0x\n%+.3f, %+.3f, %+.3f"), CScene::m_pickData[RenderPivot].m_pObject, CScene::m_pickData[RenderPivot].m_vPickedPosition.x, CScene::m_pickData[RenderPivot].m_vPickedPosition.y, CScene::m_pickData[RenderPivot].m_vPickedPosition.z);
				gpD2DBrush->SetColor(D2D1_COLOR_F{ 1, 1, 1, 1 });
				pRenderTarget->DrawText(wsText, lstrlen(wsText), gpTextFormat, D2D1_RECT_F{ vPos.x, vPos.y + 30, vPos.x + 300, vPos.y + 70 }, gpD2DBrush);
			}
			else {
				if (CScene::m_pickData[RenderPivot].m_bPicked) {
					WCHAR wsText[50];
					swprintf_s(wsText, _T("%+.3f, %+.3f, %+.3f"), CScene::m_pickData[RenderPivot].m_vPickedPosition.x, CScene::m_pickData[RenderPivot].m_vPickedPosition.y, CScene::m_pickData[RenderPivot].m_vPickedPosition.z);
					gpD2DBrush->SetColor(D2D1_COLOR_F{ 1, 1, 1, 1 });
					pRenderTarget->DrawText(wsText, lstrlen(wsText), gpTextFormat, D2D1_RECT_F{ vPos.x, vPos.y + 30, vPos.x + 300, vPos.y + 50 }, gpD2DBrush);
				}
			}
		}
	}
	else {
		D2D1_SIZE_F image_size = m_pBmpAim->GetSize();
		XMFLOAT2 vPos((SCREEN_WIDTH / 2)- (image_size.width/2), (SCREEN_HEIGHT / 2)-(image_size.height/2));
		pRenderTarget->DrawBitmap(m_pBmpAim, D2D_RECT_F{ vPos.x, vPos.y, vPos.x + image_size.width, vPos.y + image_size.height }, 1);
	}
	pRenderTarget->EndDraw();
}

void CInputFramework::MouseActive(bool bFlag) {
	if (bFlag == false) {
		m_bMouseActive = false;
		SetMousePivot();
		SetCapture(m_hWnd);
		SetCursor(NULL);
		SetMouseToCenter();
	}
	else {
		m_bMouseActive = true;
		ReleaseCapture();
		SetCursor(LoadCursor(0, MAKEINTRESOURCE(IDC_CURSOR2)));
	}
}

void CInputFramework::MouseOnOff() {
	if (m_bMouseActive) {
		m_bMouseActive = false;
		SetMousePivot();
		SetCapture(m_hWnd);
		SetCursor(NULL);
		SetMouseToCenter();
	}
	else {
		m_bMouseActive = true;
		ReleaseCapture();
		SetCursor(LoadCursor(0, MAKEINTRESOURCE(IDC_CURSOR2)));
	}
}

void CInputFramework::SetMouseToCenter() {
	SetCursorPos(m_PivotMousePos.x+FRAME_BUFFER_POS_X, m_PivotMousePos.y+FRAME_BUFFER_POS_Y);
}

bool CInputFramework::MouseInputProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	m_NowMousePos = POINT{ LOWORD(lParam), HIWORD(lParam) };

	switch (message) {
	case WM_MOUSEMOVE:
		if (!m_bMouseActive) {
#ifdef FOR_NOTEBOOK_ERROR
			if (abs(m_PivotMousePos.y - m_NowMousePos.y) < 2 && abs(m_PivotMousePos.x - m_NowMousePos.x) < 2) break;
#else
			if (m_NowMousePos.x == m_PivotMousePos.x && m_NowMousePos.y == m_PivotMousePos.y) break;
#endif
			m_fXDelta = static_cast<float>(m_NowMousePos.x - (m_PivotMousePos.x)) / m_fMouseSensitivity;
			m_fYDelta = static_cast<float>(m_NowMousePos.y - (m_PivotMousePos.y)) / m_fMouseSensitivity;
			SetCursorPos(m_PivotMousePos.x+FRAME_BUFFER_POS_X, m_PivotMousePos.y+FRAME_BUFFER_POS_Y);
			
		}
		else {
			if (m_bLeftClick) {
#ifdef FOR_NOTEBOOK_ERROR
				if (abs(m_PreMousePos.y - m_NowMousePos.y) < 2 && abs(m_PreMousePos.x - m_NowMousePos.x) < 2) break;
#else
				if (m_PreMousePos.x == m_NowMousePos.x && m_PreMousePos.y == m_NowMousePos.y) break;
#endif
				m_fXDelta = static_cast<float>(m_NowMousePos.x - m_PreMousePos.x) / m_fMouseSensitivity;
				m_fYDelta = static_cast<float>(m_NowMousePos.y - m_PreMousePos.y) / m_fMouseSensitivity;
				SetCursorPos(m_PreMousePos.x+FRAME_BUFFER_POS_X, m_PreMousePos.y+FRAME_BUFFER_POS_Y);
				SetCapture(m_hWnd);
				SetCursor(NULL);
			}
		}
		break;
	case WM_LBUTTONDOWN:
		m_bLeftClick = true;
		if (m_bMouseActive) {
			m_PreMousePos = m_NowMousePos;
		}
		break;
	case WM_LBUTTONUP:
		m_bLeftClick = false;
		if (m_bMouseActive) {
			SetCapture(m_hWnd);
			//	ReleaseCapture();
			ReleaseCapture();
			SetCursor(LoadCursor(0, MAKEINTRESOURCE(IDC_CURSOR2)));
		}
		else {
		}
		break;
	case WM_LBUTTONDBLCLK:
		break;
	case WM_RBUTTONDOWN:
		m_bRightClick = true;
		break;
	case WM_RBUTTONUP:
		m_bRightClick = false;
		break;
	case WM_RBUTTONDBLCLK:
		break;
	case WM_MOUSEWHEEL:
		break;
	case WM_MBUTTONDOWN:
		m_bWheelClick = true;
		MouseOnOff();
		break;
	case WM_MBUTTONUP:
		m_bWheelClick = false;
		break;
	case WM_MBUTTONDBLCLK:
		break;
	case WM_MOUSELEAVE:
		break;
	default:
		return false;
	}
	return true;
}
