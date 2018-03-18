#pragma once

std::wstring GetTimeToString(time_t date = time(NULL));

float RandomBetween(float a, float b);

#define BUFF_SIZE	128
__declspec(selectany) WCHAR g_wString[BUFF_SIZE];
void AddString(int integerNumber);
void AddString(UINT integerNumber);
void AddString(float floatNumber);
void AddString(const char* pString);
void AddString(std::string& pString);
void AddString(std::wstring& pString);
void AddString(const WCHAR* pString);
void MyMessageBox();
template<typename  Ty, typename...Args>
void MyMessageBox(Ty ty, Args...args) {
	AddString(ty);
	MyMessageBox(args...);
}
bool MyCancelMessageBox();
template<typename  Ty, typename...Args>
bool MyCancelMessageBox(Ty ty, Args...args) {
	AddString(ty);
	return MyCancelMessageBox(args...);
}

//File Input output Operator
std::ofstream& operator<<(std::ofstream& stream, const XMFLOAT2& vec);
std::ofstream& operator<<(std::ofstream& stream, const XMFLOAT3& vec);
std::ofstream& operator<<(std::ofstream& stream, const XMFLOAT4X4& mtx);
std::ifstream& operator>>(std::ifstream& stream, XMFLOAT2& vec);
std::ifstream& operator>>(std::ifstream& stream, XMFLOAT3& vec);
std::ifstream& operator>>(std::ifstream& stream, XMFLOAT4X4& mtx);

bool GetFileNameFormatA(std::string fileLocation,std::string& fileName,std::string& fileFormat);
bool GetFileNameFormatW(std::wstring fileLocation, std::wstring& fileName, std::wstring& fileFormat);
#ifdef UNICODE
#define GetFileNameFormat  GetFileNameFormatW
#else
#define GetFileNameFormat  GetFileNameFormatA
#endif 

//#if defined(DEBUG) | defined(_DEBUG)
//#ifndef HR
//#define HR(x)									\
//		{												\
//			HRESULT hr = (x);							\
//			if(FAILED(hr))								\
//			{											\
//				MyMessageBox(DXGetErrorStringA(hr), "\n", DXGetErrorDescriptionA(hr), "\nfile: " ,(DWORD)__FILE__, "\n", "\nline: ", (DWORD)__LINE__);\
//				exit(-1);								\
//			}											\
//		}												
//	#endif 
//#else
//	#ifndef HR
//	#define HR(x) (x)
//	#endif
//#endif 