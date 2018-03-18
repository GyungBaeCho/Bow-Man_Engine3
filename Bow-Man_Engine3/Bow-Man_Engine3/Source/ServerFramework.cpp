#include "stdafx.h"
#include "ServerFramework.h"
#include "UiObject.h"

// ���� �Լ� ���� ���
static void err_display(char *msg, int err_no)
{
	/*
	http://shaeod.tistory.com/317
	http://darkpgmr.tistory.com/6

	_wsetlocale( LC_ALL, L"korean" );      //����ȭ ������ ���������� ����
	wcout.imbue( locale("korean") );        //��½� �κ��� ����
	wcin.imbue( locale("korean") );          //�Է½� �κ��� ����
	*/

	// ���� locale�� ����
	char *old_locale_tmp = setlocale(LC_ALL, NULL); // �ӽ� locale ���ڿ� �ּ� ��ȯ
	char *old_locale_saved = _strdup(old_locale_tmp); // ��ȯ���� old locale ���ڿ� �ּҰ� ���� setlocale ȣ��� ������� �ʵ��� ������ �޸𸮸� �Ҵ�޾Ƽ� ����


	setlocale(LC_ALL, "korean"); // locale�� �����ϰ� �ʿ��� �۾��� ����

	WCHAR *lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);

	printf("%s\n", msg);
	wprintf(L"���� - %s\n", lpMsgBuf);
	LocalFree(lpMsgBuf);

	// locale�� ����
	setlocale(LC_ALL, old_locale_saved);
	free(old_locale_saved);

	//_free_locale(tmp_locale)
	//wcout.imbue(locale(NULL));
}

void CServerFramework::Initialize() {
	WSADATA	wsadata;
	_wsetlocale(LC_ALL, L"korean");
	WSAStartup(MAKEWORD(2, 2), &wsadata);
	m_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);

	//char* ip = "www.naver.com";
	//char* port = "https";
	//struct addrinfo aiHints;
	//struct addrinfo *aiList = NULL;
	//int retVal;
	//
	//memset(&aiHints, 0, sizeof(aiHints));
	//aiHints.ai_flags = AI_PASSIVE;
	//aiHints.ai_family = AF_UNSPEC;
	//aiHints.ai_socktype = SOCK_STREAM;
	//aiHints.ai_protocol = IPPROTO_TCP;
	//
	//if ((retVal = getaddrinfo(ip, port, &aiHints, &aiList)) != 0)
	//{
	//	perror("getaddrinfo");
	//	printf("getaddrinfo() failed.\n");
	//}
	//else
	//	printf("getaddrinfo() is OK.\n");
	//
	//WCHAR wipaddr[50];
	//InetNtop(AF_INET, &aiList->ai_addr, wipaddr, 50);
	//freeaddrinfo(aiList);

	PHOSTENT hostinfo;
	char hostname[50];
	char ipaddr[50];
	memset(hostname, 0, sizeof(hostname));
	memset(ipaddr, 0, sizeof(ipaddr));
	int nError = gethostname(hostname, sizeof(hostname));

	if (nError == 0) {
		hostinfo = gethostbyname(hostname);
		strcpy(ipaddr, inet_ntoa(*(struct in_addr*)hostinfo->h_addr_list[0]));
	}

	std::wstring wMessege = TEXT("Window Socket Initialized");
	SystemMessage(wMessege);

	WCHAR wIpAddress[50];
	swprintf_s(wIpAddress, 50, TEXT("IP Address - [%hs]"), ipaddr);
	SystemMessage(wIpAddress);

	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	cout << "dwPageSize" << " : " << sysinfo.dwPageSize << endl;
	cout << "lpMinimumApplicationAddress" << " : " << sysinfo.lpMinimumApplicationAddress << endl;
	cout << "lpMaximumApplicationAddress" << " : " << sysinfo.lpMaximumApplicationAddress << endl;
	cout << "dwActiveProcessorMask" << " : " << sysinfo.dwActiveProcessorMask << endl;
	cout << "dwNumberOfProcessors" << " : " << sysinfo.dwNumberOfProcessors << endl;
	cout << "dwProcessorType" << " : " << sysinfo.dwProcessorType << endl;
	cout << "dwAllocationGranularity" << " : " << sysinfo.dwAllocationGranularity << endl;
	cout << "wProcessorLevel" << " : " << sysinfo.wProcessorLevel << endl;
	cout << "wProcessorRevision" << " : " << sysinfo.wProcessorRevision << endl;
	cout << "-------------------------------------" << endl << endl;
}

void CServerFramework::WaitThread(CServerFramework * pServerFramework) {
	sockaddr_in listen_addr;

	pServerFramework->m_ListeningSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	ZeroMemory(&listen_addr, sizeof(listen_addr));
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr = htonl(INADDR_ANY); //��� Ŭ���̾�Ʈ�� �ּҿ��� �޾ƶ�
	listen_addr.sin_port = htons(MY_SERVER_PORT);
	ZeroMemory(&listen_addr.sin_zero, 8);

	//C++ �� bind�� socket programing�� bind�� �浹�Ѵ�
	::bind(pServerFramework->m_ListeningSock, reinterpret_cast<SOCKADDR*>(&listen_addr), sizeof(listen_addr));
	listen(pServerFramework->m_ListeningSock, 10);

	SystemMessage(L"Accept Thread Activated");

	while (true) {
		sockaddr_in client_addr;
		int add_size = sizeof(client_addr);
		SOCKET new_client = ::WSAAccept(pServerFramework->m_ListeningSock, reinterpret_cast<SOCKADDR*>(&client_addr), &add_size, NULL, NULL);

		if (new_client == INVALID_SOCKET)
		{
			int error_no = WSAGetLastError();
			err_display("[Error] - accept()", error_no);
			break;
		}

		WCHAR wAccessMessage[50];
		swprintf_s(wAccessMessage, 50, L"Access Requested : %hs", inet_ntoa(client_addr.sin_addr));
		SystemMessage(wAccessMessage);
		
	//	swprintf_s(wAccessMessage, 50, L"");
	//	SystemMessage(wAccessMessage);
	}
}

void CServerFramework::Connect(const WCHAR * wIpAddress) {
	m_ClientSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);

	int strSize = WideCharToMultiByte(CP_ACP, 0, wIpAddress, -1, NULL, 0, NULL, NULL);
	char* pServerIp = new char[strSize];
	WideCharToMultiByte(CP_ACP, 0, wIpAddress, -1, pServerIp, strSize, 0, 0);

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(SOCKADDR_IN));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(MY_SERVER_PORT);
	serveraddr.sin_addr.s_addr = inet_addr(pServerIp);

//	auto ConnectFunc = [&]() {
		int retval = WSAConnect(m_ClientSock, (sockaddr *)&serveraddr, sizeof(serveraddr), NULL, NULL, NULL, NULL);
		if (retval == SOCKET_ERROR)
			SystemMessage(_T("Connection Failed"));
		else
			SystemMessage(_T("Connection Successed"));
//	};

//	static std::thread th{ ConnectFunc };

}
