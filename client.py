import socket

HOST = "10.7.236.254"   # Server address
PORT = 6379          # Same port as your C++ server

def main():
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    try:
        client.connect((HOST, PORT))
        print(f"Connected to server at {HOST}:{PORT}")

        while True:
            msg = input("Enter message (or 'quit' to exit): ")

            if msg.lower() == "quit":
                break

            client.sendall(msg.encode())

            response = client.recv(4096)
            if not response:
                print("Server closed connection.")
                break

            print("Server:", response.decode())

    except ConnectionRefusedError:
        print("Could not connect to server. Make sure it is running.")

    finally:
        client.close()
        print("Disconnected.")


if __name__ == "__main__":
    main()
