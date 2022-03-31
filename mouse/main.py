import time
import socket
import pyautogui

time.sleep(1)


socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
socket.bind(("127.0.0.1", 8769))


while True:
    try:
        location = pyautogui.locateOnScreen('target.png', confidence=0.6)
        x, y = pyautogui.position()
        print(location)
        if location is None:
            dx = 0
            dy = 0
        else:
            nx, ny = location
            dx = nx - x
            dy = -(ny - y)
        print(dx, dy)
        dx = dx.to_bytes(4, 'little', signed=True)
        dy = dy.to_bytes(4, 'little', signed=True)
        msg = dx + dy
        socket.sendto(msg, ("127.0.0.1", 11111))

    except Exception as e:
        print(e)


