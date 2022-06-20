// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


// ������ �������� TCP-���-�������

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <windows.h>
#include "Packet.h"
// link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#define MY_PORT 666 // ����, ������� ������� ������ 666

// ������ ��� ������ ���������� �������� �������������
#define PRINTNUSERS if (nclients) printf("%d user on-line\n", nclients); \
        else printf("No User on line\n");

// �������� �������, ������������� �������������� �������������
DWORD WINAPI SexToClient(LPVOID client_socket);




// ���������� ���������� - ���������� �������� �������������
int nclients = 0;

int main(int argc, char* argv[])
{
	char buff[1024]; // ����� ��� ��������� ����

	

	printf("TCP SERVER DEMO\n");
	// ��� 1 - ������������� ���������� �������
	// �.�. ������������ �������� ���������� �� ������������
	// �� ���������� ��������� �� ������� �����, ������������� � ���������
	// �� ��������� WSADATA.
	// ����� ����� ��������� ���������� ���� ����������, ������, �����
	// ������ ���� �� ����� ������������ �������� (��������� WSADATA
	// �������� 400 ����)
	if (WSAStartup(0x0202, (WSADATA *)&buff[0]))
	{
		// ������!
		printf("Error WSAStartup %d\n", WSAGetLastError());
		return -1;
	}

	// ��� 2 - �������� ������
	SOCKET mysocket;
	// AF_INET - ����� ���������
	// SOCK_STREAM - ��������� ����� (� ���������� ����������)
	// 0 - �� ��������� ���������� TCP ��������
	if ((mysocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		// ������!
		printf("Error socket %d\n", WSAGetLastError());
		WSACleanup(); // �������������� ���������� Winsock
		return -1;
	}

	// ��� 3 - ���������� ������ � ��������� �������
	sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(MY_PORT); // �� �������� � ������� �������!!!
	local_addr.sin_addr.s_addr = 0; // ������ ��������� �����������
									// �� ��� ���� IP-������

									// �������� bind ��� ����������
	if (bind(mysocket, (sockaddr *)&local_addr, sizeof(local_addr)))
	{
		// ������
		printf("Error bind %d\n", WSAGetLastError());
		closesocket(mysocket); // ��������� �����!
		WSACleanup();
		return -1;
	}

	// ��� 4 - �������� �����������
	// ������ ������� - 0x100
	if (listen(mysocket, 0x100))
	{
		// ������
		printf("Error listen %d\n", WSAGetLastError());
		closesocket(mysocket);
		WSACleanup();
		return -1;
	}

	printf("ozidanie podkluceniy...\n");

	// ��� 5 - ��������� ��������� �� �������
	SOCKET client_socket; // ����� ��� �������
	sockaddr_in client_addr; // ����� ������� (����������� ��������)

							 // ������� accept ���������� �������� ������ ���������
	int client_addr_size = sizeof(client_addr);

	// ���� ���������� �������� �� ����������� �� �������
	while ((client_socket = accept(mysocket, (sockaddr *)&client_addr, &client_addr_size)))
	{
		nclients++; // ����������� ������� �������������� ��������

					// �������� �������� ��� �����
		HOSTENT *hst;
	//	hst = gethostbyaddr((char *)&client_addr.sin_addr.s_addr, 4, AF_INET);

		//struct sockaddr_in saGNI;
		char hostname[NI_MAXHOST];
		char servInfo[NI_MAXSERV];
		u_short port = 27015;

		// Call getnameinfo
		DWORD dwRetval = getnameinfo((struct sockaddr *) &client_addr,
			sizeof(struct sockaddr),
			hostname,
			NI_MAXHOST, servInfo, NI_MAXSERV, NI_NUMERICSERV);

		if (dwRetval != 0) {
			printf("getnameinfo failed with error # %ld\n", WSAGetLastError());
			return 1;
		}
		else {
			printf("getnameinfo returned hostname = %s\n", hostname);
			//return 0;
		}


		// ����� �������� � �������
		printf("+%s [%s] new connect!\n",
			/*(hst) ? hst->h_name : ""*/  "1", hostname /*inet_ntoa(client_addr.sin_addr)*/);
		PRINTNUSERS

			// ����� ������ ������ ��� ����������� �������
			// ��, ��� ����� ������������� ������������ _beginthreadex
			// ��, ��������� ������� ������� ������� ����������� �� ����������
			// ����� �� ������, ����� �������� � CreateThread
			DWORD thID;
		CreateThread(NULL, NULL, SexToClient, &client_socket, NULL, &thID);
	}
	return 0;
}

// ��� ������� ��������� � ��������� ������
// � ���������� ���������� ��������������� ������� ���������� �� ���������
DWORD WINAPI SexToClient(LPVOID client_socket)
{
	SOCKET my_sock;
	my_sock = ((SOCKET *)client_socket)[0];
	char buff[20 * 1024] = {0};
#define sHELLO "SOCKET PODKLUCHEN\r\n"

	// ���������� ������� �����������
	send(my_sock, sHELLO, sizeof(sHELLO), 0);

	char text[257];
	// ���� ���-�������: ����� ������ �� ������� � ����������� �� �������
	int bytes_recv;
	while ((bytes_recv = recv(my_sock, &buff[0], sizeof(buff), 0)) &&
		bytes_recv != SOCKET_ERROR)
	{
		buff[bytes_recv + 1] = 0;
		decode((MYPACKET*)&buff[0], &text[0]);
		
	//	printf("%s", buff);
		printf("%s", text);


		send(my_sock, &buff[0], bytes_recv, 0);		
	}
	

	// ���� �� �����, �� ��������� ����� �� ����� �� �������
	// ���������� �������� recv ������ - ���������� � �������� ���������
	nclients--; // ��������� ������� �������� ��������
	printf("-disconnect\n"); PRINTNUSERS

		// ��������� �����
		closesocket(my_sock);
	return 0;
}


