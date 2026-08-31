#!/usr/bin/env python3

# Implements a test which runs from 1..8 PWM signals at 20kHZ
# Run each # of signals 50 times with repeatable, random values on all pins
# Report the average CPU slowdown and the average frequency of each pin (to show if things slow down as PWM pins added)

import sys
sys.path.append("/Users/earle/Documents/Arduino/hardware/esp8266com/esp8266/tests/rig/waveform") # Add pyserial dir to search path

import analyzer
from time import sleep
import random

sut = analyzer.ESP8266(port = 'COM3')
la = analyzer.LogicAnalyzer()

sut.reset()
idle = sut.perfTest()

RUNS = 50

freqs = [ 20000 ] # [ 1000, 5000, 10000, 20000, 30000, 40000 ]
print("MHZ = " + str(sut.cpuMHz()) )
print("IFO = " + str(sut.info()) )
print("Signals,CPU Slowdown Factor,PWM Frequency")
sut.reset()
for f in freqs:
    for signals in range(1, 9, 1):
        running = 0;
        random.seed(123 * signals)
        sut.reset()
        sut.analogWriteFreq(f)
        totalPeriods = [[],[],[],[],[],[],[],[]]
        for i in range(0, RUNS, 1):
            for pin in range(0, signals, 1):
                a = random.randint(0,1024)
                sut.analogWrite(pin, a)
            csv = la.capture(pins = 'D0,D1,D2,D3,D4,D5,D6,D7', samples = 24000, rate = '24mhz')
            for s in range (0, signals, 1):
                cycleCnt, avgPeriod, avgDuty, periods, highs, duties, stdP, stdD = la.decodePWM(csv, s + 1)
                totalPeriods[s] += periods
            running += sut.perfTest()
        running = running / RUNS
        print("%d,%0.5f," % ( signals, running / idle), end="" )
        for s in range (0, signals, 1):
            avgPeriod = sum(totalPeriods[s])/len(totalPeriods[s])
            print("%0.f," % (1e9 / avgPeriod), end="")
        print("")

