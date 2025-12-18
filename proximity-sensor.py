#!/usr/bin/python3
#Name: Nahian Ibne Monir

from signal import signal, SIGTERM, SIGHUP, pause
from time import sleep
from threading import Thread
from gpiozero import DistanceSensor, RGBLED, Buzzer, Device, Button
from gpiozero.pins.pigpio import PiGPIOFactory

Device.pin_factory = PiGPIOFactory()

led = RGBLED(red=26, green=19, blue=13, active_high=False)
buzzer = Buzzer(24)
toggle = Button(25)


mute = False
reading = True
sensor = DistanceSensor(echo=20, trigger=21)

def on_off():
    global mute
    mute = not mute
toggle.when_pressed = on_off


def safe_exit(signum, frame):
    global reading
    reading = False

def read_distance():
    global mute
    while reading:
        distance_cm = sensor.distance * 100
        message = f"Distance: {distance_cm:.2f} cm"
        print(message)

        if distance_cm >= 20:
            led.color = (0, 1, 0)
            if not mute:
                buzzer.off()

        elif distance_cm >= 5:
            led.color = (1, 1, 0)
            if not mute:
                buzzer.beep(on_time=0.05, off_time=0.05, n=1)
        else:
            led.color= (1,0,0)
            if not mute:
                buzzer.on()
            else:
                buzzer.off()
        sleep(0.1)

try:
    signal(SIGTERM, safe_exit)
    signal(SIGHUP, safe_exit)


74
75 try:
76     signal(SIGTERM, safe_exit)
77     signal(SIGHUP, safe_exit)
78
79
80     reader = Thread(target=read_distance, daemon=True)
81     reader.start()
82
83     pause()
84
85 except KeyboardInterrupt:
86     pass
87
88 finally:
89     reading = False
90     reader.join()
91     led.off()
92     buzzer.off()
93     sensor.close()
    reader = Thread(target=read_distance, daemon=True)
    reader.start()

    pause()

except KeyboardInterrupt:
    pass

finally:
    reading = False
    reader.join()
    led.off()
    buzzer.off()
    sensor.close()
