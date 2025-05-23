#include <pcap.h>
#include <stdio.h>

int main() {
    printf("Hello\n");
    
    pcap_if_t *alldevs;
    pcap_if_t *d;
    char errbuf[PCAP_ERRBUF_SIZE];

    // Ambil list adapter
    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
        return 1;
    }

    int i = 0;
    for (d = alldevs; d != NULL; d = d->next) {
        printf("%d. %s", ++i, d->name);
        if (d->description) {
            printf(" (%s)\n", d->description);
        } else {
            printf(" (No description available)\n");
        }
    }

    if (i == 0) {
        printf("\nNo interfaces found! Make sure Npcap is installed.\n");
        return -1;
    }

    int dev_num;
    printf("Enter the interface number (1-%d): ", i);
    scanf("%d", &dev_num);

    if (dev_num < 1 || dev_num > i) {
        printf("Interface number out of range.\n");
        pcap_freealldevs(alldevs);
        return -1;
    }

    // Pilih device
    for (d = alldevs, i = 1; i < dev_num; d = d->next, i++);

    // Buka device untuk sniffing
    pcap_t *handle = pcap_open_live(d->name, 65536, 1, 1000, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "\nUnable to open the adapter. %s is not supported by Npcap\n", d->name);
        pcap_freealldevs(alldevs);
        return -1;
    }

    printf("Listening on %s...\n", d->description);

    // Tangkap 1 paket
    struct pcap_pkthdr *header;
    const u_char *pkt_data;

    int res = pcap_next_ex(handle, &header, &pkt_data);
    if (res == 1) {
        printf("Packet captured at %ld.%06ld, length: %d\n",
               header->ts.tv_sec, header->ts.tv_usec, header->len);
    } else if (res == 0) {
        printf("Timeout expired\n");
    } else {
        printf("Error occurred: %s\n", pcap_geterr(handle));
    }

    // Bersih-bersih
    pcap_close(handle);
    pcap_freealldevs(alldevs);

    return 0;
}
