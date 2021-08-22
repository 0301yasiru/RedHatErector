import RPi.GPIO as GPIO
from datetime import datetime
from time import sleep

clock_ = 11
serial_ = 10
latch_ = 22

def shiftOut(dPin, cPin, val):
    for i in range(0,8):
        GPIO.output(cPin,GPIO.HIGH)
        GPIO.output(dPin,(0x01&(val>>i)==0x01) and GPIO.HIGH or GPIO.LOW)
        GPIO.output(cPin,GPIO.LOW)

def setupGPIO():
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(clock_, GPIO.OUT)
    GPIO.setup(serial_, GPIO.OUT)
    GPIO.setup(latch_, GPIO.OUT)

    GPIO.output(latch_, GPIO.LOW)

def main():
    setupGPIO()

    start = datetime.now()
    GPIO.output(latch_, GPIO.HIGH)
    shiftOut(serial_, clock_, 0b10101010)
    GPIO.output(latch_, GPIO.LOW)
    print("Time tacke -> {}".format((datetime.now() - start).microseconds))

if __name__ == "__main__":
    main()
    #while True: pass
    GPIO.cleanup()
    exit()