import serial
print(serial.__file__)
import time

ser = serial.Serial('/dev/tty.usbmodem1101', 115200)  # or COM3 on Windows

# light up each column
for col in range(16):
    ser.write(bytes([col, 6]))  # column, magnitude
    time.sleep(0.1)