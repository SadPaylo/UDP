#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")
using namespace std;

#define BUFSIZE         2048    //buffer
#define SERVER_PORT     8008   //server port #

int main(void)
{
	// Инициализируем WinSocket

	int rc;
	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0)
	{
		cerr << "WSAStartup failed, Err #" << wsResult << endl;
		cin.get();
		return 0;
	}

	//Срздание сокета
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET)
	{
		cerr << "Can't create a socket, Err #" << WSAGetLastError() << endl;
		cin.get();
		WSACleanup();
		return 0;
	}

	int OptVal = 1;
	int OptLen = sizeof(int);
	rc = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&OptVal, OptLen);
	if (rc == SOCKET_ERROR)
	{
		cerr << "Can't set a sockopt, Err #" << WSAGetLastError() << endl;
		cin.get();
		WSACleanup();
		return 0;
	}
	else
		cout << "Socket option SO_REUSEADDR is ON" << endl;

	// Заполняем структуру подсказок сервера
	struct sockaddr_in server_addr;
	int server_len = sizeof(server_addr);
	ZeroMemory(&server_addr, server_len);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;

	rc = bind(sock, (struct sockaddr*) & server_addr, server_len);
	if (rc == SOCKET_ERROR)
	{
		cerr << "Can't bind a socket, Err #" << WSAGetLastError() << endl;
		cin.get();
		WSACleanup();
		return 0;
	}

	struct sockaddr_in client_addr;
	int client_len = sizeof(client_addr);
	ZeroMemory(&client_addr, client_len);


	// Do-While Loop для отправки и получения данных
	char buf[BUFSIZE];
	string userInput;

	while (true)
	{
		ZeroMemory(buf, BUFSIZE);


		// Ждем сообщения
		int bytesRecieved = recvfrom(sock, buf, BUFSIZE, 0, (struct sockaddr*) & client_addr, &client_len);
		if (bytesRecieved == SOCKET_ERROR)
		{
			cerr << "-----------------------" << endl;
			continue;
		}


		// Показать информацию
		char clientIP[256];
		ZeroMemory(clientIP, 256);

		inet_ntop(AF_INET, &client_addr.sin_addr, clientIP, 256);

		cerr << "Msg from " << clientIP << " : " << ntohs(client_addr.sin_port) << " $ " << buf << endl;

		int sendResult = sendto(sock, buf, BUFSIZE, 0, (sockaddr*)&client_addr, client_len);
		if (sendResult == SOCKET_ERROR)
		{
			cerr << "Can't send msg, Err #" << WSAGetLastError() << endl;
		}
	}


	// Завершаем соединение
	cin.get();
	closesocket(sock);
	WSACleanup();
	return 0;
}
