#ifndef UTILITY_H
#define UTILITY_H
#include <WinSock2.h>
#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 1024

#define SOCKADDR_SIZE sizeof(SOCKADDR_IN)

#define HANDSHAKE_REQUEST "HELLO"

#define HANDSHAKE_REQUEST_LENGTH strlen(HANDSHAKE_REQUEST)

#define HANDSHAKE_RESPONSE "HI"

#define PREFIX "LOCALHOST: "

#define HANDSHAKE_RESPONSE_LENGTH strlen(HANDSHAKE_RESPONSE)

static inline SOCKADDR_IN *getAddress(char *ip, int port) {
	SOCKADDR_IN *address = (SOCKADDR_IN *)calloc(1, sizeof(SOCKADDR_IN));

	address->sin_family = AF_INET;
	address->sin_addr.s_addr = ip == NULL ? INADDR_ANY : inet_addr(ip);
	address->sin_port = htons(port);

	return address;
}

static inline SOCKET getSocket() {
	return socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

typedef struct {
	SOCKET socket;
	char* buffer;
	char* ip;
} Target;

DWORD WINAPI receive(Target* target) {
	int bufferLength;

	while(1) {
		bufferLength = recv(target->socket, target->buffer, BUFFER_SIZE, 0);

		switch(bufferLength) {
			case 0: {
				puts("FINISH");

				break;
			}

			case SOCKET_ERROR: {
				printf("\nRECV_ERROR(%d)", WSAGetLastError());

				exit(1);

				break;
			}

			default: {
				target->buffer[bufferLength] = '\0';

				printf("\n\033[A\033[K%s: %s", target->ip, target->buffer);

				fputs(PREFIX, stdout);

				break;
			}
		}
	}
}

#endif