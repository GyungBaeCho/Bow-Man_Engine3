#pragma once

//_CRT_SECURE_NO_WARNINGS
//_WINSOCK_DEPRECATED_NO_WARNINGS
#define WM_SOCKET                   WM_USER + 1               

#define MY_SERVER_PORT				4000

#define MAX_BUFF_SIZE               4000
#define MAX_NAME_SIZE               16         

#define REGULAR_SEND_INTERVAL      1700

struct CLIENT
{
	char         id[MAX_NAME_SIZE];            // 플레이어 이름
	int          key;						 // 플레이어 키
	unsigned int location;					// 플레이어 위치
	SOCKET      sock;					// 소켓

	WSABUF      send_wsabuf;               // 송신 버퍼
	char         send_buffer[MAX_BUFF_SIZE];

	WSABUF      recv_wsabuf;               // 수신 버퍼
	char         recv_buffer[MAX_BUFF_SIZE];
	char         packet_buffer[MAX_BUFF_SIZE];   // 수신 중인 패킷 조립 버퍼

	int         packet_size;               // 패킷 크기
	int         transfered_packet_size;         // 송수신 중인 패킷량
};



#define Server		CServerFramework::GetInstance()

class CServerFramework{
private:
	static CServerFramework *m_pInstance;
	HANDLE m_hIocp;

	SOCKET			m_ClientSock;
	SOCKET			m_ListeningSock;
	std::thread		*m_pWaitThread;

private:
	CServerFramework() {}
	~CServerFramework() {}

public:
	static void CreateInstance() {
		if (m_pInstance) return;
		m_pInstance = new CServerFramework;
	}

	static CServerFramework* GetInstance() {
		return m_pInstance;
	}

	void Initialize();

	void Release() {
		closesocket(m_ClientSock);
		closesocket(m_ListeningSock);
		if (m_pWaitThread) {
			m_pWaitThread->join();
			delete m_pWaitThread;
		}
		WSACleanup();
	}

	static void WaitThread(CServerFramework * pServerFramework);

	void Connect(const WCHAR * wIpAddress);

	void CreateAcceptThread() {
		static bool bFlag = false;
		if (bFlag) return;
		bFlag = true;
		m_pWaitThread = new std::thread{ WaitThread, this };
	}
};

__declspec(selectany) CServerFramework *CServerFramework::m_pInstance = nullptr;