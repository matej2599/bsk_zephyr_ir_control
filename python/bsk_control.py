import socket
import time
import threading

PORT = 5000
UDP_PORT = 12345
DISCOVERY_TIMEOUT_SEC = 10
REPEAT_DISCOVERY_EVERY_SEC = 60

BSK_COMMANDS = ["BSK_TURN_ON_OFF",
                "BSK_SLEEP",
                "BSK_MODE_SUPPLY",
                "BSK_MODE_EXTRACT",
                "BSK_RECOVERY",
                "BSK_SPEED_SLOW",
                "BSK_SPEED_MID",
                "BSK_SPEED_FAST"]

class BskControl:
    def __init__(self):
        self.devices = {}
        self.discovery_thread = None

    def start_discovery(self):
        if self.discovery_thread is None:
            self.discovery_thread = threading.Thread(
                target=self.discovery_worker,
                daemon=True
            )
            self.discovery_thread.start()

    def discovery_worker(self):
        print("worker discovery running")
        while True:
            try:
                self.discover()
            except Exception as e:
                print("Discovery failed")

            time.sleep(REPEAT_DISCOVERY_EVERY_SEC)

    def discover(self):
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)

        sock.bind(("", 0))
        sock.settimeout(3)

        sock.sendto(
            b"BSK_DISCOVER",
            ("255.255.255.255", UDP_PORT)
        )

        deadline = time.monotonic() + DISCOVERY_TIMEOUT_SEC
        while time.monotonic() < deadline:
            try:
                data, addr = sock.recvfrom(1024)
                msg = data.decode().strip()
                parts = msg.split(";")

                if parts[0] != "BSK_DISCOVER_RESPONSE":
                    continue

                device_id = parts[1]
                tcp_port = int(parts[2])

                self.devices[device_id] = {
                    "ip": addr[0],
                    "port": tcp_port
                }

            except socket.timeout:
                break
        sock.close()

        print(f"Found {len(self.devices)}:")
        for name, device in self.devices.items():
            print(f"{name} - {device["ip"]}:{device["port"]}")

    def send_command(self, device_id, command):
        if device_id == "all" or device_id == "ALL":
            for name, device in self.devices.items():
                result = self.send_command_internal(device["ip"],
                                                    device["port"],
                                                    command)
                if result != "OK":
                    return result
            return "OK";
        else:
            device = self.devices[device_id]
            return self.send_command_internal(device["ip"],
                                              device["port"],
                                              command)

    def send_command_internal(self, ip, port, command):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.settimeout(2)
            s.connect((ip, port))
            s.sendall(f"{command}\n".encode())
            response = s.recv(1024).decode().strip()
            print("ESP says:", response)
            return response
