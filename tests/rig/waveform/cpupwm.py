import sys
sys.path.append("/Users/earle/Documents/Arduino/hardware/esp8266com/esp8266/tests/rig/waveform") # Add pyserial dir to search path

import analyzer
from time import sleep
import random

sut = analyzer.ESP8266(port = 'COM3')
la = analyzer.LogicAnalyzer()

sut.reset()
idle = sut.perfTest()

freqs = [ 20000 ] # [ 1000, 5000, 10000, 20000, 30000, 40000 ]
print("MHZ = " + str(sut.cpuMHz()) )
print("IFO = " + str(sut.info()) )
print("Signals,CPU Slowdown Factor")
sut.reset()
for f in freqs:
    for signals in range(1, 5+1, 1):
        running = 0;
        random.seed(123)
        sut.reset()
        sut.analogWriteFreq(f)
        for i in range(0, 100, 1):
            for pin in range(1, signals + 1, 1):
                sut.analogWrite(pin, random.randint(0,1024))
            running += sut.perfTest()
        running = running / 100.0
        print("%d, %0.5f" % ( signals, running / idle) )

