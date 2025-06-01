#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

// Flag global untuk loop berjalan atau tidak
volatile BOOL running = TRUE;

// Handler untuk menangani Ctrl+C di console Windows
BOOL WINAPI ConsoleHandler(DWORD signal) {
    if (signal == CTRL_C_EVENT) {
        printf("\n[!] Ctrl+C detected, exiting gracefully...\n");
        running = FALSE;  // Set flag jadi FALSE supaya loop berhenti
    }
    return TRUE;
}

int main(void) {
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    char buffer[1024];
    int clientAddrLen, bytesReceived;

    // Tambahan untuk select()
    fd_set masterSet, readSet;
    SOCKET maxSocket;

    // Daftarkan console ctrl handler untuk graceful exit
    SetConsoleCtrlHandler(ConsoleHandler, TRUE);

    printf("[*] Starting WSA...\n");
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        printf("[!] WSAStartup failed\n");
        return 1;
    }

    printf("[*] Creating socket...\n");
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        printf("[!] socket() failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(12345);

    printf("[*] Binding...\n");
    if (bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("[!] bind() failed: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    printf("[*] Listening on port 12345...\n");
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        printf("[!] listen() failed: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Inisialisasi fd_set
    FD_ZERO(&masterSet);
    FD_SET(serverSocket, &masterSet);
    maxSocket = serverSocket;

    // Loop utama pakai select()
    while (running) {
        printf("[*] Waiting for client...\n");

        // Salin masterSet ke readSet sebelum dipakai select()
        readSet = masterSet;

        struct timeval timeout; // timeout 5 detik
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        int ret = select(0, &readSet, NULL, NULL, &timeout);

        if (ret == SOCKET_ERROR) {
            printf("[!] select() failed: %d\n", WSAGetLastError());
            break;
        } else if (ret == 0) {
            // Timeout terjadi, gak ada socket yang ready selama 5 detik
            printf("[*] select() timeout: tidak ada aktivitas\n");
            continue;  // bisa lanjut loop lagi, atau handle timeout khusus
        }

        // Cek tiap socket apakah ready
        for (SOCKET s = 0; s <= maxSocket; s++) {
            if (FD_ISSET(s, &readSet)) {
                printf("[*] Socket %llu ready\n", (unsigned long long)s);

                if (s == serverSocket) {
                    // Socket server ready, berarti ada koneksi baru
                    clientAddrLen = sizeof(clientAddr);
                    clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddr, &clientAddrLen);
                    if (clientSocket == INVALID_SOCKET) {
                        printf("[!] accept() failed: %d\n", WSAGetLastError());
                        continue;
                    }

                    // Tambahkan client socket ke masterSet
                    FD_SET(clientSocket, &masterSet);
                    if (clientSocket > maxSocket) {
                        maxSocket = clientSocket;
                    }

                    printf("[+] Client connected!\n");
                } else {
                    // Ada data masuk dari client
                    bytesReceived = recv(s, buffer, sizeof(buffer), 0);
                    if (bytesReceived > 0) {
                        printf("[>] Received %d bytes: %.*s\n", bytesReceived, bytesReceived, buffer);
                        // Echo balik ke client
                        send(s, buffer, bytesReceived, 0);
                    } else {
                        if (bytesReceived == 0) {
                            printf("[*] Client disconnected\n");
                        } else {
                            printf("[!] recv() failed: %d\n", WSAGetLastError());
                        }
                        // Tutup socket client yang disconnect dan hapus dari masterSet
                        closesocket(s);
                        FD_CLR(s, &masterSet);
                    }
                }
            }
        }
    }

    printf("[*] Cleaning up sockets...\n");
    // Tutup semua socket yang masih aktif di masterSet
    for (SOCKET s = 0; s <= maxSocket; s++) {
        if (FD_ISSET(s, &masterSet)) {
            closesocket(s);
        }
    }

    closesocket(serverSocket);
    WSACleanup();
    printf("[*] Exited gracefully.\n");
    return 0;
}
