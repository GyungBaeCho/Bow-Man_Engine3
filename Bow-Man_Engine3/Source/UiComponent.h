#pragma once
class CUiObject;
//void DefaultFunc(CUiObject* pThis, HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam){}
//int  DefaultFunc(CUiObject* pUiThis, XMFLOAT2& pvPos){ return 1; }
//void DefaultFunc(CUiObject* pUiThis){}

//Boolean
void RenderBoolElement(CUiObject* pUiThis, ID2D1RenderTarget *pD2DRenderTarget);
bool MouseInputBool(CUiObject* pThis, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//MouseInput
bool MouseInputDefault(CUiObject* pThis, HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
bool MouseInputMove(CUiObject* pThis,HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
bool MouseInputResize(CUiObject * pThis,HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
bool MouseInputExit(CUiObject * pThis,HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
bool MouseInputMinimize(CUiObject * pThis,HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
bool MouseInputWebPage(CUiObject * pThis, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
bool MouseInputTexture(CUiObject* pThis, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
bool MouseInputTextureAddress(CUiObject* pThis, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
bool MouseInputShader(CUiObject* pThis, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
bool MouseInputShaderAddress(CUiObject* pThis, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
bool MouseInputModel(CUiObject* pThis, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
bool MouseInputDpthStncl(CUiObject* pThis, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
bool MouseInputRstrzr(CUiObject* pThis, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
bool MouseInputBlnd(CUiObject* pThis, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//KeyInput
void KeyInputDefault(CUiObject* pThis, HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
void KeyInputChat(CUiObject* pThis, HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

//Update
void UpdateDefault(CUiObject* pUiThis);

//UpdateCoord
int UpdateCoord(CUiObject* pUiThis, XMFLOAT2& pvPos);

//Render
void RenderDefault(CUiObject* pUiThis,ID2D1RenderTarget *pD2DRenderTarget);

void RenderSuperParent(CUiObject* pUiThis,ID2D1RenderTarget *pD2DRenderTarget);
void RenderTitle(CUiObject* pUiThis,ID2D1RenderTarget *pD2DRenderTarget);
void RenderResize(CUiObject* pUiThis,ID2D1RenderTarget *pD2DRenderTarget);
void RenderExit(CUiObject* pUiObject,ID2D1RenderTarget *pD2DRenderTarget);
void RenderMin(CUiObject* pUiObject,ID2D1RenderTarget *pD2DRenderTarget);

void RenderInt(CUiObject* pUiThis,ID2D1RenderTarget *pD2DRenderTarget);
void RenderFloat(CUiObject* pUiThis, ID2D1RenderTarget *pD2DRenderTarget);
void RenderWchar(CUiObject* pUiThis,ID2D1RenderTarget *pD2DRenderTarget);
void RenderWstring(CUiObject* pUiThis, ID2D1RenderTarget *pD2DRenderTarget);
void RenderAddress(CUiObject* pUiThis, ID2D1RenderTarget *pD2DRenderTarget);
void RenderBitmap(CUiObject* pUiThis, ID2D1RenderTarget *pD2DRenderTarget);

void RenderElement(CUiObject* pUiThis,ID2D1RenderTarget *pD2DRenderTarget);
void RenderTexture(CUiObject* pUiThis, ID2D1RenderTarget *pD2DRenderTarget);
void RenderTextureAddress(CUiObject* pUiThis, ID2D1RenderTarget *pD2DRenderTarget);
void RenderMaterialAddress(CUiObject* pUiThis, ID2D1RenderTarget *pD2DRenderTarget);
void RenderShader(CUiObject* pUiThis, ID2D1RenderTarget *pD2DRenderTarget);
void RenderShaderAddress(CUiObject* pUiThis, ID2D1RenderTarget *pD2DRenderTarget);
void RenderChatDataBase(CUiObject* pUiThis,ID2D1RenderTarget *pD2DRenderTarget);
void RenderChatter(CUiObject* pUiThis,ID2D1RenderTarget *pD2DRenderTarget);
void RenderFPS(CUiObject* pUiObject,ID2D1RenderTarget *pD2DRenderTarget);
void RenderStopWatch(CUiObject* pUiObject,ID2D1RenderTarget *pD2DRenderTarget);
void RenderObjectPosition(CUiObject* pUiThis,ID2D1RenderTarget *pD2DRenderTarget);
void RenderModelData(CUiObject* pUiThis,ID2D1RenderTarget *pD2DRenderTarget);


