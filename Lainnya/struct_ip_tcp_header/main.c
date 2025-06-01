#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

// IP Header
struct ip_header {
    unsigned char  ihl:4;     // Header length
    unsigned char  version:4; // Version
    unsigned char  tos;
    unsigned short tot_len;
    unsigned short id;
    unsigned short frag_off;
    unsigned char  ttl;
    unsigned char  protocol;
    unsigned short check;
    unsigned int   saddr;
    unsigned int   daddr;
};

// TCP Header
struct tcp_header {
    unsigned short source;
    unsigned short dest;
    unsigned int   seq;
    unsigned int   ack_seq;
    unsigned short doff_res_flags;
    unsigned short window;
    unsigned short check;
    unsigned short urg_ptr;
};

// Helper to print IP
void print_ip(unsigned int ip) {
    struct in_addr addr;
    addr.S_un.S_addr = ip;
    printf("%s", inet_ntoa(addr));
}

// Contoh parsing data dummy
int main() {
    // Data dummy (panjang harus minimal 40 bytes = IP (20) + TCP (20))
    unsigned char packet[] = {
        0x45, 0x00, 0x00, 0x3c, 0x1c, 0x46, 0x40, 0x00,
        0x40, 0x06, 0xb1, 0xe6, 0xc0, 0xa8, 0x00, 0x02,
        0xc0, 0xa8, 0x00, 0x01,
        0x00, 0x50, 0x01, 0xbb, 0xab, 0xcd, 0x00, 0x01,
        0x12, 0x34, 0x00, 0x00, 0x50, 0x18, 0x71, 0x10,
        0xe6, 0x32, 0x00, 0x00
    };

    struct ip_header *iph = (struct ip_header *)packet;
    struct tcp_header *tcph = (struct tcp_header *)(packet + iph->ihl * 4);

    printf("IP Header:\n");
    printf(" |-Version        : %d\n", iph->version);
    printf(" |-IHL            : %d (x4 = %d bytes)\n", iph->ihl, iph->ihl * 4);
    printf(" |-Total Length   : %d\n", ntohs(iph->tot_len));
    printf(" |-TTL            : %d\n", iph->ttl);
    printf(" |-Protocol       : %d\n", iph->protocol);
    printf(" |-Source IP      : "); print_ip(iph->saddr); printf("\n");
    printf(" |-Destination IP : "); print_ip(iph->daddr); printf("\n");

    printf("\nTCP Header:\n");
    printf(" |-Source Port      : %d\n", ntohs(tcph->source));
    printf(" |-Destination Port : %d\n", ntohs(tcph->dest));
    printf(" |-Sequence Number  : %u\n", ntohl(tcph->seq));
    printf(" |-Ack Number       : %u\n", ntohl(tcph->ack_seq));
    printf(" |-Flags            : 0x%02x\n", (ntohs(tcph->doff_res_flags) & 0x3F));
    printf(" |-Window Size      : %d\n", ntohs(tcph->window));

    return 0;
}
