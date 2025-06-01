// Win32

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

int main(void) {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;
    char *message = "Halo dari client!";
    char buffer[1024];
    int bytes_received;

    // Init Winsock
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("WSAStartup gagal: %d\n", WSAGetLastError());
        return 1;
    }

    // Buat socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("Socket gagal dibuat: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Setup alamat server
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect ke server
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        printf("Koneksi gagal: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    printf("Terkoneksi ke server.\n");

    // Kirim pesan
    if (send(sock, message, strlen(message), 0) < 0) {
        printf("Gagal mengirim: %d\n", WSAGetLastError());
    } else {
        printf("Pesan terkirim.\n");
    }

    // Terima respon (jika ada)
    bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("Dari server: %s\n", buffer);
    }

    // Tutup koneksi
    closesocket(sock);
    WSACleanup();
    return 0;
}
