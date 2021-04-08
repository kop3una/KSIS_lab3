#include "pch.h"
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#pragma comment(lib, "ws2_32.lib")
#include <Windows.h>
#include <conio.h>
#include <time.h>

using namespace std;

#define ECHO_PORT 7 

DWORD WINAPI WorkWithClient(LPVOID client_socket);

DWORD WINAPI CheckCommonBuffer(LPVOID client_socket);

int nclients = 0;

int main() {
	//system("chcp 437");
	char buff[1024];

	printf("ECHO TCP-Server\n");

	if (WSAStartup(0x0202, (WSADATA*)&buff[0])) {
		printf("Error WSAStartup %d\n", WSAGetLastError());
		return -1;
	}

	SOCKET mysocket;

	if ((mysocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		// Ошибка!
		printf("Error socket %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(ECHO_PORT);
	local_addr.sin_addr.s_addr = 0;

	if (bind(mysocket, (sockaddr*)&local_addr, sizeof(local_addr))) {
		printf("Error bind %d\n", WSAGetLastError());
		closesocket(mysocket);  // закрываем сокет!
		WSACleanup();
		return -1;
	}

	if (listen(mysocket, 20)) {
		// Ошибка
		printf("Error listen %d\n", WSAGetLastError());
		closesocket(mysocket);
		WSACleanup();
		return -1;
	}

	printf("Waiting for connection\n");

	SOCKET client_socket[20];
	sockaddr_in client_addr;

	int client_addr_size = sizeof(client_addr);

	while ((client_socket[nclients] = accept(mysocket, (sockaddr*)&client_addr, &client_addr_size))) {
		nclients++;

		HOSTENT* hst;
		hst = gethostbyaddr((char*)&client_addr.sin_addr.s_addr, 4, AF_INET); // пытаемся получить имя хоста
		printf("+%s [%s] new connection\n", (hst) ? hst->h_name : "", inet_ntoa(client_addr.sin_addr));
		DWORD thID;

		CreateThread(NULL, NULL, WorkWithClient, client_socket, NULL, &thID);
	}
	return 0;
}

DWORD WINAPI WorkWithClient(LPVOID sockets) {
	SOCKET my_sock;
	my_sock = ((SOCKET*)sockets)[nclients - 1];
	char buff[1024] = { 0 };

#define sHELLO "Send Message...\r\n"
	//printf(" \n new thread is started connect!\n");

	// отправляем клиенту приветствие 
	send(my_sock, sHELLO, sizeof(sHELLO), 0);

	// цикл эхо-сервера: прием строки от клиента и
	int bytes_recv;
	// возвращение ее клиенту
	while (1) {
		if ((bytes_recv = recv(my_sock, buff, sizeof(buff), 0)) != SOCKET_ERROR) {
			buff[bytes_recv] = 0;
			SOCKET temp;
			temp = ((SOCKET*)sockets)[nclients - 1];
			send(temp, buff, bytes_recv, 0);
		}
		else break;
	}

	nclients--;
	closesocket(my_sock);
	return 0;
}
