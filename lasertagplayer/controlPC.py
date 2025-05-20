import socket
import threading
import tkinter as tk
from tkinter import ttk

HOST = '0.0.0.0'
PORT = 1234

player_data = {
    0: {"HP": 0, "AMMO": 0, "conn": None},
    1: {"HP": 0, "AMMO": 0, "conn": None}
}

def update_gui():
    for i in range(2):
        hp = player_data[i]["HP"]
        ammo = player_data[i]["AMMO"]

        hp_var[i].set(max(0, hp))  # Clamp HP to 0
        ammo_var[i].set(ammo)
        hp_label[i].config(text=f"{max(0, hp)}/10")
        ammo_label[i].config(text=f"{ammo}/10")
    root.after(500, update_gui)

def handle_player(conn, addr, player_id):
    print(f"Player {player_id + 1} connected from {addr}")
    player_data[player_id]["conn"] = conn
    while True:
        try:
            data = conn.recv(1024)
            if not data:
                break
            msg = data.decode().strip()
            parts = msg.split("|")
            for part in parts:
                if "HP:" in part:
                    player_data[player_id]["HP"] = int(part.replace("HP:", ""))
                elif "AMMO:" in part:
                    player_data[player_id]["AMMO"] = int(part.replace("AMMO:", ""))
            print(f"[Player {player_id + 1}] {msg}")
        except Exception as e:
            print(f"Error with player {player_id + 1}: {e}")
            break
    conn.close()
    player_data[player_id]["conn"] = None
    print(f"Player {player_id + 1} disconnected")

def start_server():
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind((HOST, PORT))
    server.listen(2)
    print("Waiting for connections...")
    player_id = 0
    while player_id < 2:
        conn, addr = server.accept()
        thread = threading.Thread(target=handle_player, args=(conn, addr, player_id))
        thread.start()
        player_id += 1

def reset_game():
    print("Resetting game...")
    for i in range(2):
        player_data[i]["HP"] = 10
        player_data[i]["AMMO"] = 10
        conn = player_data[i]["conn"]
        if conn:
            try:
                conn.sendall(b"RESET\n")
            except Exception as e:
                print(f"Failed to send reset to Player {i+1}: {e}")

root = tk.Tk()
root.title("Player Status")

hp_var = [tk.IntVar(), tk.IntVar()]
ammo_var = [tk.IntVar(), tk.IntVar()]
hp_label = [None, None]
ammo_label = [None, None]

for i in range(2):
    frame = ttk.LabelFrame(root, text=f"Player {i+1}", padding=10)
    frame.grid(row=0, column=i, padx=10, pady=10)

    ttk.Label(frame, text="Health").grid(row=0, column=0, sticky='w')
    ttk.Progressbar(frame, maximum=10, variable=hp_var[i], length=150).grid(row=1, column=0, pady=5)
    hp_label[i] = ttk.Label(frame, text="0/10")
    hp_label[i].grid(row=1, column=1, padx=5)

    ttk.Label(frame, text="Ammo").grid(row=2, column=0, sticky='w')
    ttk.Progressbar(frame, maximum=10, variable=ammo_var[i], length=150).grid(row=3, column=0, pady=5)
    ammo_label[i] = ttk.Label(frame, text="0/10")
    ammo_label[i].grid(row=3, column=1, padx=5)

# Add a Reset button
reset_button = ttk.Button(root, text="Reset Game", command=reset_game)
reset_button.grid(row=1, column=0, columnspan=2, pady=10)

server_thread = threading.Thread(target=start_server, daemon=True)
server_thread.start()

update_gui()

root.mainloop()
