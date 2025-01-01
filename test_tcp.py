import socket
import sys

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect(("192.168.68.114", 7050))

try:

    while True:
        data = sock.recv(2000)
        print(data)

finally:
    print('closing socket')
    sock.close()