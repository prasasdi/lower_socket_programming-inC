#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void packet_handler(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
    printf("Captured a packet with length of [%d]\n", header->len);
}

int main() {
    pcap_if_t *alldevs;
    char errbuf[PCAP_ERRBUF_SIZE];

    // Cari semua device jaringan
    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        fprintf(stderr, "Error finding devices: %s\n", errbuf);
        return 1;
    }

    if (alldevs == NULL) {
        printf("No interfaces found! Make sure Npcap is installed.\n");
        return 1;
    }

    printf("Available interfaces:\n");
    int i = 0;
    for (pcap_if_t *d = alldevs; d != NULL; d = d->next) {
        printf("%d. %s", i++, d->name);
        if (d->description)
            printf(" (%s)", d->description);
        printf("\n");
    }

    int iface_num;
    printf("Enter the interface number to sniff: ");
    if (scanf("%d", &iface_num) != 1) {
        fprintf(stderr, "Invalid input.\n");
        pcap_freealldevs(alldevs);
        return 1;
    }

    // Validasi input
    pcap_if_t *device = alldevs;
    for (int j = 0; j < iface_num; j++) {
        if (device->next == NULL) {
            fprintf(stderr, "Interface number out of range.\n");
            pcap_freealldevs(alldevs);
            return 1;
        }
        device = device->next;
    }

    // Buka interface
    pcap_t *handle = pcap_open_live(device->name, 65536, 1, 1000, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Couldn't open device %s: %s\n", device->name, errbuf);
        pcap_freealldevs(alldevs);
        return 1;
    }

    printf("Opened interface %s for sniffing...\n", device->name);

    pcap_freealldevs(alldevs);

    // Tangkap paket selama 10 detik
    time_t start_time = time(NULL);
    while (time(NULL) - start_time < 10) {
        int res = pcap_dispatch(handle, 1, packet_handler, NULL);
        if (res < 0) {
            fprintf(stderr, "Error capturing packets: %s\n", pcap_geterr(handle));
            break;
        }
    }

    pcap_close(handle);

    printf("Capture finished.\n");
    return 0;
}
