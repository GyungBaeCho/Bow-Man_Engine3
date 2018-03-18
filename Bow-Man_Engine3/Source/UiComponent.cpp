#include "stdafx.h"
#include "Framework.h"
#include "D2DFramework.h"

#include "UiComponent.h"
#include "UiObject.h"
#include "Object.h"
#include "ResourceData.h"

#include "Model.h"
#include "RenderState.h"
#include "Texture.h"
#include "Material.h"
#include "Shader.h"

#include "ModelAssembly.h"
#include "Renderer.h"
#include "Timer.h"

//MouseInput---------------------------------------------------------------------------------------
bool MouseInputDefault(CUiObject * pThis,HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam){
	XMFLOAT2 vMousePos(LOWORD(lParam), HIWORD(lParam));

	switch (message) {
	case WM_MOUSEMOVE:
		CUiObject::m_pMouseOnObject = pThis;
		return true;
	case WM_RBUTTONDOWN:
		return true;
	case WM_RBUTTONUP:
		break;
	case WM_LBUTTONDOWN:
		CUiObject::m_pSelected = pThis;
		return true;
	case WM_LBUTTONUP:
		return true;
	}
	return false;
}

bool MouseInputMove(CUiObject * pThis,HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam){
	XMFLOAT2 vMousePos(LOWORD(lParam), HIWORD(lParam));
	XMFLOAT2 vUiPos = pThis->GetPosition();
	static XMFLOAT2 vMousePivot;

	switch(message){
	case WM_MOUSEMOVE:
		CUiObject::m_pMouseOnObject = pThis;
		if(CUiObject::m_pDragged){
			if (CUiObject::m_pDragged == pThis) {
				pThis->GetSuperParent()->SetPosition(vMousePos.x - vMousePivot.x, vMousePos.y - vMousePivot.y);
				pThis->GetSuperParent()->RearrangeCoord();
			}
		}
		return true;
	case WM_RBUTTONDOWN:
		return true;
	case WM_RBUTTONUP:
		break;
	case WM_LBUTTONDOWN:
		vMousePivot = XMFLOAT2(vMousePos.x - vUiPos.x, vMousePos.y - vUiPos.y);
		CUiObject::m_pSelected = pThis;
		CUiObject::m_pDragged = pThis;
		return true;
	case WM_LBUTTONUP:
		CUiObject::m_pDragged = nullptr;
		return true;
	}
	return false;
}

bool MouseInputResize(CUiObject * pThis,HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam){
	XMFLOAT2 vMousePos(LOWORD(lParam), HIWORD(lParam));
	static XMFLOAT2 vMousePivot;

	switch(message){
	case WM_MOUSEMOVE:
		CUiObject::m_pMouseOnObject = pThis;
		if(CUiObject::m_pDragged){
			if (CUiObject::m_pDragged == pThis) {
				XMFLOAT2 vParentPos = pThis->GetSuperParent()->GetPosition();
				float fSizeX = vMousePos.x + vMousePivot.x - vParentPos.x;
				float fSizeY = vMousePos.y + vMousePivot.y - vParentPos.y;
				if (fSizeX < CUiObject::m_nMinSizeX) fSizeX = CUiObject::m_nMinSizeX;
				if (fSizeY < CUiObject::m_nMinSizeY) fSizeY = CUiObject::m_nMinSizeY;
				if (vParentPos.x + fSizeX > SCREEN_WIDTH) fSizeX = SCREEN_WIDTH - vParentPos.x;
				if (vParentPos.y + fSizeY > SCREEN_HEIGHT) fSizeY = SCREEN_HEIGHT - vParentPos.y;
				pThis->GetSuperParent()->SetSize(fSizeX, fSizeY);
			}
		}
		return true;
	case WM_RBUTTONDOWN:
		return true;
	case WM_RBUTTONUP:
		break;
	case WM_LBUTTONDOWN:
		XMFLOAT2 vParentEndPos = pThis->GetSuperParent()->GetPosition();
		XMFLOAT2 vParentSize = pThis->GetSuperParent()->GetSize();
		vParentEndPos.x += vParentSize.x;
		vParentEndPos.y += vParentSize.y;
		vMousePivot = XMFLOAT2(vParentEndPos.x - vMousePos.x, vParentEndPos.y - vMousePos.y);
		CUiObject::m_pSelected = pThis;
		CUiObject::m_pDragged = pThis;
		return true;
	case WM_LBUTTONUP:
		CUiObject::m_pDragged = nullptr;
		return true;
	}
	return false;
} 

bool MouseInputExit(CUiObject * pThis,HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam){
	XMFLOAT2 vMousePos(LOWORD(lParam), HIWORD(lParam));
	
	switch(message){
	case WM_MOUSEMOVE:
		CUiObject::m_pMouseOnObject = pThis;
		return true;
	case WM_RBUTTONDOWN:
		return true;
	case WM_RBUTTONUP:
		break;
	case WM_LBUTTONDOWN:
		return true;
	case WM_LBUTTONUP:
		if (CUiObject::m_pDragged == nullptr) {
			CUiObject::m_pSelected = nullptr;
			CUiObject::m_pMouseOnObject = nullptr;
			if (pThis->Collision(vMousePos))
				UiManager->RemoveUi(pThis);
		}
		return true;
	}
	return false;
}

bool MouseInputMinimize(CUiObject * pThis,HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam){
	switch(message){
	case WM_MOUSEMOVE:
		CUiObject::m_pMouseOnObject = pThis;
		return true;
	case WM_RBUTTONDOWN:
		return true;
	case WM_RBUTTONUP:
		break;
	case WM_LBUTTONDOWN:
		CUiObject::m_pSelected = pThis;
		return true;
	case WM_LBUTTONUP:
		if (CUiObject::m_pDragged == false) {
			SystemMessage(L"Ui Minimize 미구현");
		//	pThis->GetSuperParent()->SetSize(CUiObject::m_nMinSizeX, CUiObject::m_nMinSizeY);
		}
		return true;
	}
	return false;
}

bool MouseInputWebPage(CUiObject * pThis, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_MOUSEMOVE:
		CUiObject::m_pMouseOnObject = pThis;
		return true;
	case WM_RBUTTONDOWN:
		return true;
	case WM_RBUTTONUP:
		break;
	case WM_LBUTTONDOWN:
		CUiObject::m_pSelected = pThis;
		return true;
	case WM_LBUTTONUP:
		if (CUiObject::m_pDragged == false) {
			std::wstring* pwWebPageAddress = static_cast<std::wstring*>(pThis->GetData());
			ShellExecute(0, L"open", L"iexplore", pwWebPageAddress->c_str(), NULL, SW_SHOW);
		}
		return true;
	}
	return false;
}

bool MouseInputTexture(CUiObject * pThis, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	XMFLOAT2 vMousePos(LOWORD(lParam), HIWORD(lParam));

	switch (message) {
	case WM_MOUSEMOVE:
		CUiObject::m_pMouseOnObject = pThis;
		return true;
	case WM_RBUTTONDOWN:
		return true;
	case WM_RBUTTONUP:
		break;
	case WM_LBUTTONDOWN:
		CUiObject::m_pSelected = pThis;
		CUiObject::m_pDragged = pThis;
		if (!gpDraggedTexture) {
			if (gbLMouseDown) {
				gpDraggedTexture = static_cast<CTexture*>(pThis->GetData());
			}
		}
		return true;
	case WM_LBUTTONUP:
		if (pThis->Collision(vMousePos)) {
			CUiObject::m_pDragged = nullptr;
			gpDraggedTexture = nullptr;
		}
		return true;
	}
	return false;
}

bool MouseInputTextureAddress(CUiObject * pThis, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	CTexture ** ppTexture = static_cast<CTexture**>(pThis->GetData());
	switch (message) {
	case WM_MOUSEMOVE:
		CUiObject::m_pMouseOnObject = pThis;
		return true;
	case WM_RBUTTONDOWN:
		return true;
	case WM_RBUTTONUP:
		return true;
	case WM_LBUTTONDOWN:
		return true;
	case WM_LBUTTONUP:
		CUiObject::m_pSelected = pThis->GetSuperParent();
		if (CUiObject::m_pDragged) {
			if (gpDraggedTexture) {
				*ppTexture = gpDraggedTexture;
				CUiObject::m_pDragged = nullptr;
				gpDraggedTexture = nullptr;
			}
		}
		return true;
	}
	return false;
}

