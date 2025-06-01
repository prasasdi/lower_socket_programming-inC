#include <stdio.h>
#include <pcap.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdint.h>

#pragma comment(lib, "wpcap.lib")
#pragma comment(lib, "ws2_32.lib")

struct iphdr {
    unsigned char  ihl:4, version:4;
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

struct tcphdr {
    unsigned short source;
    unsigned short dest;
    unsigned int   seq;
    unsigned int   ack_seq;
    unsigned short doff_res_flags;
    unsigned short window;
    unsigned short check;
    unsigned short urg_ptr;
};

u_short checksum(unsigned short *buf, int len) {
    unsigned long sum = 0;
    while (len > 1) {
        sum += *buf++;
        len -= 2;
    }
    if (len) {
        sum += *(unsigned char *)buf;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}

int main(void) {
    pcap_if_t *alldevs, *d;
    pcap_t *handle;
    char errbuf[PCAP_ERRBUF_SIZE];
    u_char packet[60];

    // cari interface
    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        fprintf(stderr, "Error: %s\n", errbuf);
        return 1;
    }

    // pakai interface pertama
    d = alldevs;
    if (!d) {
        fprintf(stderr, "No device found\n");
        return 1;
    }

    printf("Using device: %s\n", d->name);

    handle = pcap_open_live(d->name, 65536, 1, 1000, errbuf);
    if (!handle) {
        fprintf(stderr, "pcap_open_live() failed %s\n", errbuf);
        return 1;
    }

    memset(packet, 0, sizeof(packet));

    // IPv4 + TCP Crafting
    struct iphdr *iph = (struct iphdr *)(packet + 14); // Ethernet header = 14 bytes
    struct tcphdr *tcph = (struct tcphdr *)(packet + 14 + sizeof(struct iphdr));

     // Isi IP header
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = htons(sizeof(struct iphdr) + sizeof(struct tcphdr));
    iph->id = htons(54321);
    iph->frag_off = 0;
    iph->ttl = 64;
    iph->protocol = IPPROTO_TCP;
    iph->saddr = inet_addr("192.168.1.101");  // IP sumber (sesuaikan)
    iph->daddr = inet_addr("192.168.1.100");  // IP tujuan (sesuaikan)

    iph->check = checksum((unsigned short *)iph, sizeof(struct iphdr));

    // Isi TCP header
    // identifikasi endpoint
    tcph->source = htons(12345);
    tcph->dest = htons(80);

    // RFC 793: Initial Sequence Number (ISN) adalah kunci dari reliable delivery, terutama di fase handshake (SYN).
    tcph->seq = htonl(0);
    
    tcph->ack_seq = 0;
    tcph->doff_res_flags = htons((5 << 12) | 0x02); // doff=5, flags=SYN
    tcph->window = htons(8192);
    tcph->urg_ptr = 0;

    // Pseudo-header untuk checksum TCP
    struct pseudo_header {
        uint32_t src;
        uint32_t dst;
        uint8_t zero;
        uint8_t proto;
        uint16_t len;
    } psh;

    psh.src = iph->saddr;
    psh.dst = iph->daddr;
    psh.zero = 0;
    psh.proto = IPPROTO_TCP;
    psh.len = htons(sizeof(struct tcphdr));

    char pseudo_packet[1024];
    memcpy(pseudo_packet, &psh, sizeof(psh));
    memcpy(pseudo_packet + sizeof(psh), tcph, sizeof(struct tcphdr));
    tcph->check = checksum((unsigned short*)pseudo_packet, sizeof(psh) + sizeof(struct tcphdr));

    // Tambahkan Ethernet header dummy (karena Npcap butuh L2 header)
    memset(packet, 0xff, 6); // dst MAC (broadcast)
    memset(packet + 6, 0x11, 6); // src MAC (dummy)
    packet[12] = 0x08;
    packet[13] = 0x00; // Ethertype = IPv4

    // Kirim packet
    if (pcap_sendpacket(handle, packet, sizeof(struct iphdr) + sizeof(struct tcphdr) + 14) != 0) {
        fprintf(stderr, "Error sending packet: %s\n", pcap_geterr(handle));
    } else {
        printf("Packet sent!\n");
    }

    pcap_close(handle);
    pcap_freealldevs(alldevs);
    return 0;
}