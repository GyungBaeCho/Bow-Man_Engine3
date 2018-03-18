#pragma once

enum class ChatType : int{
	SYSTEM,
	USER,
};

struct ChatData{
	std::vector<std::wstring> m_vChat;
	std::vector<ChatType> m_vChatType;

	ChatData(){}
	~ChatData(){}

	void AddSystemMessage(const std::wstring& sysMsg);
	void AddUserMessage(const std::wstring& sysMsg);

	void OperateUserMessage(const std::wstring& sysMsg);
};
