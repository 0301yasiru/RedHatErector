import RPi.GPIO as GPIO
from datetime import datetime
from ctypes import *


cfunc = CDLL('/home/pi/Documents/Programs/Python/RedHatErector/Rpi/GPIO.so')

cfunc.setup()

time = datetime.now()

cfunc.shiftData(0b10011111)

time = datetime.now() - time

print(f"Time taken -> {time.microseconds} us")
