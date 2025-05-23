#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>
#include <conio.h> // Untuk _kbhit dan _getch di Windows
#include "../include/task_scheduler/task_scheduler.h"

static pcap_t* g_pcap_handle = NULL;
static volatile int running = 1;

int sniff_task(Coroutine* c) {
    CORO_BEGIN(c);

    while (running) {
        struct pcap_pkthdr* header;
        const u_char* data;
        int res = pcap_next_ex(g_pcap_handle, &header, &data);

        if (res == 1 && data != NULL) {
            printf("[sniff] Packet captured! Length: %u\n", header->len);
        } else if (res == 0) {
            printf("[sniff] No packet yet...\n");
        } else if (res == -1) {
            fprintf(stderr, "[sniff] Error: %s\n", pcap_geterr(g_pcap_handle));
            break;
        } else if (res == -2) {
            printf("[sniff] Capture stopped.\n");
            break;
        }

        CORO_YIELD(c); // allow other tasks to run
    }

    CORO_END(c);
}

int heartbeat_task(Coroutine* c) {
    CORO_BEGIN(c);

    while (running) {
        printf("[heartbeat] alive at %llu ms\n", now_ms());
        CORO_YIELD(c);
        sleep_ms(2000);  // simulate periodic behavior
    }

    CORO_END(c);
}

int keyboard_task(Coroutine* c) {
    CORO_BEGIN(c);

    while (running) {
        if (_kbhit()) {
            int ch = _getch();
            if (ch == 'q' || ch == 'Q') {
                printf("[keyboard] Quit signal received.\n");
                running = 0;
                break;
            }
        }

        CORO_YIELD(c);
    }

    CORO_END(c);
}

int main() {
    char errbuf[PCAP_ERRBUF_SIZE];

    // List available interfaces
    pcap_if_t* alldevs;
    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        fprintf(stderr, "pcap_findalldevs error: %s\n", errbuf);
        return 1;
    }

    printf("Available interfaces:\n");
    int i = 0;
    for (pcap_if_t* d = alldevs; d; d = d->next) {
        printf("%d. %s (%s)\n", ++i, d->name, d->description ? d->description : "No description");
    }

    printf("Enter the interface number to sniff: ");
    int choice;
    scanf("%d", &choice);

    if (choice < 1 || choice > i) {
        printf("Invalid choice.\n");
        pcap_freealldevs(alldevs);
        return 1;
    }

    pcap_if_t* chosen = alldevs;
    for (int j = 1; j < choice; j++) {
        chosen = chosen->next;
    }

    printf("Opening interface: %s\n", chosen->name);
    g_pcap_handle = pcap_open_live(chosen->name, 65536, 1, 1000, errbuf);
    if (!g_pcap_handle) {
        fprintf(stderr, "pcap_open_live error: %s\n", errbuf);
        pcap_freealldevs(alldevs);
        return 1;
    }

    pcap_freealldevs(alldevs);

    // Register coroutines
    await(sniff_task);
    await(heartbeat_task);
    await(keyboard_task);  // <--- tambahan

    // Start scheduler loop
    run_scheduler();

    pcap_close(g_pcap_handle);
    return 0;
}
