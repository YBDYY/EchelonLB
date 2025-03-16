import socket
import sys

# Get the port from the command line arguments
port = int(sys.argv[1])

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)  # Allow reusing the port
server.bind(("127.0.0.1", port))
server.listen(5)
print(f"[INFO] Backend is running on port {port}")

while True:
    conn, addr = server.accept()
    print(f"[INFO] Connection from {addr}")
    
    try:
        while True:
            data = conn.recv(1024)
            if not data:
                print("[INFO] Client disconnected")
                break  # Exit loop when client closes connection
            
            try:
                decoded_data = data.decode("utf-8", errors="ignore")  # Ignore invalid characters
                print(f"[INFO] Received: {decoded_data.strip()}")
                conn.sendall(f"Echo from backend {port}: {decoded_data}".encode("utf-8"))
            except UnicodeDecodeError:
                print("[ERROR] Received non-UTF-8 data, ignoring.")
                continue
    finally:
        conn.close()
