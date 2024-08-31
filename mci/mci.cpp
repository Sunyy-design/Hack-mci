// mci.cpp : ����Ӧ�ó������ڵ㡣
//
#include <windows.h>

#include "stdafx.h"
#include "mci.h"
#include "VolumeOutMaster.h"
#include "VolumeInXXX.h"
#include "VolumeOutWave.h"

//#include "VolumeControl.h"
#include "MyVolumeCtrl.h"

#include <Winsock2.h>
#pragma comment(lib, "Ws2_32.lib")

using namespace std;

#define  PORT 9000

static int b = 0;

DWORD WINAPI ShutUp(LPVOID lpParameter)
{
	ULONGLONG o ;
	o = ::GetTickCount();
	CMyVolumeCtrl cc;
	int volum = 33;

	while(1) {
		if( b == 1)
		{
			Sleep(1000);
			volum = 0;
			cc.SetVolume(volum);
		}
		else if(0 == volum)
		{
			volum = 50;
			cc.SetVolume(volum);
		}
	}
	return 0;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	WSADATA Ws;
	SOCKET CientSocket;
	struct sockaddr_in ServerAddr;
	int Ret = 0;
	int AddrLen = 0;
	hostent	*thisHost;
	char *ip;

// ������˹رգ�ϵͳ�Զ������￪ʼ�ظ����ӡ�
restart:
	thisHost = NULL;
	ip = NULL;

	//��ʼ�� Windows Socket
	if ( WSAStartup(MAKEWORD(2,2), &Ws) != 0 )
	{
		//��ʼ��Socketʧ��
		return -1;
	}

	//���� Socket
	CientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if ( CientSocket == INVALID_SOCKET )
	{
		// ����socketʧ��
		return -1;
	}

	//�������������ip��ַ
	
	thisHost	=	gethostbyname("Sun-PC");
	ip	=	inet_ntoa(*(struct in_addr *)*(thisHost->h_addr_list)); 

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.s_addr = inet_addr(ip);
	ServerAddr.sin_port = htons(PORT);
	memset(ServerAddr.sin_zero, 0x00, 8);

	//��������
	Ret = connect(CientSocket,(struct sockaddr*)&ServerAddr, sizeof(ServerAddr));
	//������ɹ���һֱ����
	while( Ret == SOCKET_ERROR )
	{
		Ret = connect(CientSocket,(struct sockaddr*)&ServerAddr, sizeof(ServerAddr));
		// ����ʧ����������
	}

	HANDLE hThread = CreateThread(NULL, 0, ShutUp, (LPVOID)CientSocket, 0, NULL);
	if ( hThread == NULL )
	{
		// ���������߳�ʧ��
		goto end;
	}

	// ����ѭ��
	char RecvBuffer[MAX_PATH];
	while ( true )
	{
		memset(RecvBuffer, 0x00, sizeof(RecvBuffer));
		Ret = recv(CientSocket, RecvBuffer, MAX_PATH, 0);
		if ( Ret == 0 || Ret == SOCKET_ERROR ) 
		{
			// �������ر�
			closesocket(CientSocket);
			WSACleanup();
			goto restart;
		}
		// �����յ�����������Ϣ ���д���
		if(0 == strcmp(RecvBuffer,"����"))
		{
			b = 1;
		}

		if(0 == strcmp(RecvBuffer,"����"))
		{
			b = 0;
		}

		if(0 == strcmp(RecvBuffer,"�ػ�"))
		{
			system("shutdown -s");
		}

	}

end:
	closesocket(CientSocket);
	WSACleanup();

	return (int) 0;
}