bool MouseInputShader(CUiObject * pThis, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	XMFLOAT2 vMousePos(LOWORD(lParam), HIWORD(lParam));

	switch (message) {
	case WM_MOUSEMOVE:
		CUiObject::m_pMouseOnObject = pThis;
		return true;
	case WM_RBUTTONDOWN:
		return true;
	case WM_RBUTTONUP:
		break;
	case WM_LBUTTONDOWN:
		CUiObject::m_pSelected = pThis;
		CUiObject::m_pDragged = pThis;
		if (!gpDraggedShaderPack) {
			if (gbLMouseDown) {
				CShaderPack ** ppShaderpack = static_cast<CShaderPack**>(pThis->GetData());
				gpDraggedShaderPack = *ppShaderpack;
			}
		}
		return true;
	case WM_LBUTTONUP:
		if (pThis->Collision(vMousePos)) {
			CUiObject::m_pDragged = nullptr;
			gpDraggedShaderPack = nullptr;
		}
		return true;
	}
	return false;
}

bool MouseInputShaderAddress(CUiObject * pThis, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_MOUSEMOVE:
		CUiObject::m_pMouseOnObject = pThis;
		return true;
	case WM_RBUTTONDOWN:
		return true;
	case WM_RBUTTONUP:
		return true;
	case WM_LBUTTONDOWN:
		return true;
	case WM_LBUTTONUP:
		CUiObject::m_pSelected = pThis->GetSuperParent();
		if (CUiObject::m_pDragged) {
			CShaderPack ** ppShaderpack = static_cast<CShaderPack**>(pThis->GetData());
			CModel* pModel = static_cast<CModel*>(pThis->GetData(1));
			if (gpDraggedShaderPack) {
				if (pModel->GetShaderPackID() != gpDraggedShaderPack->GetID()) {
					pModel->SetShaderPack(gpDraggedShaderPack);
					*ppShaderpack = gpDraggedShaderPack;
					pModel->GetModelAssembly()->CreateModelHomo();
				}

				CUiObject::m_pDragged = nullptr;
				gpDraggedShaderPack = nullptr;
			}
		}
		return true;
	}
	return false;
}

bool MouseInputModel(CUiObject * pThis, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	POINT vMousePos = POINT{ LOWORD(lParam), HIWORD(lParam) };
	CModel* pModel = static_cast<CModel*>(pThis->GetData());

	switch (message) {
	case WM_MOUSEMOVE:
		CUiObject::m_pMouseOnObject = pThis;
		return true;
	case WM_RBUTTONDOWN:
		return true;
	case WM_RBUTTONUP:
		break;
	case WM_LBUTTONUP:
		if (CUiObject::m_pDragged) {
			CUiObject::m_pDragged = nullptr;
			return true;
		}

		pModel->CreateUI(vMousePos);
		return true;
	}
	return false;
}

bool MouseInputDpthStncl(CUiObject * pThis, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CModel* pModel = static_cast<CModel*>(pThis->GetData());
	EDpthStnclState *pDpthStncl = pModel->GetDpthStncl();
	int idx = _INT(*pDpthStncl);

	switch (message) {
	case WM_MOUSEMOVE:
		CUiObject::m_pMouseOnObject = pThis;
		return true;
	case WM_RBUTTONDOWN:
		return true;
	case WM_RBUTTONUP:
		idx--;
		if (idx == -1) idx = gsDpthStnclState.size()-1;
		*pDpthStncl = static_cast<EDpthStnclState>(idx);
		pThis->SetContent(gsDpthStnclState[static_cast<EDpthStnclState>(idx)]);
		pModel->GetModelAssembly()->CreateModelHomo();
		return true;
	case WM_LBUTTONUP:
		idx++;
		if (idx == gsDpthStnclState.size()) idx = 0;
		*pDpthStncl = static_cast<EDpthStnclState>(idx);
		pThis->SetContent(gsDpthStnclState[static_cast<EDpthStnclState>(idx)]);
		pModel->GetModelAssembly()->CreateModelHomo();
		return true;
	}
	return false;
}

bool MouseInputRstrzr(CUiObject * pThis, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CModel* pModel = static_cast<CModel*>(pThis->GetData());
	ERstrzrState *pERstrzr = pModel->GetRstrzr();
	int idx = _INT(*pERstrzr);

	switch (message) {
	case WM_MOUSEMOVE:
		CUiObject::m_pMouseOnObject = pThis;
		return true;
	case WM_RBUTTONUP:
		idx--;
		if (idx == -1) idx = gsRstrzrState.size()-1;
		*pERstrzr = static_cast<ERstrzrState>(idx);
		pThis->SetContent(gsRstrzrState[static_cast<ERstrzrState>(idx)]);
		pModel->GetModelAssembly()->CreateModelHomo();
		return true;
	case WM_LBUTTONUP:
		idx++;
		if (idx == gsRstrzrState.size()) idx = 0;
		*pERstrzr = static_cast<ERstrzrState>(idx);
		pThis->SetContent(gsRstrzrState[static_cast<ERstrzrState>(idx)]);
		pModel->GetModelAssembly()->CreateModelHomo();
		return true;
	}
	return false;
}

bool MouseInputBlnd(CUiObject * pThis, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CModel* pModel = static_cast<CModel*>(pThis->GetData());
	EBlndState *pEBlnd = pModel->GetBlnd();
	int idx = _INT(*pEBlnd);

	switch (message) {
	case WM_MOUSEMOVE:
		CUiObject::m_pMouseOnObject = pThis;
		return true;
	case WM_RBUTTONUP:
		idx--;
		if (idx == -1) idx = gsBlndState.size()-1;
		*pEBlnd = static_cast<EBlndState>(idx);
		pThis->SetContent(gsBlndState[static_cast<EBlndState>(idx)]);
		pModel->GetModelAssembly()->CreateModelHomo();
		return true;
	case WM_LBUTTONUP:
		idx++;
		if (idx == gsBlndState.size()) idx = 0;
		*pEBlnd = static_cast<EBlndState>(idx);
		pThis->SetContent(gsBlndState[static_cast<EBlndState>(idx)]);
		pModel->GetModelAssembly()->CreateModelHomo();
		return true;
	}
	return false;
}

bool MouseInputBool(CUiObject * pThis, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	bool* pbFlag = static_cast<bool*>(pThis->GetData());

	switch (message) {
	case WM_MOUSEMOVE:
		CUiObject::m_pMouseOnObject = pThis;
		return true;
	case WM_RBUTTONUP:
		return true;
	case WM_LBUTTONUP:
		if (*pbFlag)
			*pbFlag = false;
		else
			*pbFlag = true;

		return true;
	}
	return false;
}


