#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>
#include <conio.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "Ws2_32.lib")

    #ifndef INET_ADDRSTRLEN
    #define INET_ADDRSTRLEN 16
    #endif

    // Define ethhdr manually
    // struct ethhdr {
    //     u_char h_dest[6];
    //     u_char h_source[6];
    //     u_short h_proto;
    // };

    #define ntohs(x) _byteswap_ushort(x)
    #define ntohl(x) _byteswap_ulong(x)
#else
    #include <arpa/inet.h>
    #include <netinet/ip.h>
    #include <netinet/tcp.h>
    #include <netinet/if_ether.h>
    #include <sys/socket.h>
#endif

#include "../include/task_scheduler/task_scheduler.h"

#pragma comment(lib, "Ws2_32.lib") // Needed for Winsock functions

static pcap_t* g_pcap_handle = NULL;
static volatile int running = 1;

#define ETHERNET_SIZE 14

#ifndef TH_FIN
#define TH_FIN  0x01
#define TH_SYN  0x02
#define TH_RST  0x04
#define TH_PUSH 0x08
#define TH_ACK  0x10
#define TH_URG  0x20
#endif


// Ethernet header
struct ethhdr {
    u_char dst[6];
    u_char src[6];
    u_short type;
};

// IPv4 header
struct iphdr {
    u_char ihl:4, version:4;
    u_char tos;
    u_short tot_len;
    u_short id;
    u_short frag_off;
    u_char ttl;
    u_char protocol;
    u_short check;
    u_int saddr;
    u_int daddr;
};

// TCP header
struct tcphdr {
    u_short source;
    u_short dest;
    u_int seq;
    u_int ack_seq;
    u_char doff:4, reserved:4;
    u_char flags;
    u_short window;
    u_short check;
    u_short urg_ptr;
};

void parse_tcp_packet(const u_char* data, int packet_len) {
    if (packet_len < ETHERNET_SIZE + sizeof(struct iphdr)) return;

    const struct ethhdr* eth = (struct ethhdr*)data;

    if (ntohs(eth->type) != 0x0800) return; // Only handle IPv4

    const struct iphdr* ip = (struct iphdr*)(data + ETHERNET_SIZE);
    int ip_header_len = ip->ihl * 4;

    if (ip->protocol != 6) return; // Only handle TCP

    const struct tcphdr* tcp = (struct tcphdr*)(data + ETHERNET_SIZE + ip_header_len);
    int tcp_header_len = tcp->doff * 4;

    const u_char* payload = data + ETHERNET_SIZE + ip_header_len + tcp_header_len;
    int total_ip_len = ntohs(ip->tot_len);
    int payload_len = total_ip_len - ip_header_len - tcp_header_len;

    // IP to string
    char src_ip[INET_ADDRSTRLEN];
    char dst_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ip->saddr, src_ip, sizeof(src_ip));
    inet_ntop(AF_INET, &ip->daddr, dst_ip, sizeof(dst_ip));

    printf("[tcp] %s:%d -> %s:%d | Payload: %d bytes\n",
           src_ip, ntohs(tcp->source),
           dst_ip, ntohs(tcp->dest),
           payload_len);

    // Ambil byte flag (offset 13 dari awal header TCP)
    uint8_t flags = ((const uint8_t*)tcp)[13];
    if (flags & TH_SYN) printf("SYN ");
    if (flags & TH_ACK) printf("ACK ");
    if (flags & TH_FIN) printf("FIN ");
    if (flags & TH_RST) printf("RST ");
    if (flags & TH_PUSH) printf("PSH ");
    if (flags & TH_URG) printf("URG ");
    printf("\n");

    // Dump payload
    if (payload_len > 0) {
        printf("        Payload (hex): ");
        for (int i = 0; i < payload_len; ++i) {
            printf("%02X ", payload[i]);
            if ((i + 1) % 16 == 0 && i + 1 < payload_len)
                printf("\n                      ");
        }
        printf("\n");
    }
}

int sniff_task(Coroutine* c) {
    CORO_BEGIN(c);

    while (running) {
        struct pcap_pkthdr* header;
        const u_char* data;
        int res = pcap_next_ex(g_pcap_handle, &header, &data);

        if (res == 1 && data != NULL) {
            printf("[sniff] Packet captured! Length: %u\n", header->len);
            parse_tcp_packet(data, header->len);
        } else if (res == 0) {
            printf("[sniff] No packet yet...\n");
        } else if (res == -1) {
            fprintf(stderr, "[sniff] Error: %s\n", pcap_geterr(g_pcap_handle));
            break;
        } else if (res == -2) {
            printf("[sniff] Capture stopped.\n");
            break;
        }

        CORO_YIELD(c);
    }

    CORO_END(c);
}

int heartbeat_task(Coroutine* c) {
    CORO_BEGIN(c);

    while (running) {
        printf("[heartbeat] alive at %llu ms\n", now_ms());
        CORO_YIELD(c);
        sleep_ms(2000);
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
    await(keyboard_task);

    // Run scheduler
    run_scheduler();

    pcap_close(g_pcap_handle);
    return 0;
}
