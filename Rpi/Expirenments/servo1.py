# !usr/bin/python

# Import necessary liblaries
import RPi.GPIO as GPIO
import time

# Set GPOIO numbering mode
GPIO.setmode(GPIO.BOARD)

# set connectionpin to output
GPIO.setup(11, GPIO.OUT)
servo1 = GPIO.PWM(11, 50) # set pin 11 to 50Hz PWM mode

# start PWM running but with value of 0
servo1.start(0)
print("Waiting for 2 seconds")
time.sleep(2)

# Moving the sevo
while True:
	try:
		duty = int(input("Enter angle in degrees -> "))
		duty = 2 + (duty/180) * 10
		servo1.ChangeDutyCycle(duty)
		#time.sleep(0.5)
		#servo1.ChangeDutyCycle(0)
	except:
		break




# clean pins
servo1.stop()
GPIO.cleanup()

print("End of the program")
