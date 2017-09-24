#!/usr/bin/python
from gpiozero import Button, LED
from signal import pause
import os, sys, time

upGPIO = 20
offGPIO =  21
holdTime = 5
ledGPIO =  16

def when_pressed():
    os.system("echo'Got a SHUTDOWN SIGNAL from the supervisor'>>/home/pi/log.txt"
    blinkFast()

def when_released():
    blinkSlow()

def shutdown():
    os.system("echo'Going for SHUTDOWN NOW'>>/home/pi/log.txt"
    os.system("sudo poweroff")

def blinkFast():
    led.blink(on_time=0.1, off_time=0.1)

def blinkSlow():
    led.blink(on_time=0.5, off_time=0.5)

led = LED(ledGPIO)
blinkSlow();
up = LED(upGPIO)
up.on()
btn = Button(offGPIO, hold_time=holdTime)
btn.when_held = shutdown
btn.when_pressed = when_pressed
btn.when_released = when_released
pause()
