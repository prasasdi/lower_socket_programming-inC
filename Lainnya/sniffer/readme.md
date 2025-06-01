# tl;dr
(update 25/05/2025) Gw masih pelajari apa yang bisa diambil dari sniffer. Yang gw baca adalah apa yang terjadi pada antarmuka internet gw dibelakang layarnya. Apa yang gw mau ambil juga belum tau, FAFO dulu aja

# Insight apa yang bisa gw kasih?
Kayaknya cara pakai npcap dulu mungkin. Kalau kalian lihat ada yang seperti ini `\Device\NPF_{id}` dan itu artinya npcap device names yang mewakili network interfaces di Windows. (Iya maap banget, gw pengen universal cuman untuk saat ini Windows dulu ya, untuk belajar network interface)

Dan dari sniffer ini, yang gw ambil sekarang ini adalah ukuran packet. Dan gw coba untuk nonton video youtube.

Output yang gw dapat adalah seperti ini:
```bash
Opening interface: \Device\NPF_{951FA70D-8B7D-4A33-8442-65BC36139083}

...

[sniff] Packet captured! Length: 1292
[heartbeat] alive at 444081781 ms

[sniff] Packet captured! Length: 1292
[heartbeat] alive at 444083890 ms

[sniff] Packet captured! Length: 1292
[heartbeat] alive at 444086015 ms

[sniff] Packet captured! Length: 123
[heartbeat] alive at 444088125 ms

[sniff] Packet captured! Length: 615
[heartbeat] alive at 444090250 ms

[sniff] Packet captured! Length: 82
[heartbeat] alive at 444092359 ms

[sniff] Packet captured! Length: 82
[heartbeat] alive at 444094484 ms

[sniff] Packet captured! Length: 1292
[heartbeat] alive at 444096593 ms

[sniff] Packet captured! Length: 1292
[heartbeat] alive at 444098703 ms

[sniff] Packet captured! Length: 1292
[heartbeat] alive at 444105859 ms

[sniff] Packet captured! Length: 341
[heartbeat] alive at 444107968 ms

[sniff] Packet captured! Length: 986
[heartbeat] alive at 444110093 ms

[sniff] Packet captured! Length: 67
[heartbeat] alive at 444112203 ms

[sniff] Packet captured! Length: 120
[heartbeat] alive at 444114328 ms

[sniff] Packet captured! Length: 64
[heartbeat] alive at 444116437 ms

[sniff] Packet captured! Length: 73
[heartbeat] alive at 444118562 ms

[sniff] Packet captured! Length: 570
[heartbeat] alive at 444120671 ms

[keyboard] Quit signal received. << ini gw pencet tombol `q` untuk quit >>
```

Yang pertama, kenapa 1292 byte? Karena itu mendekati maksimum payload TCP (MTU minus overhead)

> The maximum size of a TCP packet is 64K (65535 bytes). Generally, the packet size gets restricted by the Maximum Transmission Unit (MTU) of network resources.

Bingung ya? Jangan kuatir. Gini aja:
1. Maksimum ukuran packet TCP = 64KB adalah batas teoritis dari TCP Segment yang bisa dikirim dalam satu packet. 

Dan angka 64KB (atau 65535) muncul karena ukuran field total length di header IP cuman 16 bit, jadi maksimal bisa bilang segitu nilainya. Tapii,

2. Di dunia nyata, paket data lewat banyak lapisan jaringan yang punya batas ukuran sendiri.

Misal Ethernet yang punya batas ukuran packet yang disebut MTU (maximum transimission unit) dan umumnya adalah 1500 bytes. Berarti sebuah packet IP yang lewat Ethernet enggak boleh lebih dari 1500 bytes

3. Dari 1500 kenapa 1292?

Bisa jadi 1292 adalah ukuran payload TCP (data) + header TCP/IP yang kita tangkap.
# Tools yang diperlukan
1. Developer Command Control
2. Driver npcap

# Cara jalanin aplikasi
1. Buka `Developer Command Control` dan kemudian `/cd` ke direktori ini
2. Udah begitu kemudian `nmake run` dan pilih interface mana yang mau di baca packetnya, gw sendiri pakai `\Device\NPF_{951FA70D-8B7D-4A33-8442-65BC36139083} (Intel(R) Wi-Fi 6 AX201 160MHz)`

# Update 25/05/2025
Gw dump hex dari TCP Payload yang bisa kita kulik bareng-bareng.