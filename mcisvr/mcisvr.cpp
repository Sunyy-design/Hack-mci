// mcisvr.cpp : 定义控制台应用程序的入口点。
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
			cout<<"客户端断开连接!"<<endl;
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
			cout<<"发送消息失败::"<<GetLastError()<<endl;
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
		//如果成功 将本地主机名存放入name缓冲区
		if((hostinfo = gethostbyname(name)) != NULL) 
		{ 
			//获取主机IP
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
	cout<<"服务器绑定端口"<<ptrip<<endl;

	ret = listen(s,5);
	if( ret == SOCKET_ERROR ){
		closesocket(s);
		s = NULL;
		WSACleanup();
		return 0;
	}
	cout<<"服务器端口开启监听"<<endl;

	cout<<"服务端已经启动"<<endl;

	// 循环确定是否有新的连接
	while ( true )
	{
		AddrLen = sizeof(ClientAddr);
		CientSocket = accept(s, (struct sockaddr*)&ClientAddr, &AddrLen);
		if ( CientSocket == INVALID_SOCKET )
		{
			cout<<"Accept Failed::"<<GetLastError()<<endl;
			break;
		}
		// 每当有新客户端连接入时，就创建一对儿发送接收线程的线程。

		cout<<"客户端连接::"<<inet_ntoa(ClientAddr.sin_addr)<<":"<<ClientAddr.sin_port<<endl;

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

