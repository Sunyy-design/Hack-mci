// mcic.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>
#include <Windows.h>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

#define  PORT 9000

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
	WSADATA  Ws;
	SOCKET CientSocket;
	struct sockaddr_in ServerAddr;
	int Ret = 0;
	int AddrLen = 0;
	HANDLE hThreadSed = NULL;


	//初始化 Windows Socket
	if ( WSAStartup(MAKEWORD(2,2), &Ws) != 0 )
	{
		cout<<"Init Windows Socket Failed::"<<GetLastError()<<endl;
		return -1;
	}

	//创建 Socket
	CientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if ( CientSocket == INVALID_SOCKET )
	{
		cout<<"Create Socket Failed::"<<GetLastError()<<endl;
		return -1;
	}

	//根据主机名获得ip地址
	hostent	*thisHost;
	thisHost	=	gethostbyname("Sun-PC");
	char *ip	=	inet_ntoa(*(struct in_addr *)*(thisHost->h_addr_list)); 

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.s_addr = inet_addr(ip);
	ServerAddr.sin_port = htons(PORT);
	memset(ServerAddr.sin_zero, 0x00, 8);

	//请求连接
	Ret = connect(CientSocket,(struct sockaddr*)&ServerAddr, sizeof(ServerAddr));
	//如果不成功就一直请求
	while( Ret == SOCKET_ERROR )
	{
		Ret = connect(CientSocket,(struct sockaddr*)&ServerAddr, sizeof(ServerAddr));
		cout<<"Connect Error::"<<GetLastError()<<endl;
	}

	cout<<"连接成功!"<<endl;


	// 创建发送线程
	hThreadSed = CreateThread(NULL, 0, ClientThreadRSend, (LPVOID)CientSocket, 0, NULL);
	if ( hThreadSed == NULL )
	{
		cout<<"Create ThreadSed Failed!"<<endl;
		goto end;
	}

	// 接收循环
	char RecvBuffer[MAX_PATH];
	while ( true )
	{
		memset(RecvBuffer, 0x00, sizeof(RecvBuffer));
		Ret = recv(CientSocket, RecvBuffer, MAX_PATH, 0);
		if ( Ret == 0 || Ret == SOCKET_ERROR ) 
		{
			cout<<"服务端关闭!"<<endl;
			break;
		}
		cout<<"server:"<<RecvBuffer<<endl;
	}

	end:
	CloseHandle(hThreadSed);

	closesocket(CientSocket);
	WSACleanup();
	return 0;
}

