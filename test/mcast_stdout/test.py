import os, sys, subprocess, time

proc = subprocess.Popen(["bin/recorder", "0.0.0.0", "224.1.1.1", "5000"])

import socket
address = ('224.1.1.1', 5000)
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, 2)
time.sleep(0.2)
sock.sendto("1234567890", address)
time.sleep(0.2)
sock.close()
proc.kill()