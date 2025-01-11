import socket
import sys
import datetime

if len(sys.argv) != 5:
    print("Usage: python ReTcpLogger.py <IP> <PORT> <FILENAME> <MODE>")
    sys.exit(1)

IP = sys.argv[1]
PORT = int(sys.argv[2])
FILENAME = sys.argv[3]
MODE = sys.argv[4].lower()

if MODE not in ["text", "binary"]:
    print("MODE must be 'text' or 'binary'")
    sys.exit(1)

# Add timestamp to filename
timestamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
full_filename = f"{FILENAME}_{timestamp}"

if MODE == "text":
    full_filename += ".txt"
    file_mode = 'w'
elif MODE == "binary":
    full_filename += ".bin"
    file_mode = 'wb'

# Create socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect((IP, PORT))

print(f"Connected to {IP}:{PORT} and writing data to {full_filename} in {MODE} mode")

try:
    with open(full_filename, file_mode) as f:
        while True:
            data = sock.recv(1024)
            if not data:
                break
            if MODE == "text":
                f.write(data.decode('utf-8'))
            elif MODE == "binary":
                f.write(data)
except:
    sock.close()
    print(f"Connection to {IP}:{PORT} closed")
