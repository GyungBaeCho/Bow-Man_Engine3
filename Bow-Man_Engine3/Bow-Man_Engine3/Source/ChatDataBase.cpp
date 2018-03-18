#include "stdafx.h"
#include "ChatDataBase.h"
#include "UtilityFunction.h"
#include "Timer.h"
#include "ServerFramework.h"

void ChatData::AddSystemMessage(const std::wstring& sysMsg){
	std::wstring info = L"[System]";
	info += GetTimeToString();
	info += L" : ";
	info += sysMsg;
	m_vChat.push_back(info);
	m_vChatType.push_back(ChatType::SYSTEM);
}

void ChatData::AddUserMessage(const std::wstring& sysMsg)
{
	std::wstring info = L"[User]";
	info += GetTimeToString();
	info += L" : ";
	info += sysMsg;
	m_vChat.push_back(info);
	m_vChatType.push_back(ChatType::USER);

	OperateUserMessage(sysMsg);
}

enum class SystemOperation : UINT{


};

void ChatData::OperateUserMessage(const std::wstring & inputMsg){
	std::wstring resultMessage;

	if (inputMsg.substr(0, 6) == _T("SYSTEM")) {
		std::wistringstream sTexCoord(inputMsg.substr(7));
		
		while(!sTexCoord.eof()){
			std::wstring msg;
			sTexCoord >> msg;

			if (msg == _T("fps")) {
				AddSystemMessage(TimerEngine->GetText());
			}
			else if (msg == _T("time")) {
				AddSystemMessage(GetTimeToString());
			}
			else if (msg == _T("run")) {
				sTexCoord >> msg;
				if (msg == _T("access_thread"))
					Server->CreateAcceptThread();
				else
					AddSystemMessage(_T("Invalid Excute Run " + msg));
			}
			else if (msg == _T("connect")) {
				sTexCoord >> msg;
				Server->Connect(msg.c_str());
			}
			else if (msg == _T("?")) {
				AddSystemMessage(_T("fps"));
				AddSystemMessage(_T("time"));
				AddSystemMessage(_T("run"));
				AddSystemMessage(_T("connect"));
			}
			else {
				AddSystemMessage(_T("Invalidate Instruction"));
			}
		}
	}
}
