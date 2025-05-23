# Alasan gw bikin repository ini
Socket programming atau entah bikin http di C itu gampang dan semudah pakai interface socket dari tiap OS. Udah itu aja dan itu yang bikin gw gatel. Bisa enggak gw terjun lebih dalam lagi? Awalnya dari pengen buat web-socket, tapi emang udah ada yang tinggal pakai librarynya [wsServer](https://github.com/Theldus/wsServer), tapi bisa enggak dengan gw kulik lebih rendah dari socket bisa naikin permainan gw? Bisa dong harusnya.

Repository ini gw ragu karena awalnya pengen nulis perjalanan gw untuk mencapai websocket, setelah dipikir alangkah baiknya kalau gw belajar rakit socket dengan `raw socket` dan parse `header`. Udah kayak rakit mobil, cuman mobil yang dimaksud mobil cc kecil.

# Coba deh iseng begini
Buka repository ini [wsServer ws.h header](https://github.com/Theldus/wsServer/blob/master/include/ws.h) dan coba baca, kalau elu ngerti per variabelnya maksudnya apa dan metodenya bakal ngapain. Selamat berarti elu bisa, gw sendiri enggak.

Maka dari itu, gw buat repository ini untuk langkah bertahap gw dari belajar networking (kayak `send`, `recv` dan lainnya) sampai akhirnya ke HTTP atau sekalian WebSocket

# Informasi sedikit
Pemograman socket itu udah berupa antarmuka dari OS untuk digunakan oleh program supaya bisa berkomunikasi melalui jaringan. Atau anggep aja `socket` ini sebagai colokan virtual yang memungkinkan dua program saling terhubung dan bertukar data, baik di komputer yang sama atau yang berbeda.

Update: Setelah gw kulik, ada baiknya kalau setelah belajar socket belajar lebih bawah lagi sampai bikni Packet Capture/Inject tools kayak Wireshark, tcpdump, dan Scapy. Tapi ada kalanya kita belajar sekuriti socket programming. Misal buffer overflow attack, spoofed packet, no tls/encription, resource exhaustion (DoS). 

# Foldernya apa aja nih?
1. `socket`; awalan banget gw pengen tau `socket`, `bind`, `listen` dan lainnya.