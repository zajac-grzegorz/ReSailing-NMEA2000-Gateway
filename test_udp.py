import socket
 
UDP_IP = "" # empty for broadcast address
UDP_PORT = 8050

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

while True:
    data, addr = sock.recvfrom(2000) # buffer size is 2000 bytes

    text = data.decode(encoding="utf-8")    # this is to display text data. Originally data is in bytes
    if (len(text)) :
        print(text)