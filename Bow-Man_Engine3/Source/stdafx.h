// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <wchar.h>

//File Drag & Drop
#include <ShellAPI.h>

//IME
#include <imm.h>
#pragma comment(lib, "imm32")

//stdio.h 대용품
#pragma comment(lib, "legacy_stdio_definitions.lib")

#include <math.h>
#include <time.h>
#include <conio.h>

#include <iostream>
using std::cout;
using std::endl;
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <algorithm>

//FBX Library
#include <fbxsdk.h>

//For Texture Formats
#include <Wincodec.h>


//Sound
//dsound.lib
//X3DAudio.lib
#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")
#include <x3Daudio.h>

#include <wrl.h>
using Microsoft::WRL::ComPtr;

//DirectX3D
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXCollision.h>
#include <DirectXColors.h>
using namespace DirectX;
using namespace DirectX::PackedVector;
#define XMASSERT assert 
#define XMVectorPermute( a, b, c) XMVectorPermute(a,b, c.i[0], c.i[1], c.i[2], c.i[3] )
#define _DECLSPEC_ALIGN_16_ __declspec(align(16))
#include <d3d11_4.h>
#include <d3dcompiler.h> 
#include <dxgi1_4.h>
#include <DxErr.h>
#include <DirectXTex.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "DXErr.lib")
#pragma comment(lib, "dxgi.lib")
#ifdef _DEBUG
#pragma comment(lib, "DirectXTex_Debug.lib")
#pragma comment(lib, "libfbxsdk-md_Debug.lib")
#pragma comment(lib, "libfbxsdk-mt_Debug.lib")
#pragma comment(lib, "libfbxsdk_Debug.lib")
#else
#pragma comment(lib, "DirectXTex.lib")
#pragma comment(lib, "libfbxsdk-md.lib")
#pragma comment(lib, "libfbxsdk-mt.lib")
#pragma comment(lib, "libfbxsdk.lib")
#endif

//Direct2D
#include <d2d1_2.h>
#include <dwrite.h>
#pragma comment(lib, "D2D1.lib")
#pragma comment(lib, "dwrite.lib")

//Network
#include <Ws2tcpip.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32")


//#include <dsetup.h>
//#pragma comment(lib, "d3dx9.lib")
//#pragma comment(lib, "d3dx9d.lib")
//#pragma comment(lib, "d3dx11.lib")
//#pragma comment(lib, "dxguid.lib")
//#pragma comment(lib, "winmm.lib")
//#pragma comment(lib, "dinput8.lib")
//#pragma comment(lib, "XInput.lib")
//#pragma comment(lib, "dsound.lib")
//#pragma comment(lib, "X3DAudio.lib")
//#pragma comment(lib, libfbxsdk-md.lib)
//#pragma comment(lib, libfbxsdk.lib)
//#pragma comment(lib, fmodex64_vc.lib)
//#pragma comment(lib, AntTweakBar64.lib)


//#define _WITH_MSAA4_MULTISAMPLING

//#if defined(DEBUG) | defined(_DEBUG)
//	#ifndef HR
	#define HR(x)                                              \
	{                                                          \
		HRESULT hr = (x);                                      \
		if(FAILED(hr))                                         \
		{                                                      \
			DXTrace(__FILE__, (DWORD)__LINE__, hr, L#x, true); \
			exit(-1);										   \
		}                                                      \
	}
//	#endif

//#else
//	#ifndef HR
//	#define HR(x) (x)
//	#endif
//#endif 

//#if defined(_DEBUG) || defined(DEBUG)
//#ifndef HR
//#define HR(x){																											\
//		HRESULT hr = (x);																								\
//		if(FAILED(hr)){																									\
//			std::cout<<"An error occured on line"<<(DWORD)__LINE__<<std::endl;											\
//			std::cout<<"in the file"<<(DWORD)__FILE__<<std::endl;														\
//			std::cout<<DXGetErrorStringA(hr)<<std::endl;																\
//			std::cout<<DXGetErrorDescriptionA(hr)<<std::endl<<std::endl;												\
//		}																												\
//	}
//#endif
//#else
//	#ifndef HR
//	#define HR(x) (x)
//	#endif
//#endif 
//

#define _INT(X)		static_cast<unsigned int>(X)

#define	SAFE_RELEASE(x)		if(x) x->Release(); 
void Test();


//유니코드-윈메인 CMD
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")	

// TODO: reference additional headers your program requires here

#define VK_0									0X30
#define VK_1									0X31
#define VK_2									0X32
#define VK_3									0X33
#define VK_4									0X34
#define VK_5									0X35
#define VK_6									0X36
#define VK_7									0X37
#define VK_8									0X38
#define VK_9									0X39

#define VK2_0									0X60
#define VK2_1									0X61
#define VK2_2									0X62
#define VK2_3									0X63
#define VK2_4									0X64
#define VK2_5									0X65
#define VK2_6									0X66
#define VK2_7									0X67
#define VK2_8									0X68
#define VK2_9									0X69

#define VK_A									0x41
#define VK_B									0x42
#define VK_C									0x43
#define VK_D									0x44
#define VK_E									0x45
#define VK_F									0x46
#define VK_G									0x47
#define VK_H									0x48
#define VK_I									0x49
#define VK_J									0x4A
#define VK_K									0x4B
#define VK_L									0x4C
#define VK_M									0x4D
#define VK_N									0x4E
#define VK_O									0x4F
#define VK_P									0x50
#define VK_Q									0x51
#define VK_R									0x52
#define VK_S									0x53
#define VK_T									0x54
#define VK_U									0x55
#define VK_V									0x56
#define VK_W									0x57
#define VK_X									0x58
#define VK_Y									0x59
#define VK_Z									0x5A
//#define VK_BACKSPACE							0X08
//#define VK_ENTER								0X0D
//#define VK_DELETE								0X2E

#define KEY_FOWARD								0x01
#define KEY_BACKWARD							0x02
#define KEY_LEFTWARD							0x04
#define KEY_RIGHTWARD							0x08
#define KEY_UPWARD								0x10
#define KEY_DOWNWARD							0x20
#define KEY_RUN									0x40

enum AXIS : char{
	X = 0, 
	Y = 1, 
	Z = 2,
};

//_declspec(selectany) int FRAME_BUFFER_WIDTH = GetSystemMetrics(SM_CXSCREEN);
//_declspec(selectany) int FRAME_BUFFER_HEIGHT = GetSystemMetrics(SM_CYSCREEN);
_declspec(selectany) LONG FRAME_BUFFER_WIDTH = 1000;
_declspec(selectany) LONG FRAME_BUFFER_HEIGHT = 750;
_declspec(selectany) LONG FRAME_BUFFER_POS_X = 100;
_declspec(selectany) LONG FRAME_BUFFER_POS_Y = 100;



//__declspec(selectany) int x1=1;  
//
//const __declspec(selectany) int x2 =2;  
//
//extern const __declspec(selectany) int x3=3;  
//
//extern const int num1;
//const __declspec(selectany) int num1 = 2100;
//
//extern __declspec(selectany) int x5;  