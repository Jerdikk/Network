// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"



// Пример простого TCP-клиента
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#include "Packet.h"

#define PORT 666
#define SERVERADDR "127.0.0.1" //Звёздочками пометил свой IP



int main(int argc, char* argv[])
{
	char buff[1024];
	printf("TCP DEMO CLIENT\n");

	// Шаг 1 - инициализация библиотеки Winsock
	if (WSAStartup(0x202, (WSADATA *)&buff[0]))
	{
		printf("WSAStart error %d\n", WSAGetLastError());
		return -1;
	}


	MYPACKET* mp = new MYPACKET();

	// Шаг 2 - создание сокета
	SOCKET my_sock;
	my_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (my_sock < 0)
	{
		printf("Socket() error %d\n", WSAGetLastError());
		return -1;
	}

	// Шаг 3 - установка соединения
	// заполнение структуры sockaddr_in - указание адреса и порта сервера
	sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(PORT);
	HOSTENT *hst;

	in_addr dstt;

	// преобразование IP адреса из символьного в сетевой формат
	//if (inet_addr(SERVERADDR) != INADDR_NONE)
	if (inet_pton(AF_INET, SERVERADDR, &dstt) == 1)
	{
		dest_addr.sin_addr.s_addr = dstt.S_un.S_addr;//inet_addr(SERVERADDR);
	}	
	else
	{
		// попытка получить IP адрес по доменному имени сервера
		//if (hst = gethostbyname(SERVERADDR))
		int status;
		struct addrinfo hints, *res, *p;
		char ipstr[INET6_ADDRSTRLEN];
		//struct addrinfo *servinfo;  // указатель на результаты

		memset(&hints, 0, sizeof hints); // убедимся, что структура пуста
		hints.ai_family = AF_UNSPEC;     // неважно, IPv4 или IPv6
		hints.ai_socktype = SOCK_STREAM; // TCP stream-sockets
		hints.ai_flags = AI_PASSIVE;     // заполните мой IP-адрес за меня

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


			// получаем указатель на адрес, по разному в разных протоколах
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

			// преобразуем IP в строку и выводим его:
			inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
			printf("  %s: %sn", ipver, ipstr);
		}

		freeaddrinfo(res); // free the linked list



			// hst->h_addr_list содержит не массив адресов,
			// а массив указателей на адреса
/*			((unsigned long *)&dest_addr.sin_addr)[0] =
			((unsigned long **)hst->h_addr_list)[0][0];
		else
		{
			printf("Invalid address %s\n", SERVERADDR);
			return -1;
		}*/
	}

	// адрес сервера получен - пытаемся установить соединение
	if (connect(my_sock, (sockaddr *)&dest_addr, sizeof(dest_addr)))
	{
		printf("Connect error %d\n", WSAGetLastError());
		return -1;
	}

	printf("Soedinenie s %s uspeshno ustanovlenno\n \
            Type quit for quit\n\n", SERVERADDR);
	
	// Шаг 4 - чтение и передача сообщений
	int nsize;
	while ((nsize = recv(my_sock, &buff[0], sizeof(buff) - 1, 0)) != SOCKET_ERROR)
	{
		// ставим завершающий ноль в конце строки
		buff[nsize] = 0;

		


		// выводим на экран
		printf("S=>C:%s", buff);

		// читаем пользовательский ввод с клавиатуры
		printf("S<=C:"); fgets(&buff[0], sizeof(buff) - 1, stdin);

		

		// проверка на "quit"
		if (!strcmp(&buff[0], "quit\n"))
		{
			// Корректный выход
			printf("Exit...");
			closesocket(my_sock);
			WSACleanup();
			return 0;
		}
		delete mp;
		mp = pack(buff);
		// передаем строку клиента серверу
		//send(my_sock, &buff[0], strlen(&buff[0]), 0);
		send(my_sock, &mp->r.buffer[0], packetSize(mp), 0);
	}
	printf("Recv error %d\n", WSAGetLastError());
	closesocket(my_sock);
	WSACleanup();
	return -1;
}