#include "stdafx.h"
#include "UtilityFunction.h"

std::wstring GetTimeToString(time_t date)
{
	// time_t to tm
	struct tm temp ={0};
	localtime_s(&temp,&date);

	std::wstring stringDate;

	int year = temp.tm_year + 1900;
	int month = temp.tm_mon + 1;
	int day = temp.tm_mday;
	int hour = temp.tm_hour;
	int min = temp.tm_min;
	int sec = temp.tm_sec;

	//stringDate += std::to_wstring(year);
	//stringDate += L"/";

	//if(std::to_wstring(month).size() == 1) stringDate += L"0";
	//stringDate += std::to_wstring(month);
	//stringDate += L"/";

	//if(std::to_wstring(day).size() == 1) stringDate += L"0";
	//stringDate += std::to_wstring(day);

	stringDate += L"[";
	if(std::to_wstring(hour).size() == 1) stringDate += L"0";
	stringDate += std::to_wstring(hour);
	stringDate += L":";

	if(std::to_wstring(min).size() == 1) stringDate += L"0";
	stringDate += std::to_wstring(min);
	stringDate += L":";

	if(std::to_wstring(sec).size() == 1) stringDate += L"0";
	stringDate += std::to_wstring(sec);
	stringDate += L"]";

	return stringDate;
}

float RandomBetween(float a, float b) { return(a + (float)(rand() / (float)RAND_MAX) * (b - a)); }

void AddString(int integerNumber) {
	wsprintf(g_wString, L"%s%d", g_wString, integerNumber);
}

void AddString(UINT integerNumber)
{
	wsprintf(g_wString, L"%s%u", g_wString, integerNumber);
}

void AddString(float floatNumber) {
	wsprintf(g_wString, L"%s%f", g_wString, floatNumber);
}

void AddString(const char* pString) {
	size_t origsize = strlen(pString) + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t wcstring[newsize];
	mbstowcs_s(&convertedChars, wcstring, origsize, pString, _TRUNCATE);
	wsprintf(g_wString, L"%s%s", g_wString, wcstring);
}

void AddString(std::string& pString){
	size_t origsize = strlen(pString.c_str()) + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t wcstring[newsize];
	mbstowcs_s(&convertedChars, wcstring, origsize, pString.c_str(), _TRUNCATE);
	wsprintf(g_wString, L"%s%s", g_wString, wcstring);
}

void AddString(std::wstring & pString){
	wsprintf(g_wString, L"%s%s", g_wString, pString.c_str());
}

void AddString(const WCHAR* pString) {
	wsprintf(g_wString, L"%s%s", g_wString, pString);
}

void MyMessageBox() {
//	MB_OKCANCEL
	MessageBox(NULL, g_wString, 0, 0);
	ZeroMemory(g_wString, sizeof(WCHAR) * 100);
}

bool MyCancelMessageBox(){
	switch(MessageBox(0, g_wString, L"»Æ¿Œ", MB_OKCANCEL)){
	case 1:
		ZeroMemory(g_wString, sizeof(WCHAR) * 100);
		return true;
	case 2:
		ZeroMemory(g_wString, sizeof(WCHAR) * 100);
		return false;
	}
	return false;
}


//Operator
//File Input output Operator
std::ofstream& operator<<(std::ofstream& stream, const XMFLOAT2& vec){
	stream << vec.x << " " << vec.y;
	return stream;
}

std::ofstream& operator<<(std::ofstream& stream, const XMFLOAT3& vec){
	stream << vec.x << " " << vec.y << " " << vec.z;
	return stream;
}

std::ofstream& operator<<(std::ofstream& stream, const XMFLOAT4X4& mtx){
	stream << mtx._11 << " " << mtx._12 << " " << mtx._13 << " " << mtx._14 << " " <<
			  mtx._21 << " " << mtx._22 << " " << mtx._23 << " " << mtx._24 << " " <<
			  mtx._31 << " " << mtx._32 << " " << mtx._33 << " " << mtx._34 << " " <<
			  mtx._41 << " " << mtx._42 << " " << mtx._43 << " " << mtx._44;
	return stream;
}

std::ifstream& operator>>(std::ifstream& stream, XMFLOAT2& vec){
	stream >> vec.x >> vec.y;
	return stream;
};

std::ifstream& operator>>(std::ifstream& stream, XMFLOAT3& vec){
	stream >> vec.x >> vec.y >> vec.z;
	return stream;
};

std::ifstream& operator>>(std::ifstream& stream, XMFLOAT4X4& mtx){
	stream >> mtx._11 >> mtx._12 >> mtx._13 >> mtx._14 >>
			  mtx._21 >> mtx._22 >> mtx._23 >> mtx._24 >>
			  mtx._31 >> mtx._32 >> mtx._33 >> mtx._34 >>
			  mtx._41 >> mtx._42 >> mtx._43 >> mtx._44;
	return stream;
}

bool GetFileNameFormatA(std::string fileLocation,std::string & fileName,std::string & fileFormat) {
	int formatLen = fileLocation.length() - 1;

	while(formatLen > 0) {
		if('.' == fileLocation[formatLen - 1]) break;
		if('/' == fileLocation[formatLen - 1]) return false;
		if('\\' == fileLocation[formatLen - 1]) return false;
		formatLen--;
	}

	int nameLen = formatLen - 1;
	while(nameLen > 0) {
		if('/' == fileLocation[nameLen - 1])break;
		if('\\' == fileLocation[nameLen - 1])break;
		nameLen--;
	}

	for(int i = nameLen; i<formatLen - 1; ++i) fileName.push_back(fileLocation[i]);
	for(int i = formatLen; i<fileLocation.length(); ++i) fileFormat.push_back(tolower(fileLocation[i]));

	return true;
}

bool GetFileNameFormatW(std::wstring fileLocation,std::wstring & fileName,std::wstring & fileFormat) {
	int formatLen = fileLocation.length() - 1;

	while(formatLen > 0) {
		if(L'.' == fileLocation[formatLen - 1]) break;
		if('/' == fileLocation[formatLen - 1]) return false;
		if('\\' == fileLocation[formatLen - 1]) return false;
		formatLen--;
	}

	int nameLen = formatLen - 1;
	while(nameLen > 0) {
		if(L'/' == fileLocation[nameLen - 1])break;
		if(L'\\' == fileLocation[nameLen - 1])break;
		nameLen--;
	}

	for(int i = nameLen; i<formatLen - 1; ++i) fileName.push_back(fileLocation[i]);
	for(int i = formatLen; i<fileLocation.length(); ++i) fileFormat.push_back(tolower(fileLocation[i]));

	return true;
}