// Win32

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

int main(void) {
    WSADATA wsa;
    SOCKET server, client;
    struct sockaddr_in server_addr, client_addr;
    int client_size;
    char buffer[1024];

    // Init Winsock
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }

    // Create socket
    server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == INVALID_SOCKET) {
        printf("Socket creation failed: %d\n", WSAGetLastError());
        return 1;
    }

    // Prepare server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // listen on all interfaces
    server_addr.sin_port = htons(8080);

    // Bind
    if (bind(server, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed: %d\n", WSAGetLastError());
        closesocket(server);
        WSACleanup();
        return 1;
    }

    // Listen
    listen(server, SOMAXCONN);
    printf("Server listening on port 8080...\n");

    // Accept one connection
    client_size = sizeof(client_addr);
    client = accept(server, (struct sockaddr*)&client_addr, &client_size);
    if (client == INVALID_SOCKET) {
        printf("Accept failed: %d\n", WSAGetLastError());
        closesocket(server);
        WSACleanup();
        return 1;
    }

    // Receive data
    int bytes_received = recv(client, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0'; // Null-terminate string
        printf("Received: %s\n", buffer);
    }

    // Close sockets
    closesocket(client);
    closesocket(server);
    WSACleanup();
    return 0;
}
