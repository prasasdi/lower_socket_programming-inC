# Raw TCP SYN Packet
CLI ini digunakan untuk mengirimkan paket TCP yang dirakit secara manual (raw packet), dan menggunakan Npcap untuk mengirimkan paket tersebut langsung ke jaringan.

# Kenapa Npcap?
Windows sendiri enggak memberikan akses jaringan secara mentah begitu aja. Makanya pakai Npcap untuk akses tingkat terendah yang memungkinkan untuk menerima dan mengirim packet yang enggak bisa dilakukan lewat `socket`nya windows.

## Sebenernya
Udah paling enak di Linux, tapi kalau cuman bilang "udah paling enak" tanpa alasan yang kuat juga kenapa.