//KeyInput---------------------------------------------------------------------------------------
void KeyInputDefault(CUiObject * pThis,HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{

}

void KeyInputChat(CUiObject * pThis,HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	CImeTextSystem* pChat = static_cast<CImeTextSystem*>(pThis->GetData());
	pChat->TextProc(hWnd, message, wParam, lParam);
}

//Update---------------------------------------------------------------------------------------
void UpdateDefault(CUiObject * pUiThis)
{

}

//UpdateCoord---------------------------------------------------------------------------------------
int UpdateCoord(CUiObject * pUiThis,XMFLOAT2 & pvNextPos)
{
	CUiObject* pParent = pUiThis->GetParent();
	XMFLOAT2 vInputPos = pvNextPos;

	if(pUiThis->m_uCoordAttri & UiCoordType::SYNC_X_SIZE)
		pUiThis->SetSize(pParent->GetSize().x, pUiThis->GetSize().y);
	
	if(pUiThis->m_uCoordAttri & UiCoordType::SYNC_Y_SIZE){
		XMFLOAT2 vParentPos = pParent->GetPosition();
		int ySize = pParent->GetSize().y - (vInputPos.y - vParentPos.y) - 22;
		pUiThis->SetSize(pParent->GetSize().x, ySize);
	}
	
	if(pUiThis->m_uCoordAttri & UiCoordType::RIGHT_MOST){
		vInputPos = XMFLOAT2(pvNextPos.x+pParent->GetSize().x-pUiThis->GetSize().x, pvNextPos.y);

		if(pUiThis->m_uCoordAttri & UiCoordType::NOT_LINKED){

		}
		else{
			pvNextPos = vInputPos;
		}
	}
	
	if(pUiThis->m_uCoordAttri & UiCoordType::DOWN_MOST){
		XMFLOAT2 vParentPos = pParent->GetPosition();
		int ySize = pParent->GetSize().y - (vInputPos.y - vParentPos.y);

		vInputPos = XMFLOAT2(vInputPos.x, vInputPos.y+ySize-pUiThis->GetSize().y);

		if(pUiThis->m_uCoordAttri & UiCoordType::NOT_LINKED){

		}
		else{
		}
	}

	if(pUiThis->m_uCoordAttri & UiCoordType::LEFT_WARD)
		pvNextPos.x -= pUiThis->GetSize().x + gInterval;
	else if(pUiThis->m_uCoordAttri & UiCoordType::DOWN_WARD)
		pvNextPos.y += pUiThis->GetSize().y + gInterval;

	
	pUiThis->SetPosition(vInputPos);

	return UiCoordType::DOWN_WARD;
}

//Render---------------------------------------------------------------------------------------
void RenderDefault(CUiObject * pUiThis,ID2D1RenderTarget *pD2DRenderTarget)
{
	XMFLOAT2 vPosition = pUiThis->GetPosition();
	const XMFLOAT2 &vSize = pUiThis->GetSize();

	ID2D1SolidColorBrush* pBrush = nullptr;
	if(pUiThis == CUiObject::m_pMouseOnObject)
		pBrush = (*gpvpD2DBrush)[_INT(BRUSH_COLOR::WHITE_A50)];
	else
		pBrush = (*gpvpD2DBrush)[_INT(BRUSH_COLOR::WHITE_A25)];

	pD2DRenderTarget->FillRectangle(D2D1_RECT_F{vPosition.x,
												 vPosition.y, 
												 vPosition.x+vSize.x, 
												 vPosition.y+vSize.y
											},
											pBrush
										);
}

void RenderSuperParent(CUiObject * pUiThis,ID2D1RenderTarget *pD2DRenderTarget)
{
	XMFLOAT2 vPosition = pUiThis->GetPosition();
	const XMFLOAT2 &vSize = pUiThis->GetSize();

	gpD2DBrush->SetColor(D2D1_COLOR_F{0,0,0,0.8f});
	pD2DRenderTarget->FillRectangle(D2D1_RECT_F{vPosition.x,
												 vPosition.y, 
												 vPosition.x+vSize.x, 
												 vPosition.y+vSize.y
											},
											gpD2DBrush
										);
}

void RenderElement(CUiObject * pUiThis,ID2D1RenderTarget *pD2DRenderTarget)
{
	XMFLOAT2 vPosition = pUiThis->GetPosition();
	const XMFLOAT2 &vSize = pUiThis->GetSize();

	if (pUiThis == CUiObject::m_pMouseOnObject)
		gpD2DBrush->SetColor(D2D1_COLOR_F{ 1, 1, 1, 0.3f });
	else
		gpD2DBrush->SetColor(D2D1_COLOR_F{ 1, 1, 1, 0.1f });

	pD2DRenderTarget->FillRectangle(D2D1_RECT_F{vPosition.x,
												 vPosition.y, 
												 vPosition.x+vSize.x, 
												 vPosition.y+vSize.y
											},
											gpD2DBrush
										);

	std::wstring wsResult;
	const std::wstring &wsTitle = pUiThis->GetTitle();
	const std::wstring &wsContent = pUiThis->GetContent();
	wsResult = wsTitle + _T(" : ") + wsContent;

	pD2DRenderTarget->DrawText(wsResult.c_str(),
							wsResult.size(),
							gpTextFormat,
							D2D1_RECT_F{
								vPosition.x+gInterval,
								vPosition.y+gInterval, 
								vPosition.x+1000,
								vPosition.y+vSize.y-gInterval
							},
							(*gpvpD2DBrush)[_INT(BRUSH_COLOR::WHITE)],
							D2D1_DRAW_TEXT_OPTIONS_CLIP,
							DWRITE_MEASURING_MODE_NATURAL
					);
}

void RenderTexture(CUiObject * pUiThis, ID2D1RenderTarget * pD2DRenderTarget)
{
	CTexture* pTexture = static_cast<CTexture*>(pUiThis->GetData());
	ID2D1Bitmap *pBitmap = pTexture->GetBitmap();

	XMFLOAT2 vPosition = pUiThis->GetPosition();
	const XMFLOAT2 &vSize = pUiThis->GetSize();

	ID2D1SolidColorBrush* pBrush = nullptr;
	if (pUiThis == CUiObject::m_pMouseOnObject)
		pBrush = (*gpvpD2DBrush)[_INT(BRUSH_COLOR::WHITE_A50)];
	else
		pBrush = (*gpvpD2DBrush)[_INT(BRUSH_COLOR::WHITE_A25)];

	pD2DRenderTarget->FillRectangle(D2D1_RECT_F{ vPosition.x,
			vPosition.y,
			vPosition.x + vSize.x,
			vPosition.y + vSize.y
		},
		pBrush
	);

	D2D1_SIZE_F image_size = pBitmap->GetSize();
	float fImageRatioX = (image_size.width / image_size.height);
	float fImageRatioY = 1;

	XMFLOAT2 vNewPosition;
	XMFLOAT2 vNewSize;

	vNewSize.x = vSize.y * fImageRatioX;
	vNewSize.y = vSize.y * fImageRatioY;

	vNewPosition.x = vPosition.x + (vSize.x - vNewSize.x)/2;
	vNewPosition.y = vPosition.y;

	// 완전 불투명 상태로 지정된 좌표에 비트맵을 출력한다.
	pD2DRenderTarget->DrawBitmap(pBitmap, D2D_RECT_F{vNewPosition.x+gInterval, 
													 vNewPosition.y + gInterval, 
													 vNewPosition.x + vNewSize.x-gInterval, 
													 vNewPosition.y + vNewSize.y-gInterval 
													});

	gpD2DBrush->SetColor(D2D1_COLOR_F{ 1.0f, 1.0f, 1.0f, 0.5f });
	pD2DRenderTarget->DrawRectangle(D2D1_RECT_F{ vNewPosition.x + gInterval,
												vNewPosition.y + gInterval,
												vNewPosition.x + vNewSize.x - gInterval,
												vNewPosition.y + vNewSize.y - gInterval
											},
											gpD2DBrush
	);

	gpD2DBrush->SetColor(D2D1_COLOR_F{ 0.0f, 0.0f, 0.0f, 0.4f });
	pD2DRenderTarget->FillRectangle(D2D1_RECT_F{ vPosition.x + gInterval,
												vPosition.y + gInterval,
												vPosition.x + vSize.x - gInterval,
												vPosition.y + 20 - gInterval
											},  
											gpD2DBrush
	);

	const std::wstring &wsTitle = pUiThis->GetTitle();
	pD2DRenderTarget->DrawText(wsTitle.c_str(),
			wsTitle.size(),
			gpTextFormat,
			D2D1_RECT_F{
			vPosition.x + gInterval,
			vPosition.y + gInterval,
			vPosition.x + vSize.x,
			vPosition.y + vSize.y - gInterval
		},
		(*gpvpD2DBrush)[_INT(BRUSH_COLOR::WHITE)],
		D2D1_DRAW_TEXT_OPTIONS_CLIP,
		DWRITE_MEASURING_MODE_NATURAL
	);
}

void RenderTextureAddress(CUiObject * pUiThis, ID2D1RenderTarget * pD2DRenderTarget)
{
	CTexture** pTexture = static_cast<CTexture**>(pUiThis->GetData());
	ID2D1Bitmap *pBitmap = (*pTexture)->GetBitmap();

	XMFLOAT2 vPosition = pUiThis->GetPosition();
	const XMFLOAT2 &vSize = pUiThis->GetSize();

	ID2D1SolidColorBrush* pBrush = nullptr;
	if (pUiThis == CUiObject::m_pMouseOnObject)
		pBrush = (*gpvpD2DBrush)[_INT(BRUSH_COLOR::WHITE_A50)];
	else
		pBrush = (*gpvpD2DBrush)[_INT(BRUSH_COLOR::WHITE_A25)];

	pD2DRenderTarget->FillRectangle(D2D1_RECT_F{ vPosition.x,
		vPosition.y,
		vPosition.x + vSize.x,
		vPosition.y + vSize.y
	},
		pBrush
	);

	if(pBitmap){
		D2D1_SIZE_F image_size = pBitmap->GetSize();
		float fImageRatioX = (image_size.width / image_size.height);
		float fImageRatioY = 1;

		XMFLOAT2 vNewPosition;
		XMFLOAT2 vNewSize;

		vNewSize.x = vSize.y * fImageRatioX;
		vNewSize.y = vSize.y * fImageRatioY;

		vNewPosition.x = vPosition.x + (vSize.x - vNewSize.x) / 2;
		vNewPosition.y = vPosition.y;

		// 완전 불투명 상태로 지정된 좌표에 비트맵을 출력한다.
		pD2DRenderTarget->DrawBitmap(pBitmap, D2D_RECT_F{ vNewPosition.x + gInterval,
			vNewPosition.y + gInterval,
			vNewPosition.x + vNewSize.x - gInterval,
			vNewPosition.y + vNewSize.y - gInterval
		});

		gpD2DBrush->SetColor(D2D1_COLOR_F{ 0.0f, 0.0f, 0.0f, 0.4f });
		pD2DRenderTarget->FillRectangle(D2D1_RECT_F{ vPosition.x + gInterval,
			vPosition.y + gInterval,
			vPosition.x + vSize.x - gInterval,
			vPosition.y + 20 - gInterval
		},
			gpD2DBrush
		);

		gpD2DBrush->SetColor(D2D1_COLOR_F{ 1.0f, 1.0f, 1.0f, 0.5f });
		pD2DRenderTarget->DrawRectangle(D2D1_RECT_F{ vNewPosition.x + gInterval,
			vNewPosition.y + gInterval,
			vNewPosition.x + vNewSize.x - gInterval,
			vNewPosition.y + vNewSize.y - gInterval
		},
			gpD2DBrush
		);

		std::wstring wsData = pUiThis->GetTitle() + _T("-") + (*pTexture)->GetName();
		pD2DRenderTarget->DrawText(wsData.c_str(),
			wsData.size(),
			gpTextFormat,
			D2D1_RECT_F{
			vPosition.x + gInterval,
			vPosition.y + gInterval,
			vPosition.x + 1000,
			vPosition.y + vSize.y - gInterval
		},
			(*gpvpD2DBrush)[_INT(BRUSH_COLOR::WHITE)],
			D2D1_DRAW_TEXT_OPTIONS_CLIP,
			DWRITE_MEASURING_MODE_NATURAL
		);
	}
}

void RenderMaterialAddress(CUiObject * pUiThis, ID2D1RenderTarget * pD2DRenderTarget)
{
	CMaterial** pMaterial = static_cast<CMaterial**>(pUiThis->GetData());

	XMFLOAT2 vPosition = pUiThis->GetPosition();
	const XMFLOAT2 &vSize = pUiThis->GetSize();

	ID2D1SolidColorBrush* pBrush = nullptr;
	if (pUiThis == CUiObject::m_pMouseOnObject)
		pBrush = (*gpvpD2DBrush)[_INT(BRUSH_COLOR::WHITE_A50)];
	else
		pBrush = (*gpvpD2DBrush)[_INT(BRUSH_COLOR::WHITE_A25)];

	pD2DRenderTarget->FillRectangle(D2D1_RECT_F{ vPosition.x,
		vPosition.y,
		vPosition.x + vSize.x,
		vPosition.y + vSize.y
	},
		pBrush
	);

	float fMinX = vPosition.x + (vSize.x/2) - (vSize.y/2);
	float fMinY = vPosition.y + gInterval;
	float fMaxX = fMinX + (vSize.y);
	float fMaxY = vPosition.y + vSize.y - gInterval;

	XMFLOAT4 f4Color = (*pMaterial)->GetMaterialColor();
	gpD2DBrush->SetColor(D2D1_COLOR_F{f4Color.x, f4Color.y, f4Color.z, 1});
	pD2DRenderTarget->FillRectangle(D2D1_RECT_F{ fMinX, fMinY, fMaxX, fMaxY }, gpD2DBrush);

	gpD2DBrush->SetColor(D2D1_COLOR_F{ 1.0f, 1.0f, 1.0f, 0.5f });
	pD2DRenderTarget->DrawRectangle(D2D1_RECT_F{ fMinX, fMinY, fMaxX, fMaxY },
		gpD2DBrush
	);

	gpD2DBrush->SetColor(D2D1_COLOR_F{ 0.0f, 0.0f, 0.0f, 0.4f });
	pD2DRenderTarget->FillRectangle(D2D1_RECT_F{ vPosition.x + gInterval,
		vPosition.y + gInterval,
		vPosition.x + vSize.x - gInterval,
		vPosition.y + 20 - gInterval
	},
		gpD2DBrush
	);

//	std::wstring wsData = pUiThis->GetTitle() + _T("-") + (*pMaterial)->GetName() + _T("\nzzzz\nzzzz");
	std::wstring wsData = pUiThis->GetTitle() + _T("-") + (*pMaterial)->GetName();
	pD2DRenderTarget->DrawText(wsData.c_str(),
		wsData.size(),
		gpTextFormat,
		D2D1_RECT_F{
		vPosition.x + gInterval,
		vPosition.y + gInterval,
		vPosition.x + 1000,
		vPosition.y + vSize.y - gInterval
	},
		(*gpvpD2DBrush)[_INT(BRUSH_COLOR::WHITE)],
		D2D1_DRAW_TEXT_OPTIONS_CLIP,
		DWRITE_MEASURING_MODE_NATURAL
	);
}

void RenderShader(CUiObject * pUiThis, ID2D1RenderTarget * pD2DRenderTarget)
{
	CShaderPack* pShaderPack = static_cast<CShaderPack*>(pUiThis->GetData());
	XMFLOAT2 vPosition = pUiThis->GetPosition();
	const XMFLOAT2 &vSize = pUiThis->GetSize();

	ID2D1SolidColorBrush* pBrush = nullptr;
	if (pUiThis == CUiObject::m_pMouseOnObject)
		pBrush = (*gpvpD2DBrush)[_INT(BRUSH_COLOR::WHITE_A50)];
	else
		pBrush = (*gpvpD2DBrush)[_INT(BRUSH_COLOR::WHITE_A25)];

	pD2DRenderTarget->FillRectangle(D2D1_RECT_F{ vPosition.x,
		vPosition.y,
		vPosition.x + vSize.x,
		vPosition.y + vSize.y
	},
		pBrush
	);

	std::wstring wsData = pUiThis->GetTitle() + _T("-") + pShaderPack->GetName();
	pD2DRenderTarget->DrawText(wsData.c_str(),
		wsData.size(),
		gpTextFormat,
		D2D1_RECT_F{
		vPosition.x + gInterval,
		vPosition.y + gInterval,
		vPosition.x + 1000,
		vPosition.y + vSize.y - gInterval
	},
		(*gpvpD2DBrush)[_INT(BRUSH_COLOR::WHITE)],
		D2D1_DRAW_TEXT_OPTIONS_CLIP,
		DWRITE_MEASURING_MODE_NATURAL
	);
}

void RenderShaderAddress(CUiObject * pUiThis, ID2D1RenderTarget * pD2DRenderTarget)
{
	CShaderPack** pShaderPack = static_cast<CShaderPack**>(pUiThis->GetData());
	XMFLOAT2 vPosition = pUiThis->GetPosition();
	const XMFLOAT2 &vSize = pUiThis->GetSize();

	if (pUiThis == CUiObject::m_pMouseOnObject) {
		switch ((*pShaderPack)->GetPixelShaderType()) {
		case EPixelShaderType::NONE:
			gpD2DBrush->SetColor(D2D1_COLOR_F{ 1, 1, 1, 0.5f });
			break;
		case EPixelShaderType::FOWARD:
			gpD2DBrush->SetColor(D2D1_COLOR_F{ 1, 0, 1, 0.5f });
			break;
		case EPixelShaderType::DEFERRED:
			gpD2DBrush->SetColor(D2D1_COLOR_F{ 1, 1, 0, 0.5f });
			break;
		case EPixelShaderType::BLEND:
			gpD2DBrush->SetColor(D2D1_COLOR_F{ 1, 1, 1, 0.5f });
			break;
		case EPixelShaderType::SKYBOX:
			gpD2DBrush->SetColor(D2D1_COLOR_F{ 0, 1, 0, 0.5f });
			break;
		case EPixelShaderType::SHADOW:
			gpD2DBrush->SetColor(D2D1_COLOR_F{ 0, 1, 1, 0.5f });
			break;
		}
	}
	else {
		switch ((*pShaderPack)->GetPixelShaderType()) {
		case EPixelShaderType::NONE:
			gpD2DBrush->SetColor(D2D1_COLOR_F{ 1, 1, 1, 0.25f });
			break;
		case EPixelShaderType::FOWARD:
			gpD2DBrush->SetColor(D2D1_COLOR_F{ 1, 0, 1, 0.25f });
			break;
		case EPixelShaderType::DEFERRED:
			gpD2DBrush->SetColor(D2D1_COLOR_F{ 1, 1, 0, 0.25f });
			break;
		case EPixelShaderType::BLEND:
			gpD2DBrush->SetColor(D2D1_COLOR_F{ 1, 1, 1, 0.25f });
			break;
		case EPixelShaderType::SKYBOX:
			gpD2DBrush->SetColor(D2D1_COLOR_F{ 0, 1, 0, 0.25f });
			break;
		case EPixelShaderType::SHADOW:
			gpD2DBrush->SetColor(D2D1_COLOR_F{ 0, 1, 1, 0.25f });
			break;
		}
	}

	pD2DRenderTarget->FillRectangle(D2D1_RECT_F{ vPosition.x,
			vPosition.y,
			vPosition.x + vSize.x,
			vPosition.y + vSize.y
		},
		gpD2DBrush
	);

	std::wstring wsData = pUiThis->GetTitle() + _T("-") + (*pShaderPack)->GetName();
	pD2DRenderTarget->DrawText(wsData.c_str(),
		wsData.size(),
		gpTextFormat,
		D2D1_RECT_F{
		vPosition.x + gInterval,
		vPosition.y + gInterval,
		vPosition.x + 1000,
		vPosition.y + vSize.y - gInterval
	},
		(*gpvpD2DBrush)[_INT(BRUSH_COLOR::WHITE)],
		D2D1_DRAW_TEXT_OPTIONS_CLIP,
		DWRITE_MEASURING_MODE_NATURAL
	);
}

void RenderChatDataBase(CUiObject * pUiThis,ID2D1RenderTarget *pD2DRenderTarget)
{
	ChatData* pChat = static_cast<ChatData*>(pUiThis->GetData());
	XMFLOAT2 vPosition = pUiThis->GetPosition();
	const XMFLOAT2 &vSize = pUiThis->GetSize();

	float fMinX = vPosition.x + gInterval;
	float fMinY = vPosition.y + gInterval;
	float fMaxX = vPosition.x+vSize.x - gInterval;
	float fMaxY = vPosition.y+vSize.y - gInterval;

//	gpD2DBrush->SetColor(D2D1_COLOR_F{1,1,1,0.05f});
//	pD2DRenderTarget->FillRectangle(D2D1_RECT_F{fMinX,fMinY,fMaxX, fMaxY}, gpD2DBrush);

	int nPrintLine = static_cast<int>(vSize.y/gElementSizeY);
	int pivot = pChat->m_vChat.size() - nPrintLine;
	if(pivot < 0) pivot = 0;

	for(int i=pivot ; i<pChat->m_vChat.size() ; ++i){
		switch(pChat->m_vChatType[i]){
		case ChatType::SYSTEM:
			gpD2DBrush->SetColor(D2D1_COLOR_F{1, 0.1f, 0.1f, 1});
			break;
		case ChatType::USER:
			gpD2DBrush->SetColor(D2D1_COLOR_F{1, 1, 1, 1});
			break;

		}
		pD2DRenderTarget->DrawText(pChat->m_vChat[i].c_str(),
						pChat->m_vChat[i].size(),
						gpTextFormat,
						D2D1_RECT_F{
							vPosition.x+gInterval,
							vPosition.y+gInterval, 
							vPosition.x+vSize.x,
							vPosition.y+gElementSizeY-gInterval
						},
					//	(*gpvpD2DBrush)[TINT(BRUSH_COLOR::WHITE)],
						gpD2DBrush,
						D2D1_DRAW_TEXT_OPTIONS_CLIP,
						DWRITE_MEASURING_MODE_NATURAL
				);

		vPosition.y += gElementSizeY;
	}
}

void RenderChatter(CUiObject * pUiThis,ID2D1RenderTarget *pD2DRenderTarget)
{
	CImeTextSystem* pChat = static_cast<CImeTextSystem*>(pUiThis->GetData());

	XMFLOAT2 vPosition = pUiThis->GetPosition();
	const XMFLOAT2 &vSize = pUiThis->GetSize();

	float fMinX = vPosition.x + gInterval;
	float fMinY = vPosition.y + gInterval;
	float fMaxX = vPosition.x+vSize.x - gInterval;
	float fMaxY = vPosition.y+vSize.y - gInterval;

	if (pUiThis == CUiObject::m_pMouseOnObject)
		gpD2DBrush->SetColor(D2D1_COLOR_F{1,1,1,0.3f});
	else
		gpD2DBrush->SetColor(D2D1_COLOR_F{1,1,1,0.1f});

	pD2DRenderTarget->FillRectangle(D2D1_RECT_F{fMinX,fMinY,fMaxX, fMaxY}, gpD2DBrush);
	pD2DRenderTarget->DrawText(pChat->GetStr(),
							lstrlen(pChat->GetStr()),
							gpTextFormat,
							D2D1_RECT_F{
								vPosition.x+gInterval,
								vPosition.y+gInterval, 
								vPosition.x+vSize.x,
								vPosition.y+vSize.y-gInterval
							},
							(*gpvpD2DBrush)[_INT(BRUSH_COLOR::WHITE)],
							D2D1_DRAW_TEXT_OPTIONS_CLIP,
							DWRITE_MEASURING_MODE_NATURAL
					);
}

void RenderWchar(CUiObject * pUiThis,ID2D1RenderTarget *pD2DRenderTarget)
{
	WCHAR* pString = static_cast<WCHAR*>(pUiThis->GetData());
	XMFLOAT2 vPosition = pUiThis->GetPosition();
	const XMFLOAT2 &vSize = pUiThis->GetSize();

	float fMinX = vPosition.x + gInterval;
	float fMinY = vPosition.y + gInterval;
	float fMaxX = vPosition.x+vSize.x - gInterval;
	float fMaxY = vPosition.y+vSize.y - gInterval;

	if (pUiThis == CUiObject::m_pMouseOnObject)
		gpD2DBrush->SetColor(D2D1_COLOR_F{1,1,1,0.3f});
	else
		gpD2DBrush->SetColor(D2D1_COLOR_F{1,1,1,0.1f});

	//String Element의 Title 렌더도 필요함.

	pD2DRenderTarget->FillRectangle(D2D1_RECT_F{fMinX,fMinY,fMaxX, fMaxY}, gpD2DBrush);
	pD2DRenderTarget->DrawText(pString,
							lstrlen(pString),
							gpTextFormat,
							D2D1_RECT_F{
								vPosition.x+gInterval,
								vPosition.y+gInterval, 
								vPosition.x+vSize.x,
								vPosition.y+vSize.y-gInterval
							},
							(*gpvpD2DBrush)[_INT(BRUSH_COLOR::WHITE)],
							D2D1_DRAW_TEXT_OPTIONS_CLIP,
							DWRITE_MEASURING_MODE_NATURAL
					);
}

void RenderBoolElement(CUiObject * pUiThis, ID2D1RenderTarget * pD2DRenderTarget)
{
	bool* pbFlbg = static_cast<bool*>(pUiThis->GetData());
	XMFLOAT2 vPosition = pUiThis->GetPosition();
	const XMFLOAT2 &vSize = pUiThis->GetSize();

	float fMinX = vPosition.x + gInterval;
	float fMinY = vPosition.y + gInterval;
	float fMaxX = vPosition.x + vSize.x - gInterval;
	float fMaxY = vPosition.y + vSize.y - gInterval;

	if (pUiThis == CUiObject::m_pMouseOnObject)
		gpD2DBrush->SetColor(D2D1_COLOR_F{ 1, 1, 1, 0.3f });
	else
		gpD2DBrush->SetColor(D2D1_COLOR_F{ 1, 1, 1, 0.1f });

	const std::wstring &wsTitle = pUiThis->GetTitle();
//	const std::wstring &wsContent = pUiThis->GetContent();

	pD2DRenderTarget->FillRectangle(D2D1_RECT_F{ fMinX, fMinY, fMaxX, fMaxY }, gpD2DBrush);
	pD2DRenderTarget->DrawText(wsTitle.c_str(),
		wsTitle.size(),
		gpTextFormat,
		D2D1_RECT_F{
		fMinX,
		fMinY,
		fMaxX,
		fMaxY
	},
		(*gpvpD2DBrush)[_INT(BRUSH_COLOR::WHITE)],
		D2D1_DRAW_TEXT_OPTIONS_CLIP,
		DWRITE_MEASURING_MODE_NATURAL
	);

	gpD2DBrush->SetColor(D2D1_COLOR_F{1,1,1,1});
	pD2DRenderTarget->DrawRectangle(D2D1_RECT_F{ fMaxX-14, fMinY+gInterval, fMaxX-gInterval, fMaxY-gInterval }, gpD2DBrush, 1);

	if (*pbFlbg) {
		gpD2DBrush->SetColor(D2D1_COLOR_F{ 1, 1, 1, 1 });
		pD2DRenderTarget->FillRectangle(D2D1_RECT_F{ fMaxX - 12, fMinY + 4, fMaxX - 4, fMaxY - 4 }, gpD2DBrush);
	}
}

void RenderFPS(CUiObject * pUiThis,ID2D1RenderTarget *pD2DRenderTarget)
{
	CTimer* pTimer = static_cast<CTimer*>(pUiThis->GetData());
	XMFLOAT2 vPosition = pUiThis->GetPosition();
	const XMFLOAT2 &vSize = pUiThis->GetSize();
	
#ifdef _DEBUG
	int nFPSMax = 500;
#else
	int nFPSMax = 1000;
#endif
	float fPercentage = static_cast<float>(pTimer->GetAvgFPS()) / nFPSMax;

	float fMinX = vPosition.x + gInterval;
	float fMinY = vPosition.y + gInterval;
	float fMaxX = vPosition.x + (vSize.x * fPercentage) - gInterval;
	float fMaxY = vPosition.y+vSize.y - gInterval;

	if (pUiThis == CUiObject::m_pMouseOnObject){
		gpD2DBrush->SetColor(D2D1_COLOR_F{1-fPercentage,fPercentage,0,1});
	}
	else{
		gpD2DBrush->SetColor(D2D1_COLOR_F{1-fPercentage,fPercentage,0,0.8f});
	}

	pD2DRenderTarget->FillRectangle(D2D1_RECT_F{fMinX,fMinY,fMaxX, fMaxY}, gpD2DBrush);
	pD2DRenderTarget->DrawText(pTimer->GetText(),
							lstrlen(pTimer->GetText()),
							gpTextFormat,
							D2D1_RECT_F{
								vPosition.x+gInterval,
								vPosition.y+gInterval, 
								vPosition.x+vSize.x,
								vPosition.y+vSize.y-gInterval
							},
							(*gpvpD2DBrush)[_INT(BRUSH_COLOR::WHITE)],
							D2D1_DRAW_TEXT_OPTIONS_CLIP,
							DWRITE_MEASURING_MODE_NATURAL
					);
}

void RenderStopWatch(CUiObject * pUiThis, ID2D1RenderTarget * pD2DRenderTarget)
{
	CStopWatch* pStopWatch = static_cast<CStopWatch*>(pUiThis->GetData());
	XMFLOAT2 vPosition = pUiThis->GetPosition();
	const XMFLOAT2 &vSize = pUiThis->GetSize();
	
	float fElapsedTime = pStopWatch->GetAvgElapsedTime();
	float fPercentage = fElapsedTime / (1.0/70);
	float fMinX = vPosition.x + gInterval;
	float fMinY = vPosition.y + gInterval;
	float fMaxX = vPosition.x + (vSize.x * fPercentage);
	float fMaxY = vPosition.y+vSize.y - gInterval;

	if (pUiThis == CUiObject::m_pMouseOnObject){
		gpD2DBrush->SetColor(D2D1_COLOR_F{fPercentage,1-fPercentage,0,1});
	}
	else{
		gpD2DBrush->SetColor(D2D1_COLOR_F{fPercentage,1-fPercentage,0,0.8f});
	}

	pD2DRenderTarget->FillRectangle(D2D1_RECT_F{fMinX,fMinY,fMaxX, fMaxY}, gpD2DBrush);
	pD2DRenderTarget->DrawText(pStopWatch->GetText(),
							lstrlen(pStopWatch->GetText()),
							gpTextFormat,
							D2D1_RECT_F{
								vPosition.x+gInterval,
								vPosition.y+gInterval, 
								vPosition.x+vSize.x,
								vPosition.y+vSize.y-gInterval
							},
							(*gpvpD2DBrush)[_INT(BRUSH_COLOR::WHITE)],
							D2D1_DRAW_TEXT_OPTIONS_CLIP,
							DWRITE_MEASURING_MODE_NATURAL
					);
}

void RenderInt(CUiObject * pUiThis,ID2D1RenderTarget *pD2DRenderTarget)
{
	int* pInt = static_cast<int*>(pUiThis->GetData());
	WCHAR wChar[30];
	swprintf_s(wChar, L"%s%04d", pUiThis->GetTitle().c_str(), *pInt);

	XMFLOAT2 vPosition = pUiThis->GetPosition();
	const XMFLOAT2 &vSize = pUiThis->GetSize();

	float fMinX = vPosition.x + gInterval;
	float fMinY = vPosition.y + gInterval;
	float fMaxX = vPosition.x+vSize.x - gInterval;
	float fMaxY = vPosition.y+vSize.y - gInterval;

	if (pUiThis == CUiObject::m_pMouseOnObject)
		gpD2DBrush->SetColor(D2D1_COLOR_F{1,1,1,0.3f});
	else
		gpD2DBrush->SetColor(D2D1_COLOR_F{1,1,1,0.1f});

	//String Element의 Title 렌더도 필요함.

	pD2DRenderTarget->FillRectangle(D2D1_RECT_F{fMinX,fMinY,fMaxX, fMaxY}, gpD2DBrush);
	pD2DRenderTarget->DrawText(wChar,
							lstrlen(wChar),
							gpTextFormat,
							D2D1_RECT_F{
								vPosition.x+gInterval,
								vPosition.y+gInterval, 
								vPosition.x+vSize.x,
								vPosition.y+vSize.y-gInterval
							},
							(*gpvpD2DBrush)[_INT(BRUSH_COLOR::WHITE)],
							D2D1_DRAW_TEXT_OPTIONS_CLIP,
							DWRITE_MEASURING_MODE_NATURAL
					);
}

void RenderFloat(CUiObject * pUiThis, ID2D1RenderTarget * pD2DRenderTarget)
{
	float* pFloat = static_cast<float*>(pUiThis->GetData());
	WCHAR wChar[30];
	swprintf_s(wChar, L"%s%+07.2f", pUiThis->GetTitle().c_str(), *pFloat);

	XMFLOAT2 vPosition = pUiThis->GetPosition();
	const XMFLOAT2 &vSize = pUiThis->GetSize();

	float fMinX = vPosition.x + gInterval;
	float fMinY = vPosition.y + gInterval;
	float fMaxX = vPosition.x + vSize.x - gInterval;
	float fMaxY = vPosition.y + vSize.y - gInterval;

	if (pUiThis == CUiObject::m_pMouseOnObject)
		gpD2DBrush->SetColor(D2D1_COLOR_F{ 1, 1, 1, 0.3f });
	else
		gpD2DBrush->SetColor(D2D1_COLOR_F{ 1, 1, 1, 0.1f });

	pD2DRenderTarget->FillRectangle(D2D1_RECT_F{ fMinX, fMinY, fMaxX, fMaxY }, gpD2DBrush);
	pD2DRenderTarget->DrawText(wChar,
		lstrlen(wChar),
		gpTextFormat,
		D2D1_RECT_F{
		vPosition.x + gInterval,
		vPosition.y + gInterval,
		vPosition.x + vSize.x,
		vPosition.y + vSize.y - gInterval
	},
		(*gpvpD2DBrush)[_INT(BRUSH_COLOR::WHITE)],
		D2D1_DRAW_TEXT_OPTIONS_CLIP,
		DWRITE_MEASURING_MODE_NATURAL
	);
}

void RenderWstring(CUiObject * pUiThis, ID2D1RenderTarget * pD2DRenderTarget)
{
	const std::wstring *wString = static_cast<std::wstring*>(pUiThis->GetData());
	XMFLOAT2 vPosition = pUiThis->GetPosition();
	const XMFLOAT2 &vSize = pUiThis->GetSize();

	float fMinX = vPosition.x + gInterval;
	float fMinY = vPosition.y + gInterval;
	float fMaxX = vPosition.x + vSize.x - gInterval;
	float fMaxY = vPosition.y + vSize.y - gInterval;

	if (pUiThis == CUiObject::m_pMouseOnObject)
		gpD2DBrush->SetColor(D2D1_COLOR_F{ 1, 1, 1, 0.3f });
	else
		gpD2DBrush->SetColor(D2D1_COLOR_F{ 1, 1, 1, 0.1f });


	pD2DRenderTarget->FillRectangle(D2D1_RECT_F{ fMinX, fMinY, fMaxX, fMaxY }, gpD2DBrush);
	pD2DRenderTarget->DrawText(wString->c_str(),
		wString->size(),
		gpTextFormat,
		D2D1_RECT_F{
		vPosition.x + gInterval,
		vPosition.y + gInterval,
		vPosition.x + vSize.x,
		vPosition.y + vSize.y - gInterval
	},
		(*gpvpD2DBrush)[_INT(BRUSH_COLOR::WHITE)],
		D2D1_DRAW_TEXT_OPTIONS_CLIP,
		DWRITE_MEASURING_MODE_NATURAL
	);
}

void RenderTitle(CUiObject * pUiThis,ID2D1RenderTarget *pD2DRenderTarget)
{
	CUiObject* pParent = pUiThis->GetParent();
	XMFLOAT2 vParentSize = pParent->GetSize();
	XMFLOAT2 vPosition = pUiThis->GetPosition();
	const XMFLOAT2 &vSize = pUiThis->GetSize();

	if (pUiThis == CUiObject::m_pMouseOnObject)
		gpD2DBrush->SetColor(D2D1_COLOR_F{0.9f,0.8f,0,0.9f});
	else
		gpD2DBrush->SetColor(D2D1_COLOR_F{1,1,1,1});

	pD2DRenderTarget->FillRectangle(D2D1_RECT_F{vPosition.x,
												 vPosition.y, 
												 vPosition.x+vSize.x, 
												 vPosition.y+vSize.y
											},
											gpD2DBrush
										);

	const std::wstring &wsTitle = pUiThis->GetTitle();
	pD2DRenderTarget->DrawText(wsTitle.c_str(),
							wsTitle.size(),
							gpTextFormat,
							D2D1_RECT_F{
								vPosition.x+gInterval,
								vPosition.y+gInterval, 
								vPosition.x+vSize.x,
								vPosition.y+vSize.y-gInterval
							},
							(*gpvpD2DBrush)[_INT(BRUSH_COLOR::BLACK)],
							D2D1_DRAW_TEXT_OPTIONS_CLIP,
							DWRITE_MEASURING_MODE_NATURAL
					);
}

void RenderResize(CUiObject * pUiThis,ID2D1RenderTarget *pD2DRenderTarget)
{
	XMFLOAT2 vPosition = pUiThis->GetPosition();
	const XMFLOAT2 &vSize = pUiThis->GetSize();

	float fMinX = vPosition.x+2;
	float fMinY = vPosition.y+2;
	float fMaxX = vPosition.x+vSize.x-2;
	float fMaxY = vPosition.y+vSize.y-2;

	ID2D1SolidColorBrush* pBrush = nullptr;
	if (pUiThis == CUiObject::m_pMouseOnObject)
		pBrush = (*gpvpD2DBrush)[_INT(BRUSH_COLOR::RED)];
	else
		pBrush = (*gpvpD2DBrush)[_INT(BRUSH_COLOR::WHITE_A50)];

	pD2DRenderTarget->DrawLine(D2D1_POINT_2F{fMinX, 
												fMaxY}, 
								D2D1_POINT_2F{fMaxX,
												fMinY}, 
								pBrush, 2
							);
	pD2DRenderTarget->DrawLine(D2D1_POINT_2F{fMaxX, 
												fMinY}, 
								D2D1_POINT_2F{fMaxX,
												fMaxY}, 
								pBrush, 2
						);
	pD2DRenderTarget->DrawLine(D2D1_POINT_2F{fMaxX, 
												fMaxY}, 
								D2D1_POINT_2F{fMinX,
												fMaxY}, 
								pBrush, 2
						);
}

void RenderExit(CUiObject * pThis,ID2D1RenderTarget *pD2DRenderTarget)
{
	XMFLOAT2 vPosition = pThis->GetPosition();
	const XMFLOAT2 &vSize = pThis->GetSize();

	if (pThis == CUiObject::m_pMouseOnObject){
		float fMinX = vPosition.x + 2;
		float fMinY = vPosition.y + 2;
		float fMaxX = vPosition.x+vSize.x - 2;
		float fMaxY = vPosition.y+vSize.y - 2;

		pD2DRenderTarget->FillRectangle(D2D1_RECT_F{fMinX,fMinY,fMaxX, fMaxY}, (*gpvpD2DBrush)[_INT(BRUSH_COLOR::RED)]);

		fMinX += 3;
		fMinY += 3;
		fMaxX -= 3;
		fMaxY -= 3;

		pD2DRenderTarget->DrawLine(D2D1_POINT_2F{fMinX, 
												  fMinY}, 
									D2D1_POINT_2F{fMaxX, 
												  fMaxY},
									(*gpvpD2DBrush)[_INT(BRUSH_COLOR::WHITE)]
								);
		pD2DRenderTarget->DrawLine(D2D1_POINT_2F{fMaxX, 
												  fMinY}, 
									D2D1_POINT_2F{fMinX,
												  fMaxY}, 
									(*gpvpD2DBrush)[_INT(BRUSH_COLOR::WHITE)]
								);
	}
	else{
		float fMinX = vPosition.x + 5;
		float fMinY = vPosition.y + 5;
		float fMaxX = vPosition.x+vSize.x - 5;
		float fMaxY = vPosition.y+vSize.y - 5;
		pD2DRenderTarget->DrawLine(D2D1_POINT_2F{fMinX, 
												  fMinY}, 
									D2D1_POINT_2F{fMaxX, 
												  fMaxY},
									(*gpvpD2DBrush)[_INT(BRUSH_COLOR::BLACK)]
								);
		pD2DRenderTarget->DrawLine(D2D1_POINT_2F{fMaxX, 
												  fMinY}, 
									D2D1_POINT_2F{fMinX,
												  fMaxY}, 
									(*gpvpD2DBrush)[_INT(BRUSH_COLOR::BLACK)]
								);
	}
}

void RenderMin(CUiObject * pThis,ID2D1RenderTarget *pD2DRenderTarget)
{
	XMFLOAT2 vPosition = pThis->GetPosition();
	const XMFLOAT2 &vSize = pThis->GetSize();

	if (pThis == CUiObject::m_pMouseOnObject){
		float fMinX = vPosition.x + 2;
		float fMinY = vPosition.y + 2;
		float fMaxX = vPosition.x+vSize.x - 2;
		float fMaxY = vPosition.y+vSize.y - 2;

		pD2DRenderTarget->FillRectangle(D2D1_RECT_F{fMinX,fMinY,fMaxX, fMaxY}, (*gpvpD2DBrush)[_INT(BRUSH_COLOR::RED)]);

		fMinX += 3;
		fMinY += 3;
		fMaxX -= 3;
		fMaxY -= 3;

		pD2DRenderTarget->DrawLine(D2D1_POINT_2F{fMinX, 
												  fMaxY}, 
									D2D1_POINT_2F{fMaxX, 
												  fMaxY},
									(*gpvpD2DBrush)[_INT(BRUSH_COLOR::WHITE)]
								);
	}
	else{
		float fMinX = vPosition.x + 5;
		float fMinY = vPosition.y + 5;
		float fMaxX = vPosition.x+vSize.x - 5;
		float fMaxY = vPosition.y+vSize.y - 5;
		pD2DRenderTarget->DrawLine(D2D1_POINT_2F{fMinX, 
												  fMaxY}, 
									D2D1_POINT_2F{fMaxX, 
												  fMaxY},
									(*gpvpD2DBrush)[_INT(BRUSH_COLOR::BLACK)]
								);
	}
}

void RenderObjectPosition(CUiObject * pUiThis,ID2D1RenderTarget *pD2DRenderTarget)
{
	CObject* pObject = static_cast<CObject*>(pUiThis->GetData());
	XMFLOAT2 vPosition = pUiThis->GetPosition();
	const XMFLOAT2 &vSize = pUiThis->GetSize();

	WCHAR pString[100];
	XMFLOAT3 vObjectPos;
	XMStoreFloat3(&vObjectPos, pObject->GetPosition());

	swprintf_s(pString, L"%+07.2f, %+07.2f, %+07.2f", vObjectPos.x,vObjectPos.y,vObjectPos.z);
	float fMinX = vPosition.x + 2;
	float fMinY = vPosition.y + 2;
	float fMaxX = vPosition.x+vSize.x - 2;
	float fMaxY = vPosition.y+vSize.y - 2;

	if (pUiThis == CUiObject::m_pMouseOnObject)
		gpD2DBrush->SetColor(D2D1_COLOR_F{1,1,1,0.3f});
	else
		gpD2DBrush->SetColor(D2D1_COLOR_F{1,1,1,0.1f});

	//String Element의 Title 렌더도 필요함.

	pD2DRenderTarget->FillRectangle(D2D1_RECT_F{fMinX,fMinY,fMaxX, fMaxY}, gpD2DBrush);
	pD2DRenderTarget->DrawText(pString,
							lstrlen(pString),
							gpTextFormat,
							D2D1_RECT_F{
								vPosition.x+gInterval,
								vPosition.y+gInterval, 
								vPosition.x+vSize.x,
								vPosition.y+vSize.y-gInterval
							},
							(*gpvpD2DBrush)[_INT(BRUSH_COLOR::WHITE)],
							D2D1_DRAW_TEXT_OPTIONS_CLIP,
							DWRITE_MEASURING_MODE_NATURAL
					);
}

void RenderAddress(CUiObject * pUiThis, ID2D1RenderTarget * pD2DRenderTarget)
{
	CObject* pObject = static_cast<CObject*>(pUiThis->GetData());
	XMFLOAT2 vPosition = pUiThis->GetPosition();
	const XMFLOAT2 &vSize = pUiThis->GetSize();

	WCHAR pString[30];
	swprintf_s(pString, L"Reference : %0x", pObject);
	float fMinX = vPosition.x + 2;
	float fMinY = vPosition.y + 2;
	float fMaxX = vPosition.x + vSize.x - 2;
	float fMaxY = vPosition.y + vSize.y - 2;

	if (pUiThis == CUiObject::m_pMouseOnObject)
		gpD2DBrush->SetColor(D2D1_COLOR_F{ 1, 1, 1, 0.3f });
	else
		gpD2DBrush->SetColor(D2D1_COLOR_F{ 1, 1, 1, 0.1f });

	pD2DRenderTarget->FillRectangle(D2D1_RECT_F{ fMinX, fMinY, fMaxX, fMaxY }, gpD2DBrush);
	gpD2DBrush->SetColor(D2D1_COLOR_F{ 0.2f, 0.6f, 1, 1 });
	pD2DRenderTarget->DrawText(pString,
		lstrlen(pString),
		gpTextFormat,
		D2D1_RECT_F{
		vPosition.x + gInterval,
		vPosition.y + gInterval,
		vPosition.x + vSize.x,
		vPosition.y + vSize.y - gInterval
	},
		gpD2DBrush,
		D2D1_DRAW_TEXT_OPTIONS_CLIP,
		DWRITE_MEASURING_MODE_NATURAL
	);
}

void RenderBitmap(CUiObject * pUiThis, ID2D1RenderTarget * pD2DRenderTarget)
{
	ID2D1Bitmap* pBitmap = static_cast<ID2D1Bitmap*>(pUiThis->GetData());

	XMFLOAT2 vPosition = pUiThis->GetPosition();
	const XMFLOAT2 &vSize = pUiThis->GetSize();

	ID2D1SolidColorBrush* pBrush = nullptr;
	if (pUiThis == CUiObject::m_pMouseOnObject)
		pBrush = (*gpvpD2DBrush)[_INT(BRUSH_COLOR::WHITE_A50)];
	else
		pBrush = (*gpvpD2DBrush)[_INT(BRUSH_COLOR::WHITE_A25)];

	pD2DRenderTarget->FillRectangle(D2D1_RECT_F{ vPosition.x,
		vPosition.y,
		vPosition.x + vSize.x,
		vPosition.y + vSize.y
	},
		pBrush
	);

	D2D1_SIZE_F image_size = pBitmap->GetSize();
	float fImageRatioX = (image_size.width / image_size.height);
	float fImageRatioY = 1;

	XMFLOAT2 vNewPosition;
	XMFLOAT2 vNewSize;

	vNewSize.x = vSize.y * fImageRatioX;
	vNewSize.y = vSize.y * fImageRatioY;

	vNewPosition.x = vPosition.x + (vSize.x - vNewSize.x) / 2;
	vNewPosition.y = vPosition.y;

	// 완전 불투명 상태로 지정된 좌표에 비트맵을 출력한다.
	pD2DRenderTarget->DrawBitmap(pBitmap, D2D_RECT_F{ vNewPosition.x + gInterval,
		vNewPosition.y + gInterval,
		vNewPosition.x + vNewSize.x - gInterval,
		vNewPosition.y + vNewSize.y - gInterval
	});

	gpD2DBrush->SetColor(D2D1_COLOR_F{ 1.0f, 1.0f, 1.0f, 0.5f });
	pD2DRenderTarget->DrawRectangle(D2D1_RECT_F{ vNewPosition.x + gInterval,
		vNewPosition.y + gInterval,
		vNewPosition.x + vNewSize.x - gInterval,
		vNewPosition.y + vNewSize.y - gInterval
	},
		gpD2DBrush
	);

	gpD2DBrush->SetColor(D2D1_COLOR_F{ 0.0f, 0.0f, 0.0f, 0.4f });
	pD2DRenderTarget->FillRectangle(D2D1_RECT_F{ vPosition.x + gInterval,
		vPosition.y + gInterval,
		vPosition.x + vSize.x - gInterval,
		vPosition.y + 20 - gInterval
	},
		gpD2DBrush
	);

	const std::wstring &wsTitle = pUiThis->GetTitle();
	pD2DRenderTarget->DrawText(wsTitle.c_str(),
		wsTitle.size(),
		gpTextFormat,
		D2D1_RECT_F{
		vPosition.x + gInterval,
		vPosition.y + gInterval,
		vPosition.x + vSize.x,
		vPosition.y + vSize.y - gInterval
	},
		(*gpvpD2DBrush)[_INT(BRUSH_COLOR::WHITE)],
		D2D1_DRAW_TEXT_OPTIONS_CLIP,
		DWRITE_MEASURING_MODE_NATURAL
	);
}

void RenderModelData(CUiObject * pUiThis,ID2D1RenderTarget *pD2DRenderTarget)
{
	CModel* pModelData = static_cast<CModel*>(pUiThis->GetData());
	XMFLOAT2 vPosition = pUiThis->GetPosition();
	const XMFLOAT2 &vSize = pUiThis->GetSize();

	float fMinX = vPosition.x + gInterval;
	float fMinY = vPosition.y + gInterval;
	float fMaxX = vPosition.x+vSize.x - gInterval;
	float fMaxY = vPosition.y+vSize.y - gInterval;

	if (pUiThis == CUiObject::m_pMouseOnObject)
		gpD2DBrush->SetColor(D2D1_COLOR_F{1,1,1,0.3f});
	else
		gpD2DBrush->SetColor(D2D1_COLOR_F{1,1,1,0.1f});

	//String Element의 Title 렌더도 필요함.

	std::wstring wId = pModelData->GetName();
	pD2DRenderTarget->FillRectangle(D2D1_RECT_F{fMinX,fMinY,fMaxX, fMaxY}, gpD2DBrush);
	pD2DRenderTarget->DrawText(wId.c_str(),
							lstrlen(wId.c_str()),
							gpTextFormat,
							D2D1_RECT_F{
								vPosition.x+gInterval,
								vPosition.y+gInterval, 
								vPosition.x+vSize.x,
								vPosition.y+vSize.y-gInterval
							},
							(*gpvpD2DBrush)[_INT(BRUSH_COLOR::WHITE)],
							D2D1_DRAW_TEXT_OPTIONS_CLIP,
							DWRITE_MEASURING_MODE_NATURAL
					);
}
