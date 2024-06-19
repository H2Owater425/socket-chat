#include <WinSock2.h>
#include <Windows.h>
#include <wchar.h>
#include <process.h>
#include "utility.h"

#pragma comment(lib, "ws2_32.lib")

int main(int argumentCount, char *arguments[]) {
	WSADATA *wsaData = (WSADATA *)malloc(sizeof(WSADATA));

	if(WSAStartup(MAKEWORD(2, 2), wsaData) == 0) {
		free(wsaData);

		char buffer[BUFFER_SIZE + 1] = {0};

		switch(argumentCount) {
			case 2: {
				SOCKET serverSocket = getSocket();
				SOCKADDR_IN *serverAddress = getAddress(NULL, atoi(arguments[1]));
				int addressSize = SOCKADDR_SIZE;

				if(serverSocket != INVALID_SOCKET) {
					if(bind(serverSocket, (SOCKADDR *)serverAddress, addressSize) != SOCKET_ERROR) {
						if(listen(serverSocket, SOMAXCONN) != SOCKET_ERROR) {
							puts("ACCEPTING");

							SOCKADDR_IN *clientAddress = (SOCKADDR_IN *)malloc(addressSize);
							SOCKET clientSocket = accept(serverSocket, (SOCKADDR *)clientAddress, &addressSize);

							if(clientSocket != INVALID_SOCKET) {
								if(recv(clientSocket, buffer, BUFFER_SIZE, 0) == HANDSHAKE_REQUEST_LENGTH && strncmp(buffer, HANDSHAKE_REQUEST, HANDSHAKE_REQUEST_LENGTH) == 0) {
									send(clientSocket, HANDSHAKE_RESPONSE, HANDSHAKE_RESPONSE_LENGTH, 0);

									memset(buffer, 0, BUFFER_SIZE + 1);

									Target target = {
										clientSocket,
										buffer,
										inet_ntoa(clientAddress->sin_addr)
									};

									printf("ACCEPTED(%s)\n", target.ip);

									if(CloseHandle((HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)receive, &target, 0, NULL)) != 0) {
										while(1) {
											fputs(PREFIX, stdout);
											fgets(buffer, 1024, stdin);

											send(clientSocket, buffer, strlen(buffer), 0);
										}

										return 0;
									} else {
										printf("THREAD_ERROR(%d)", GetLastError());
									}
								} else {
									puts("HANDSHAKE_ERROR");
								}
							} else {
								printf("CLIENT_SOCKET_ERROR(%d)", WSAGetLastError());
							}

							closesocket(clientSocket);
							free(clientAddress);
						} else {
							printf("LISTEN_ERROR(%d)", WSAGetLastError());
						}
					} else {
						printf("BIND_ERROR(%d)", WSAGetLastError());
					}

					closesocket(serverSocket);
				} else {
					printf("SERVER_SOCKET_ERROR(%d)", WSAGetLastError());
				}

        WSACleanup();
				free(serverAddress);

				return 1;
			}

			case 3: {
				SOCKET serverSocket = getSocket();

				if(serverSocket != INVALID_SOCKET) {
					SOCKADDR_IN *serverAddress = getAddress(arguments[1], atoi(arguments[2]));

					puts("CONNECTING");

					if(connect(serverSocket, (SOCKADDR *)serverAddress, SOCKADDR_SIZE) != SOCKET_ERROR) {
						send(serverSocket, HANDSHAKE_REQUEST, HANDSHAKE_REQUEST_LENGTH, 0);

						if(recv(serverSocket, buffer, BUFFER_SIZE, 0) == HANDSHAKE_RESPONSE_LENGTH && strncmp(buffer, HANDSHAKE_RESPONSE, HANDSHAKE_RESPONSE_LENGTH) == 0) {
							memset(buffer, 0, BUFFER_SIZE + 1);

							Target target = {
								serverSocket,
								buffer,
								inet_ntoa(serverAddress->sin_addr)
							};

							printf("CONNECTED(%s)\n", target.ip);

							if(CloseHandle((HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)receive, &target, 0, NULL)) != 0) {
								while(1) {
									fputs(PREFIX, stdout);
									fgets(buffer, 1024, stdin);

									send(serverSocket, buffer, strlen(buffer), 0);
								}

								return 0;
							} else {
								printf("THREAD_ERROR(%d)", GetLastError());
							}
						} else {
							puts("HANDSHAKE_ERROR");
						}
					} else {
						printf("CONNECT_ERROR(%d)", WSAGetLastError());
					}

					closesocket(serverSocket);
					free(serverAddress);
				} else {
					printf("SERVER_SOCKET_ERROR(%d)", WSAGetLastError());
				}

        WSACleanup();

				return 1;
			}

			default: {
				puts("Usage: chat [ip] <port>\n\nOptions:\n  <port>             port number\n  ip                 ip address, use to behave as client");

				return 1;
			}
		}
	} else {
		printf("WSASTARTUP_ERROR(%d)", WSAGetLastError());

		return 1;
	}
}