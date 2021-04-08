// ECHO-UDP-SERVER.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <winsock.h>
#pragma comment( lib, "ws2_32.lib" )
#define _CRT_SECURE_NO_WARNINGS
#define MY_PORT 1337

int main() {
	int errorCode;
	int clientSize;
	int localSize;
	char Buf[1024];
	int BufSize;
	WORD versionRequested;
	WSADATA wsaData;
	SOCKET serverSocket;
	SOCKADDR_IN saClient, saLocal;
	HOSTENT* hostent;
	printf_s("%s", "ECHO UDP SERVER\n");
	versionRequested = MAKEWORD(2, 2);
	//Шаг 1. Инициализируем библиотеку Winsock
	errorCode = WSAStartup(versionRequested, &wsaData);
	if (errorCode != 0) {
		printf("Sorry, it's not available to find a usable Winsock DLL\n");
		return 1;
	}
	if ((LOBYTE(wsaData.wVersion) != 2) || (HIBYTE(wsaData.wVersion) != 2)) {
		printf("Sorry, it's not available to find a usable Winsock DLL\n");
		return 1;
	}
	//Шаг 2. Создаем сокет
	//AF-INET - сокет интернета
	//SOCK-DGRAM - дейтаграммный сокет, не ориентированный на соединение
	//IPROTO-UDP - прокол UDP
	serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (serverSocket == INVALID_SOCKET) {
		printf("socket failed with error = %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	else {
		//Шаг 3. Связываем сокет с локальным адресом
		saLocal.sin_family = AF_INET;
		saLocal.sin_port = htons(MY_PORT);
		saLocal.sin_addr.s_addr = INADDR_ANY; //сервер принимает подключения  со всех IP-адресов
		localSize = sizeof(saLocal);
		//вызываем bind для связывания
		errorCode = bind(serverSocket, (sockaddr*)&saLocal, sizeof(saLocal));
		if (errorCode != 0) {
			printf("bind failed with error = %d\n", WSAGetLastError());
			closesocket(serverSocket);
			WSACleanup();
			return 1;
		}
		while (1) {
			//Шаг 4. Ожидаем подключения от клиента и получаем сообщения
			clientSize = sizeof(saClient);
			BufSize = recvfrom(serverSocket, &Buf[0], sizeof(Buf) - 1, 0, (sockaddr*)&saClient, &clientSize);
			if (BufSize == SOCKET_ERROR) {
				printf("recvfrom failed with error = %d\n", WSAGetLastError());
				closesocket(serverSocket);
				break;
			}
			else {
				//выводим сведения о подключившемся клиенте
				hostent = gethostbyaddr((char*)&saClient.sin_addr, 4, AF_INET);
				printf("accepted connection from %s, port %d\n", inet_ntoa(saClient.sin_addr), ntohs(saClient.sin_port));
				Buf[BufSize] = 0;  //добавляем завершающий ноль в конце для вывода на экран
				//printf("C=>S:%s\n", &Buf[0]);
			}
			sendto(serverSocket, &Buf[0], BufSize, 0, (sockaddr*)&saClient, sizeof(saClient));
		}
	}
	//Шаг 5. Закрываем сокет и деинициализируем Winsock
	errorCode = closesocket(serverSocket);
	if (errorCode == SOCKET_ERROR) {
		printf("closesocket failed with error = %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	WSACleanup();
	return 0;
}

