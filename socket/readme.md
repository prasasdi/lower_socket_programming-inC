# Loh, kok cuman pake aja?
Iyalah, socket programming pada dasarnya pakai interface (antarmuka) yang sudah disediakan oleh sistem operasi untuk akses jaringan. Walaupun namanya "socket" bukan berarti bener berupa bentuk fisik atau apa, melainkan abstraksi OS biar bisa komunikasi dengan perangkat yang lain atau antar proses dalam mesin yang sama. Gitu sih

API yang digunakan dari contoh diatas kan begini:
1. `socket_server.c` dan `socket_client.c` keduanya pakai API `socket()` utk bikin socket yang kemudian,
2. `socket_server.c` pakai API `bind()`, `listen()`, dan `accept()` abis itu,
3. `socket_client.c` pakai `send()` dan `recv()` untuk kirim dan terima data

> Semua itu hanya instruksi ke OS untuk "OS, tolong bikin channel komunikasi ke alamat X pakai protokol Y dan kirim data Z"

# Enggak detail?
Bisa iya dan enggak sih karena analoginya kayak begini, bayangin kayak telepon seseorang yang kita perlu tau cuman tiga hal ini: angkat, tekan nomor, kemudian bicara. Sudah itu aja dan itu interfacenya. Selebihnya seperti cara kerja kabel bawah tanah/ laut, sinyal digital, dan switching di BTS enggak perlu tau -- terserah sih --

# Benefitnya apa?
Dari banyak benefit keuntungan yang bisa digapai; entah dari kontrol penuh jaringan bikin firewall/IDS sendiri misalnya, bahkan belajar arsitektur protokol jaringan. Kayaknya arsitektur protokol jaringan yang paling masuk akal.

Atau misal cita-cita gw yang pengen buat atau bikin MQTT sendiri, atau protokol komunikasi antar perangkat alat embedded.

# Pertimbangkan Resikonya!
| Risiko            | Penjelasan                                                    |
| ----------------- | ------------------------------------------------------------- |
| ⚙️ Kompleks       | Semua tanggung jawab di tanganmu: reassembly, retransmit, dll |
| 🔐 Security Risk  | Salah handling bisa bikin app kamu rawan eksploit             |
| ⚠️ Tidak portabel | Banyak hal low-level ini beda antara OS, bahkan NIC driver    |
| 🛑 Root/Admin     | Banyak butuh hak akses tinggi                                 |

## Yang bisa dilakuin, menurut gw
+ Kirim ICMP (ping) manual
+ Kirim paket TCP SYN tanpa TCP stack
+ Atau bikin mini sniffer pakai Npcap

# Jangan kuatir cuy
Bikin aplikasi CRUD web juga kalau enggak ngerti perihal security juga riskan juga toh? Btw liat tabel dibawah

| Aspek            | CRUD (Web/API)                   | Low-Level Network                           |
| ---------------- | -------------------------------- | ------------------------------------------- |
| Proteksi default | Framework bantu (ORM, sanitizer) | Nggak ada—semua harus manual                |
| Akses protokol   | Biasanya HTTP/HTTPS via lib      | Akses langsung ke paket/protokol            |
| Kesalahan umum   | SQL injection, XSS, CSRF         | Buffer overflow, packet spoof, DoS          |
| Kontrol akses    | Middleware/Auth bawaan           | Harus bikin sendiri (by IP/level)           |
| Audit/logging    | Logger bawaan (Serilog, etc)     | Harus log sendiri semua traffic             |
| Sandbox/batasan  | Jalan di VM/container aman       | Bisa langsung ngerusak NIC / OS kalau salah |

Memang, rata-rata kita pakai tools yang sudah ada. Tapi kalau low level mesti bikin sendiri -- atau pegang semua senjatanya sendiri --

# Gw tutup disini dulu
Iya emang kan socket programming itu cuman pakai interface OS aja untuk bisa berkomunikasi antar mesin atau antar proses. Kalau pertanyaannya "bisa enggak kalau implement interface sendiri" ya gw coba cari tau. Gw ada ngarah mikir ke low level lebih jauh lagi dan kenapa cuman pakai interface dari OS aja dan kenapa ga buat sendiri. Dari sini gw ketemu coba bermain di sekuriti socket programming.

Itu lebih baik sih.