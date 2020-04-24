#!/usr/bin/env/python3

import sys
sys.path.insert(0, "/Users/earle/Documents/Arduino/hardware/esp8266com/esp8266/tools/pyserial") # Add pyserial dir to search path

import csv
import serial
import subprocess
import statistics
import time

SIGROKCLI = 'C:\Program Files (x86)\sigrok\sigrok-cli\sigrok-cli.exe'

class LogicAnalyzer:
    def __init__(self, driver = 'fx2lafw'):
        self.driver = driver
    
    def capture(self, pins, samples = 12000, rate = '24mhz'):
        cmd = [ SIGROKCLI, '-d', self.driver, '-C', pins, '-O', 'csv', '--samples', str(samples), '-c', 'samplerate=' + str(rate) ]
        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        out = proc.stdout.read()
        err = proc.stderr.read()
        code = proc.wait()
        if code != 0:
            return None
        ret = []
        for row in out.splitlines():
            if row.startswith(b';') or row.startswith(b'nano'):
                continue # Skip any comments and header line
            line = [ int(i) for i in row.split(b',') ]
            ret += [ line ]
        return ret

    def decodePWM(self, vals, col = 1):
        x = 0
        cycleCnt = 0
        totalPeriod = 0
        totalHigh = 0
        listPeriod = []
        listHigh = []
        listDutyCycle = []
        cnt = len(vals) - 1
    
        # Find a 0 to start looking for 1st 0->1 edge
        while (vals[x][col] != 0) and (x < cnt):
            x = x + 1

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
            # This was a DC signal
            cycleCnt = 1
            totalPeriod = vals[cnt][0] - vals[0][0]
            if vals[cnt][col]:
                totalHigh = totalPeriod
                listHigh = [ totalHigh ]
            stdevPeriod = 0
            stdevDuty = 0
        else:
            stdevPeriod = statistics.stdev(listPeriod)
            stdevDuty = statistics.stdev(listDutyCycle)
            
        return cycleCnt, totalPeriod / cycleCnt, totalHigh / totalPeriod, listPeriod, listHigh, listDutyCycle, stdevPeriod, stdevDuty


class ESP8266:
    def __init__(self, port = 'COM6'):
        self._port = port
        pass

    def _comm(self, command, checkResp = True):
        ser = serial.Serial(self._port, 115200, timeout = 5, parity = serial.PARITY_NONE)
        ser.write(bytearray(command, 'ascii'))
        ser.flush()
        resp = ser.readline().decode("utf-8")
        ser.close()
        if checkResp:
            resp = resp.strip()
            if resp[0] == '+':
                return resp[1:]
            elif resp[0] == '-':
                # error!
                raise Exception("SUT error: " + resp[1:])
            else:
                # Garbage back from SUT
                raise Exception("Garbage from SUT: '" + resp + "'")
        return ""

    def reset(self):
        self._comm('z\n', checkResp = False)
        time.sleep(2)
        
    def analogWriteRange(self, max):
        self._comm('r %d\n' % (max))

    def analogWriteFreq(self, freq):
        self._comm('f %d\n' % (freq))

    def digitalWrite(self, pin, val):
        self._comm('d %d %d\n' % (pin, val))
    
    def analogWrite(self, pin, val):
        self._comm('a %d %d\n' % (pin, val))
    
    def tone(self, pin, f):
        self._comm('t %d %d\n' % (pin, val))
    
    def perfTest(self):
        return int(self._comm('c\n'))

    def cpuMHz(self):
        return int(self._comm('i\n').split(' ')[0])

    def info(self):
        return self._comm('i\n').split(' ')[1]