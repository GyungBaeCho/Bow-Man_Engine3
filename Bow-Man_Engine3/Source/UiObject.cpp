#include "stdafx.h"
#include "UiObject.h"
#include "Framework.h"
#include "D2DFramework.h"
#include "Texture.h"
#include "Shader.h"

void CUiObject::RearrangeCoord(){
	if(m_pSuperParent == this){
		if(m_vPosition.x<0) m_vPosition.x = 0;
		if(m_vPosition.y<0) m_vPosition.y = 0;
		if(m_vPosition.x>SCREEN_WIDTH-m_vSize.x) m_vPosition.x = SCREEN_WIDTH-m_vSize.x;
		if(m_vPosition.y>SCREEN_HEIGHT-m_vSize.y) m_vPosition.y = SCREEN_HEIGHT-m_vSize.y;
	}

	XMFLOAT2 vNewPos = GetPosition();
	for(CUiObject* pChild : m_lpChild){
		pChild->FuncCoordUpdate(pChild,vNewPos);
		pChild->RearrangeCoord();
	}
}

void CUiObject::Render(ID2D1RenderTarget *pD2DRenderTarget){
	float fMinX = m_vPosition.x + 2;
	float fMinY = m_vPosition.y + 2;
	float fMaxX = m_vPosition.x+m_vSize.x - 2;
	float fMaxY = m_vPosition.y+m_vSize.y - 2;

	if(m_pSuperParent == this)
		pD2DRenderTarget->PushAxisAlignedClip(D2D1::RectF(fMinX-1,fMinY-1,fMaxX+1,fMaxY+1),D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	
	FuncRender(this, pD2DRenderTarget);
	for(CUiObject* pChild : m_lpChild) 
		pChild->Render(pD2DRenderTarget);

	if (m_pSuperParent == this) {
		pD2DRenderTarget->PopAxisAlignedClip();
		gpD2DBrush->SetColor(D2D1_COLOR_F{ 1, 1, 1, 0.3f });
		float fParentMinX = m_pSuperParent->GetPosition().x + 2;
		float fParentMinY = m_pSuperParent->GetPosition().y + 2;
		float fParentMaxX = m_pSuperParent->GetPosition().x + m_pSuperParent->GetSize().x - 2;
		float fParentMaxY = m_pSuperParent->GetPosition().y + m_pSuperParent->GetSize().y - 2;
		pD2DRenderTarget->DrawRectangle(D2D1_RECT_F{ fParentMinX, fParentMinY, fParentMaxX, fParentMaxY }, gpD2DBrush, 1);
	}
	if (m_pSelected == this) {
		gpD2DBrush->SetColor(D2D1_COLOR_F{ 1, 1, 1, 1 });
		float fParentMinX = m_pSuperParent->GetPosition().x + 2;
		float fParentMinY = m_pSuperParent->GetPosition().y + 2;
		float fParentMaxX = m_pSuperParent->GetPosition().x + m_pSuperParent->GetSize().x - 2;
		float fParentMaxY = m_pSuperParent->GetPosition().y + m_pSuperParent->GetSize().y - 2;
		pD2DRenderTarget->DrawRectangle(D2D1_RECT_F{ fParentMinX, fParentMinY, fParentMaxX, fParentMaxY }, gpD2DBrush, 1);
	}
}

void CUiObject::Move(XMFLOAT2 vPosition){
	m_vPosition.x += vPosition.x;
	m_vPosition.y += vPosition.y;

	if(m_vPosition.x<0) m_vPosition.x = 0;
	if(m_vPosition.y<0) m_vPosition.y = 0;
	if(m_vPosition.x>SCREEN_WIDTH-m_vSize.x) m_vPosition.x = SCREEN_WIDTH-m_vSize.x;
	if(m_vPosition.y>SCREEN_HEIGHT-m_vSize.y) m_vPosition.y = SCREEN_HEIGHT-m_vSize.y;
}

void CUiManager::Initialize(HWND hWnd) {
	m_pSystemChat = new CImeTextSystem(hWnd);
	m_pSystemChat->Initialize(hWnd);
	m_pSystemChat->SetChatList(&m_SystemChatData);

	CUiObject *pUiObject = CreateUi(L"System Console", UiElement::TITLE | UiElement::EXIT | UiElement::MINIMIZE | UiElement::RESIZE);
	CUiObject *pChild = new CUiObject(UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE | UiCoordType::SYNC_Y_SIZE);
	pChild->SetData(&m_SystemChatData);
	pChild->SetRenderFunc(RenderChatDataBase);
	pUiObject->AddChild(pChild);

	pChild = new CUiObject(UiCoordType::SYNC_X_SIZE | UiCoordType::DOWN_MOST);
	pChild->SetData(m_pSystemChat);
	pChild->SetRenderFunc(RenderChatter);
	pChild->SetKeyInputFunc(KeyInputChat);
	pUiObject->AddChild(pChild);
	pUiObject->SetSize(700, 300);
	pUiObject->SetPosition(0, 1000);
}

CUiObject * CUiManager::CreateUi(std::wstring wsTitle, UINT UiElement, bool bParentFlag) {
	CUiObject* pUiObject = new CUiObject(wsTitle);
	pUiObject->SetSize(200, 0);
	if (UiElement & UiElement::TITLE) {
		CUiObject *pTitle = new CUiObject;
		pTitle->SetTitle(wsTitle);
		pTitle->SetSize(0, 20);
		pTitle->m_uCoordAttri = UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE;
		pTitle->SetRenderFunc(RenderTitle);
		pTitle->SetMouseInputFunc(MouseInputMove);
		pUiObject->AddChild(pTitle);

		if (UiElement & UiElement::EXIT) {
			CUiObject *pButton = new CUiObject;
			pButton->SetSize(20, 20);
			pButton->m_uCoordAttri = UiCoordType::LEFT_WARD | RIGHT_MOST;
			pButton->SetRenderFunc(RenderExit);
			pButton->SetMouseInputFunc(MouseInputExit);
			pTitle->AddChild(pButton, false);
		}
		if (UiElement & UiElement::MINIMIZE) {
			CUiObject *pButton = new CUiObject;
			pButton->SetSize(20, 20);
			pButton->m_uCoordAttri = UiCoordType::LEFT_WARD;
			pButton->SetRenderFunc(RenderMin);
			pButton->SetMouseInputFunc(MouseInputMinimize);
			pTitle->AddChild(pButton, false);
		}
	}
	if (UiElement & UiElement::RESIZE) {
		CUiObject *pChild = new CUiObject;
		pChild->SetSize(20, 20);
		pChild->m_uCoordAttri = UiCoordType::RIGHT_MOST | UiCoordType::DOWN_MOST | UiCoordType::NOT_LINKED;
		pChild->SetMouseInputFunc(MouseInputResize);
		pChild->SetRenderFunc(RenderResize);
		pUiObject->AddChild(pChild, false);
	}
	pUiObject->SetRenderFunc(RenderSuperParent);
	pUiObject->SetPosition(XMFLOAT2(0, 0));

	if (bParentFlag) {
		if (UiElement & UiElement::ON_TOP) {
			m_lpUiObject.push_front(pUiObject);
			CUiObject::m_pSelected = pUiObject;
		}
		else
			m_lpUiObject.push_back(pUiObject);
	}
	return pUiObject;
}

void CUiManager::Render(ID2D1RenderTarget * pD2DRenderTarget) {
	pD2DRenderTarget->BeginDraw();
	std::list<CUiObject*>::reverse_iterator itrtPivot = m_lpUiObject.rbegin();
	std::list<CUiObject*>::reverse_iterator itrtEnd = m_lpUiObject.rend();
	for (; itrtPivot != itrtEnd; ++itrtPivot) 
		(*itrtPivot)->Render(pD2DRenderTarget);

	if (gpDraggedTexture && gbLMouseDown) {
		ID2D1Bitmap * pBitmap = gpDraggedTexture->GetBitmap();
		D2D1_SIZE_F vSize = pBitmap->GetSize();
		int maxSize = 400;

		float fRatioX;
		float fRatioY;
		if (vSize.width > vSize.height) {
			fRatioX = 1;
			fRatioY = vSize.height / vSize.width;
		}
		else {
			fRatioX = vSize.width / vSize.height;
			fRatioY = 1;
		}

		POINT pos;
		GetCursorPos(&pos);

		float fXPosMin = pos.x;
		float fXPosMax = pos.x + (fRatioX * maxSize);
		float fYPosMin = pos.y;
		float fYPosMax = pos.y + (fRatioY * maxSize);

		if (fXPosMax > SCREEN_WIDTH) {
			float fMove = fXPosMax - SCREEN_WIDTH;
			fXPosMin -= fMove;
			fXPosMax -= fMove;
		}
		if (fYPosMax > SCREEN_HEIGHT) {
			float fMove = fYPosMax - SCREEN_HEIGHT;
			fYPosMin -= fMove;
			fYPosMax -= fMove;
		}

		gpD2DBrush->SetColor(D2D1_COLOR_F{ 1, 1, 1, 0.4f });
		pD2DRenderTarget->FillRectangle( D2D_RECT_F{ fXPosMin, fYPosMin, fXPosMax, fYPosMax }, gpD2DBrush);

		pD2DRenderTarget->DrawBitmap(pBitmap, D2D_RECT_F{ fXPosMin, fYPosMin, fXPosMax, fYPosMax });
	}
	else if (gpDraggedShaderPack && gbLMouseDown) {

		POINT pos;
		GetCursorPos(&pos);

		gpD2DBrush->SetColor(D2D1_COLOR_F{ 1, 1, 1, 0.4f });
		pD2DRenderTarget->FillRectangle(
			D2D_RECT_F{ (float)pos.x,
				(float)pos.y,
				(float)pos.x + 300,
				(float)pos.y + 20
			},
			gpD2DBrush);
		std::wstring str = gpDraggedShaderPack->GetName();
		pD2DRenderTarget->DrawText(str.c_str(),
						str.length(),
						gpTextFormat,
						D2D1_RECT_F{
							static_cast<float>(pos.x),
							static_cast<float>(pos.y),
							static_cast<float>(pos.x)+300,
							static_cast<float>(pos.y)+20
						},
						(*gpvpD2DBrush)[_INT(BRUSH_COLOR::WHITE)],
						D2D1_DRAW_TEXT_OPTIONS_CLIP,
						DWRITE_MEASURING_MODE_NATURAL
				);
	}

	pD2DRenderTarget->EndDraw();
}

bool CUiManager::MouseInputProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_LBUTTONDOWN:
		gbLMouseDown = true;
		break;
	case WM_LBUTTONUP:
		gbLMouseDown = false;
		break;
	case WM_MOUSEMOVE:
		break;
	}

	if (CUiObject::m_pDragged)
		CUiObject::m_pDragged->MouseInputProc(hWnd, message, wParam, lParam, false);

	for (CUiObject *pUiObject : m_lpUiObject) {
		if (pUiObject->MouseInputProc(hWnd, message, wParam, lParam)) {
			//선택된 Ui 가장 앞쪽으로 이동(렌더 순서는 뒤에서 앞쪽)
			SetUiOnTop(CUiObject::m_pSelected);
			return true;
		}
	}
	switch (message) {
	case WM_LBUTTONDOWN:
		CUiObject::m_pSelected = nullptr;
		break;
	case WM_LBUTTONUP:
		CUiObject::m_pDragged = nullptr;
		gpDraggedTexture = nullptr;
		gpDraggedShaderPack = nullptr;
		break;
	case WM_MOUSEMOVE:
		CUiObject::m_pMouseOnObject = nullptr;
		break;
	}

	return false;
}

bool CUiManager::KeyInputProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_ESCAPE:
			if (RemoveUi(CUiObject::m_pSelected)) {
				return true;
			}
			break;
		}
		break;
	case WM_KEYUP:
		switch (wParam) {
		}
		break;
	}

	if (CUiObject::m_pSelected)
		CUiObject::m_pSelected->KeyInput(hWnd, message, wParam, lParam);

	return false;
}
