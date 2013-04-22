import socket

address = ('224.1.1.1', 5000)

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, 2)

n = 0
while True:
    sock.sendto(str(n), address)
    n += 1

