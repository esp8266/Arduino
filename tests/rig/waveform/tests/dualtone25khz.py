import sys
sys.path.append("/Users/earle/Documents/Arduino/hardware/esp8266com/esp8266/tests/rig/waveform") # Add pyserial dir to search path

from analyzer import WriteTest, Capture, DecodePWM
from time import sleep

# Reset the chip
WriteTest("z\n")
sleep(5)

WriteTest("r 1000\n")

freqs = [ 25000 ]# 1000, 5000, 10000, 20000, 30000, 40000

print("analogWriteFreq,analogWrite,Measured Cycles,Average Period,Average Duty Cycle,Period StdDev,Duty Cycle Stdev")
for f in freqs:
    WriteTest("f %d\n"%(f))
    for i in range(0, 1000, 10):
        WriteTest("a 5 %d\n"%(i))
        WriteTest("a 6 %d\n"%(1000 - i))
        csv = Capture(pins = 'D1', samples = 120000, samplerate = '24mhz', driver = 'fx2lafw')
        cycleCnt, avgPeriod, avgDuty, periods, highs, duties, stdP, stdD = DecodePWM(csv)
        print("%d,%d,%d,%.0f,%.4f,%.1f,%.4f"%(f, i, cycleCnt, avgPeriod, avgDuty, stdP, stdD))
 