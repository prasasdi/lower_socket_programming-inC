#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

int main() {
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    char buffer[1024];
    int clientAddrLen, bytesReceived;

    printf("[*] Starting WSA...\n");
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        printf("[!] WSAStartup failed\n");
        return 1;
    }

    printf("[*] Creating socket...\n");
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        printf("[!] socket() failed: %d\n", WSAGetLastError());
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(12345);

    printf("[*] Binding...\n");
    if (bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("[!] bind() failed: %d\n", WSAGetLastError());
        return 1;
    }

    printf("[*] Listening on port 12345...\n");
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        printf("[!] listen() failed: %d\n", WSAGetLastError());
        return 1;
    }

    while (1) {
        printf("[*] Waiting for client...\n");
        clientAddrLen = sizeof(clientAddr);
        clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddr, &clientAddrLen);
        if (clientSocket == INVALID_SOCKET) {
            printf("[!] accept() failed: %d\n", WSAGetLastError());
            continue;
        }

        printf("[+] Client connected!\n");

        while ((bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
            printf("[>] Received %d bytes: %.*s\n", bytesReceived, bytesReceived, buffer);
            send(clientSocket, buffer, bytesReceived, 0);
        }

        if (bytesReceived == 0) {
            printf("[*] Client disconnected\n");
        } else {
            printf("[!] recv() failed: %d\n", WSAGetLastError());
        }

        closesocket(clientSocket);
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
