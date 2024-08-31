// mci.cpp : 定义应用程序的入口点。
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

// 当服务端关闭，系统自动从这里开始重复连接。
restart:
	thisHost = NULL;
	ip = NULL;

	//初始化 Windows Socket
	if ( WSAStartup(MAKEWORD(2,2), &Ws) != 0 )
	{
		//初始化Socket失败
		return -1;
	}

	//创建 Socket
	CientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if ( CientSocket == INVALID_SOCKET )
	{
		// 创建socket失败
		return -1;
	}

	//根据主机名获得ip地址
	
	thisHost	=	gethostbyname("Sun-PC");
	ip	=	inet_ntoa(*(struct in_addr *)*(thisHost->h_addr_list)); 

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
		// 连接失败重新连接
	}

	HANDLE hThread = CreateThread(NULL, 0, ShutUp, (LPVOID)CientSocket, 0, NULL);
	if ( hThread == NULL )
	{
		// 创建控音线程失败
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
			// 服务器关闭
			closesocket(CientSocket);
			WSACleanup();
			goto restart;
		}
		// 这里收到服务器的消息 进行处理。
		if(0 == strcmp(RecvBuffer,"闭嘴"))
		{
			b = 1;
		}

		if(0 == strcmp(RecvBuffer,"出声"))
		{
			b = 0;
		}

		if(0 == strcmp(RecvBuffer,"关机"))
		{
			system("shutdown -s");
		}

	}

end:
	closesocket(CientSocket);
	WSACleanup();

	return (int) 0;
}