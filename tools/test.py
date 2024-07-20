#!/usr/bin/env python3
import socket
import struct
import sys

class Dev:
    def __init__(self, sock, addrport):
        self.sock = sock
        self.addrport = addrport

    def set_temp(self, temp):
        x = int(temp*10)
        data = struct.pack('<i', x)
        self.sock.sendto(b'TH8r\x01' + data, self.addrport)

    def get_temp(self):
        self.sock.sendto(b'TH8r\x02', self.addrport)
        data, addrport = self.sock.recvfrom(256)
        if data[:4] == b'TH8t':
            return struct.unpack('<i', data[4:])[0]/10.
        else:
            return None

    def probe(self):
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
        sock.sendto(b'TH8r\x00', ('192.168.1.255', 3333))
        while True:
            print(sock.recvfrom(256))

if __name__ == '__main__':
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    dev = Dev(sock, ('192.168.1.23', 3333))
    if len(sys.argv) >= 2 and sys.argv[1] == '-p':
        dev.probe()
    else:
        print('当前温度', dev.get_temp())
        s = input('请输入设定温度:')
        t = float(s)
        dev.set_temp(t)
