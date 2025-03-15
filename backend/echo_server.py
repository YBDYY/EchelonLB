import socket

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind(("127.0.0.1", 9001))
server.listen(5)

print("[INFO] Backend is running on port 9001")

while True:
    client_sock, addr = server.accept()
    print(f"[INFO] Connection from {addr}")

    while True:
        data = client_sock.recv(4096)
        if not data:
            print("[INFO] Client disconnected")
            break  #Dont close the whole server, just this connection.

        print(f"[INFO] Received: {data.decode().strip()}")
        response = f"Echo from backend 9001: {data.decode()}"
        client_sock.sendall(response.encode())

    client_sock.close()
