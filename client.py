import socket
import struct
import threading
import argparse

MAX_NICK_LEN = 32
MAX_MSG_LEN = 256

class ChatClient:
    def __init__(self, address=None, port=1500, nickname=None):
        self.sock = None
        self.nickname = nickname
        self.connected = False
        self.running = True
        
        if address:
            self.connect(address, port)
            if nickname and self.connected:
                self.register_nickname(nickname)
    
    def send_packet(self, sender, target, message):
        data = struct.pack(f'{MAX_NICK_LEN}s{MAX_NICK_LEN}s{MAX_MSG_LEN}s',
                          sender.encode(), target.encode(), message.encode())
        self.sock.send(data)
    
    def receive_loop(self):
        while self.running and self.connected:
            try:
                data = self.sock.recv(1024)
                if data:
                    print(f"\n{data.decode()}\n> ", end="", flush=True)
                else:
                    print("\nDisconnected")
                    self.connected = False
                    break
            except:
                break
    
    def connect(self, address, port):
        try:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.sock.connect((address, port))
            self.connected = True
            print(f"Connected to {address}:{port}")
            threading.Thread(target=self.receive_loop, daemon=True).start()
            return True
        except Exception as e:
            print(f"Connection failed: {e}")
            return False
    
    def register_nickname(self, nickname):
        if len(nickname) >= MAX_NICK_LEN:
            print(f"Nickname too long (max {MAX_NICK_LEN-1})")
            return
        self.nickname = nickname
        if self.connected:
            self.send_packet(nickname, nickname, "__REGISTER__")
            print(f"Nickname: {nickname}")
    
    def handle_command(self, cmd):
        parts = cmd.split(' ', 1)
        command = parts[0].upper()
        args = parts[1] if len(parts) > 1 else ""
        
        if command == "/NICK":
            self.register_nickname(args.strip())
        
        elif command == "/CONNECT":
            addr_parts = args.strip().split(':')
            address = addr_parts[0]
            port = int(addr_parts[1]) if len(addr_parts) > 1 else 1500
            self.connect(address, port)
        
        elif command == "/PRIVMSG":
            if ':' in args:
                target, message = args.split(':', 1)
                if not self.connected or not self.nickname:
                    print("Not connected or no nickname set")
                else:
                    self.send_packet(self.nickname, target.strip(), message)
        
        elif command == "/QUIT":
            self.running = False
            self.connected = False
            if self.sock:
                self.sock.close()
            return False
        
        elif command == "/HELP":
            print("/NICK <name> | /CONNECT <addr>:<port> | /PRIVMSG <user> :<msg> | /QUIT")
        
        return True
    
    def run(self):
        print("Chat Client - Type /HELP for commands\n")
        try:
            while True:
                cmd = input("> ").strip()
                if cmd.startswith('/'):
                    if not self.handle_command(cmd):
                        break
        except KeyboardInterrupt:
            print("\nExiting")
            if self.sock:
                self.sock.close()

def main():
    parser = argparse.ArgumentParser(description='IRC-Style Chat Client')
    parser.add_argument('-s', '--server', help='Server address')
    parser.add_argument('-p', '--port', type=int, default=1500, help='Port (default: 1500)')
    parser.add_argument('-n', '--nick', help='Nickname')
    args = parser.parse_args()
    
    ChatClient(args.server, args.port, args.nick).run()

if __name__ == "__main__":
    main()