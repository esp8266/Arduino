#!/usr/bin/env/python3

import sys
sys.path.insert(0, "/Users/earle/Documents/Arduino/hardware/esp8266com/esp8266/tools/pyserial") # Add pyserial dir to search path

import csv
import serial
import subprocess
import statistics
import time

SIGROKCLI = 'C:\Program Files (x86)\sigrok\sigrok-cli\sigrok-cli.exe'

def WriteTest(command, port = 'COM6'):
    ser = serial.Serial(port, 115200, timeout=5, parity=serial.PARITY_NONE)
    ser.write(bytearray(command, 'ascii'))
    ser.flush()
    ser.close()

def Capture(pins='D5=p', samples=120000, samplerate='24mhz', driver='fx2lafw'):
    cmd = [ SIGROKCLI, '-d', driver, '-C', pins, '-O', 'csv', '--samples', str(samples), '-c', 'samplerate='+str(samplerate) ]
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out = proc.stdout.read()
    err = proc.stderr.read()
    code = proc.wait()
    if code != 0:
        return None
    ret = []
    for row in out.splitlines():
        if row.startswith(b';') or row.startswith(b'nano'):
            continue
        line = [ int(i) for i in row.split(b',') ]
        ret += [ line ]
    return ret

def DecodePWM(vals, col = 1):
    x = 0
    cycleCnt = 0
    totalPeriod = 0
    totalHigh = 0
    listPeriod = []
    listHigh = []
    listDutyCycle = []
    cnt = len(vals) - 1

    # Must start looking on a 0->1 to ensure full cycle sample start
    while (vals[x][col] == 0) and (x < cnt):
        x = x + 1
    
    while x < cnt:
        # 0->1 transition here
        h = vals[x][0]
        # Look for 1->0 transition
        while (vals[x][col] == 1) and (x < cnt):
            x = x + 1
        if x < cnt:
            # Found 1->0 transition, record it
            l = vals[x][0]
            timeHigh = l - h
            # Now look for 0->1 so we can get period
            while (vals[x][col] == 0) and (x < cnt):
                x = x + 1
            if x < cnt:
                period = vals[x][0] - h
                totalPeriod = totalPeriod + period
                totalHigh = totalHigh + timeHigh
                listPeriod += [ period ]
                listHigh += [ timeHigh ]
                listDutyCycle += [ timeHigh / period ]
                cycleCnt = cycleCnt + 1
    
    if cycleCnt == 0:
        return 0, 0, 0, [], [], [], 0, 0

    return cycleCnt, totalPeriod / cycleCnt, totalHigh / totalPeriod, listPeriod, listHigh, listDutyCycle, statistics.stdev(listPeriod), statistics.stdev(listDutyCycle)

