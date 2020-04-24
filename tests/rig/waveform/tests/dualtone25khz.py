import sys
sys.path.append("/Users/earle/Documents/Arduino/hardware/esp8266com/esp8266/tests/rig/waveform") # Add pyserial dir to search path

import analyzer
from time import sleep

sut = analyzer.ESP8266(port = 'COM6')
la = analyzer.LogicAnalyzer()

sut.reset()
idle = sut.perfTest()
sut.analogWriteRange(1000)

freqs = [ 25000 ] # [ 1000, 5000, 10000, 20000, 30000, 40000 ]
print("MHZ = " + str(sut.cpuMHz()) )
print("IFO = " + str(sut.info()) )
print("analogWriteFreq,analogWrite,Measured Cycles,Average Period,Average Duty Cycle,Period StdDev,Duty Cycle Stdev,Pin2 Average Period,Pin2 Average Duty Cycle,Pin2 Period StdDev,Pin2 Duty Cycle Stdev,CPU Slowdown Factor")
for f in freqs:
    sut.analogWriteFreq(f)
    for i in range(0, 1000, 10):
        sut.analogWrite(5, i)
        sut.analogWrite(6, 1000-i)
        csv = la.capture(pins = 'D1,D2', samples = 120000, rate = '24mhz')
        cycleCnt, avgPeriod, avgDuty, periods, highs, duties, stdP, stdD = la.decodePWM(csv, 1)
        cycleCnt2, avgPeriod2, avgDuty2, periods2, highs2, duties2, stdP2, stdD2 = la.decodePWM(csv, 2)
        running = sut.perfTest()
        print("%d,%d,%d,%.0f,%.4f,%.1f,%.4f,%.0f,%.4f,%.1f,%.4f,%.2f" % (f, i, cycleCnt, avgPeriod, avgDuty, stdP, stdD, avgPeriod2, avgDuty2, stdP2, stdD2,(running / idle)))
 