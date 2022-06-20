// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"



// ������ �������� TCP-�������
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#include "Packet.h"

#define PORT 666
#define SERVERADDR "127.0.0.1" //���������� ������� ���� IP



int main(int argc, char* argv[])
{
	char buff[1024];
	printf("TCP DEMO CLIENT\n");

	// ��� 1 - ������������� ���������� Winsock
	if (WSAStartup(0x202, (WSADATA *)&buff[0]))
	{
		printf("WSAStart error %d\n", WSAGetLastError());
		return -1;
	}


	MYPACKET* mp = new MYPACKET();

	// ��� 2 - �������� ������
	SOCKET my_sock;
	my_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (my_sock < 0)
	{
		printf("Socket() error %d\n", WSAGetLastError());
		return -1;
	}

	// ��� 3 - ��������� ����������
	// ���������� ��������� sockaddr_in - �������� ������ � ����� �������
	sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(PORT);
	HOSTENT *hst;

	in_addr dstt;

	// �������������� IP ������ �� ����������� � ������� ������
	//if (inet_addr(SERVERADDR) != INADDR_NONE)
	if (inet_pton(AF_INET, SERVERADDR, &dstt) == 1)
	{
		dest_addr.sin_addr.s_addr = dstt.S_un.S_addr;//inet_addr(SERVERADDR);
	}	
	else
	{
		// ������� �������� IP ����� �� ��������� ����� �������
		//if (hst = gethostbyname(SERVERADDR))
		int status;
		struct addrinfo hints, *res, *p;
		char ipstr[INET6_ADDRSTRLEN];
		//struct addrinfo *servinfo;  // ��������� �� ����������

		memset(&hints, 0, sizeof hints); // ��������, ��� ��������� �����
		hints.ai_family = AF_UNSPEC;     // �������, IPv4 ��� IPv6
		hints.ai_socktype = SOCK_STREAM; // TCP stream-sockets
		hints.ai_flags = AI_PASSIVE;     // ��������� ��� IP-����� �� ����

		if ((status = getaddrinfo(SERVERADDR, "666", &hints, &res)) != 0) {
			fprintf(stderr, "getaddrinfo error: %sn", gai_strerror(status));
			closesocket(my_sock);
			WSACleanup();
			exit(1);
		}

		printf("IP addresses for %s:nn", argv[1]);

		for (p = res; p != NULL; p = p->ai_next) {
			void *addr;
			char *ipver;


			// �������� ��������� �� �����, �� ������� � ������ ����������
			if (p->ai_family == AF_INET) { // IPv4
				struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
				addr = &(ipv4->sin_addr);
				ipver = "IPv4";
			}
			else { // IPv6
				struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
				addr = &(ipv6->sin6_addr);
				ipver = "IPv6";
			}

			// ����������� IP � ������ � ������� ���:
			inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
			printf("  %s: %sn", ipver, ipstr);
		}

		freeaddrinfo(res); // free the linked list



			// hst->h_addr_list �������� �� ������ �������,
			// � ������ ���������� �� ������
/*			((unsigned long *)&dest_addr.sin_addr)[0] =
			((unsigned long **)hst->h_addr_list)[0][0];
		else
		{
			printf("Invalid address %s\n", SERVERADDR);
			return -1;
		}*/
	}

	// ����� ������� ������� - �������� ���������� ����������
	if (connect(my_sock, (sockaddr *)&dest_addr, sizeof(dest_addr)))
	{
		printf("Connect error %d\n", WSAGetLastError());
		return -1;
	}

	printf("Soedinenie s %s uspeshno ustanovlenno\n \
            Type quit for quit\n\n", SERVERADDR);
	
	// ��� 4 - ������ � �������� ���������
	int nsize;
	while ((nsize = recv(my_sock, &buff[0], sizeof(buff) - 1, 0)) != SOCKET_ERROR)
	{
		// ������ ����������� ���� � ����� ������
		buff[nsize] = 0;

		


		// ������� �� �����
		printf("S=>C:%s", buff);

		// ������ ���������������� ���� � ����������
		printf("S<=C:"); fgets(&buff[0], sizeof(buff) - 1, stdin);

		

		// �������� �� "quit"
		if (!strcmp(&buff[0], "quit\n"))
		{
			// ���������� �����
			printf("Exit...");
			closesocket(my_sock);
			WSACleanup();
			return 0;
		}
		delete mp;
		mp = pack(buff);
		// �������� ������ ������� �������
		//send(my_sock, &buff[0], strlen(&buff[0]), 0);
		send(my_sock, &mp->r.buffer[0], packetSize(mp), 0);
	}
	printf("Recv error %d\n", WSAGetLastError());
	closesocket(my_sock);
	WSACleanup();
	return -1;
}