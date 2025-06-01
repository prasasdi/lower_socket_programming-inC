#include <winsock2.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

int main() {
    WSADATA wsaData;
    SOCKET udpSocket;
    struct sockaddr_in serverAddr, clientAddr;
    char buffer[1024];
    int clientAddrLen, bytesReceived;

    // Init Winsock
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        printf("WSAStartup failed.\n");
        return 1;
    }

    // Create socket
    udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udpSocket == INVALID_SOCKET) {
        printf("Socket creation failed.\n");
        WSACleanup();
        return 1;
    }

    // Bind
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(12345);

    if (bind(udpSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Bind failed.\n");
        closesocket(udpSocket);
        WSACleanup();
        return 1;
    }

    printf("UDP Echo Server listening on port 12345...\n");

    // Receive and echo
    while (1) {
        clientAddrLen = sizeof(clientAddr);
        bytesReceived = recvfrom(udpSocket, buffer, sizeof(buffer), 0,
                                 (SOCKADDR*)&clientAddr, &clientAddrLen);
        if (bytesReceived == SOCKET_ERROR) {
            printf("recvfrom failed.\n");
            break;
        }
        sendto(udpSocket, buffer, bytesReceived, 0,
               (SOCKADDR*)&clientAddr, clientAddrLen);
    }

    // Cleanup
    closesocket(udpSocket);
    WSACleanup();
    return 0;
}
