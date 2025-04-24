import socket
import threading

HOST = '0.0.0.0'
PORT = 1234

def handle_player(conn, addr):
    print(f"Player connected from {addr}")
    while True:
        try:
            data = conn.recv(1024)
            if not data:
                break
            print(f"[{addr}] {data.decode().strip()}")
        except:
            break
    print(f"Player {addr} disconnected")
    conn.close()

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind((HOST, PORT))
server.listen(2)

print("Waiting for connections...")

while True:
    conn, addr = server.accept()
    thread = threading.Thread(target=handle_player, args=(conn, addr))
    thread.start()
