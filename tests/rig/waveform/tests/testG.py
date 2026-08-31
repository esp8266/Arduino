#!/usr/bin/env python3

# Implements a test pattern:
# - 2 fans at 50% duty cycle, f=25kHz
# - 2 servos at 90 degrees
# - LED at 1020/1024
# - Tone of either 1760Hz or 220Hz

FAN1   = 1
FAN2   = 2
SERVO1 = 3
SERVO2 = 4
LED    = 5
TONE   = 6

import sys
sys.path.append("/Users/earle/Documents/Arduino/hardware/esp8266com/esp8266/tests/rig/waveform") # Add pyserial dir to search path

import analyzer
from time import sleep

sut = analyzer.ESP8266(port = 'COM3')
la = analyzer.LogicAnalyzer()

sut.reset()
idle = sut.perfTest() # Get idle performance
print("MHZ = " + str(sut.cpuMHz()) )
print("IFO = " + str(sut.info()) )
sut.analogWriteFreq(25000)
sut.analogWrite(FAN1, 512)
sut.analogWrite(FAN2, 512)
sut.servoAttach(SERVO1, 90)
sut.servoAttach(SERVO2, 90)
sut.analogWrite(LED, 1020)
sut.tone(TONE, 1760)
running = sut.perfTest() # Under load CPU perf
print("CPU = %0.2f" % (running / idle))

csv = la.capture(pins = 'D1,D2,D3,D4,D5,D6', samples = 12000000, rate = '24mhz')
print("pin,cycles measured,average period ns,average f,standard deviation ns,average duty cycle ns,standard deviation ns")
pins = ["FAN1","FAN2","SERVO1","SERVO2","LED","TONE"]
pinNo = 1
for pin in pins:
    cycleCnt, avgPeriod, avgDuty, periods, highs, duties, stdP, stdD = la.decodePWM(csv, pinNo)
    pinNo = pinNo + 1
    print(",".join(map(str, [pin, cycleCnt, avgPeriod, 1e9/avgPeriod, stdP, avgDuty, stdD])))

print("\n\nRAW DATA")
print(",".join(['ns'] + pins))
for l in csv:
    print(",".join(map(str, l)))

