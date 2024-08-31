// mcisvr.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#pragma comment(lib, "Ws2_32.lib")


#include <winsock2.h>
#include <windows.h>
#include <iostream>

using namespace std;

DWORD WINAPI ClientThreadRec(LPVOID lpParameter)
{
	SOCKET Socket = (SOCKET)lpParameter;
	int Ret = 0;
	char RecvBuffer[MAX_PATH];

	while ( true )
	{
		memset(RecvBuffer, 0x00, sizeof(RecvBuffer));
		Ret = recv(Socket, RecvBuffer, MAX_PATH, 0);
		if ( Ret == 0 || Ret == SOCKET_ERROR ) 
		{
			cout<<"�ͻ��˶Ͽ�����!"<<endl;
			break;
		}
		cout<<"client:"<<RecvBuffer<<endl;
	}

	return 0;
}

DWORD WINAPI ClientThreadRSend(LPVOID lpParameter)
{
	SOCKET Socket = (SOCKET)lpParameter;
	int Ret = 0;
	char SendBuffer[MAX_PATH];
	while ( true )
	{
		cin.getline(SendBuffer, sizeof(SendBuffer));
		Ret = send(Socket, SendBuffer, (int)strlen(SendBuffer), 0);
		if ( Ret == SOCKET_ERROR )
		{
			cout<<"������Ϣʧ��::"<<GetLastError()<<endl;
			break;
		}
	}

	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	cout<<endl;cout<<endl;cout<<endl;

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	int AddrLen = 0;
	HANDLE hThreadSed = NULL;
	HANDLE hThreadRec = NULL;
	wVersionRequested = MAKEWORD( 2, 2 );
	err = WSAStartup( wVersionRequested, &wsaData );


	SOCKET s,CientSocket;
	s = socket(AF_INET,SOCK_STREAM,0);

	char *ptrip=NULL;
	char name[32]="";
	PHOSTENT hostinfo;
	if(gethostname (name, sizeof(name)) == 0)
	{ 
		//����ɹ� �����������������name������
		if((hostinfo = gethostbyname(name)) != NULL) 
		{ 
			//��ȡ����IP
			ptrip = inet_ntoa (*(struct in_addr *)*hostinfo->h_addr_list);
		}
	}

	SOCKADDR_IN addr,ClientAddr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ptrip);
	addr.sin_port = htons(9000);

	int ret = bind(s,(LPSOCKADDR)&addr,sizeof(addr));
	if( ret == SOCKET_ERROR ){
		int error = WSAGetLastError();
		closesocket(s);
		s = NULL;
		WSACleanup();
		return 0;
	}
	cout<<"�������󶨶˿�"<<ptrip<<endl;

	ret = listen(s,5);
	if( ret == SOCKET_ERROR ){
		closesocket(s);
		s = NULL;
		WSACleanup();
		return 0;
	}
	cout<<"�������˿ڿ�������"<<endl;

	cout<<"������Ѿ�����"<<endl;

	// ѭ��ȷ���Ƿ����µ�����
	while ( true )
	{
		AddrLen = sizeof(ClientAddr);
		CientSocket = accept(s, (struct sockaddr*)&ClientAddr, &AddrLen);
		if ( CientSocket == INVALID_SOCKET )
		{
			cout<<"Accept Failed::"<<GetLastError()<<endl;
			break;
		}
		// ÿ�����¿ͻ���������ʱ���ʹ���һ�Զ����ͽ����̵߳��̡߳�

		cout<<"�ͻ�������::"<<inet_ntoa(ClientAddr.sin_addr)<<":"<<ClientAddr.sin_port<<endl;

		hThreadRec = CreateThread(NULL, 0, ClientThreadRec, (LPVOID)CientSocket, 0, NULL);
		if ( hThreadRec == NULL )
		{
			cout<<"Create ThreadRec Failed!"<<endl;
			break;
		}
		hThreadSed = CreateThread(NULL, 0, ClientThreadRSend, (LPVOID)CientSocket, 0, NULL);
		if ( hThreadSed == NULL )
		{
			cout<<"Create ThreadSed Failed!"<<endl;
			break;
		}

		CloseHandle(hThreadRec);
		CloseHandle(hThreadSed);
	}

	closesocket(s);
	closesocket(CientSocket);
	WSACleanup();

	cout<<endl;cout<<endl;cout<<endl;
	system("PAUSE");

	//FreeLibrary(hdll);

	return 0;
}

