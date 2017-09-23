#!/usr/bin/python
from gpiozero import Button, LED
from signal import pause
import os, sys, time

upGPIO = 20
offGPIO =  21
holdTime = 5
ledGPIO =  3

def when_pressed():

    led.blink(on_time=0.5, off_time=0.5)

def when_released():

    led.off()

def shutdown():
    os.system("sudo poweroff")

led = LED(ledGPIO)
up = LED(upGPIO)
up.on()
btn = Button(offGPIO, hold_time=holdTime)
btn.when_held = shutdown
btn.when_pressed = when_pressed
btn.when_released = when_released
pause()
