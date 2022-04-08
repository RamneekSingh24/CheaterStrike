import socket
import time
import pyautogui
from pynput.mouse import Controller
import signal
import sys

from subprocess import Popen, PIPE, STDOUT

socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
socket.bind(("127.0.0.1", 8765))

peer_pub_key = socket.recv(1024)

p = Popen(".\keyex.exe", stdout=PIPE, stdin=PIPE, stderr=STDOUT)
keys = p.communicate(peer_pub_key)[0]
shared_auth_key = keys[:16]
my_pub_key = keys[32:32 + 64]

my_pub_key = b"\x6a\x83\xdc\x84\xd4\x4c\x8a\xbb\x5e\x42\xaf\xee\x8d\xe9\xf4\x57\x71\xfd\x73\x66\xd7\xfa\xad\xfa\xf2\x17\x14\xdd\x5a\xb9\x9e\x97\x79\xa7\x38\x72\xf2\xb8\xd6\xbe\x18\x91\x7f\xf7\xb5\xd3\xe5\x64\x9b\x12\x18\xaf\x39\x29\x6c\x24\x19\x38\x29\x0b\xc6\xac\x0c\x62"

socket.sendto(my_pub_key, ("127.0.0.1", 11111))

# shared_auth_key = b"\x3c\xa7\x58\x02\x55\xc5\xba\x2e\xca\xc6\xd2\xfa\x16\xf3\xb2\x0f"


print(shared_auth_key)



p = Popen(".\cmac.exe", stdout=PIPE, stdin=PIPE, stderr=PIPE)


def signal_handler(sig, frame):
    print('Stopping....')
    p.kill()
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

p.stdin.write(shared_auth_key)
p.stdin.flush()
p.stdout.read(4)




mouse = Controller()

pos = mouse.position

itr = 0

while True:
    itr += 1
    try:
        f = False
        if itr % 100000 == 0:
            location = pyautogui.locateOnScreen('target.png', confidence=0.6)
        else:
            location = None
        if location is None:
            new_pos = mouse.position
        else:
            print(f"[Aimbot] Found something {itr}, at ", location)
            f = True
            new_pos = location

        dx = new_pos[0] - pos[0]
        dy = -(new_pos[1] - pos[1])  # Adjust acc to expected game input
        f = False # remove this
        if f:
            print(dx, dy)
        dx = int(dx)
        dy = int(dy)
        dx = dx.to_bytes(4, 'little', signed=True)
        dy = dy.to_bytes(4, 'little', signed=True)
        msg = dx + dy
        p.stdin.write(msg)
        p.stdin.flush()
        auth_tag = p.stdout.read(16)
        msg = msg + auth_tag
        socket.sendto(msg, ("127.0.0.1", 11111))
        pos = new_pos


    except Exception as e:
        print(e)






