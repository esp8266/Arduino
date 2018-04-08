#!/usr/bin/env python
from __future__ import print_function
import pexpect
from pexpect import EOF, TIMEOUT, fdpexpect
import sys
import os
import time
import argparse
import serial
import subprocess
import imp
from urlparse import urlparse
from junit_xml import TestSuite, TestCase
try:
    from cStringIO import StringIO
except:
    from StringIO import StringIO
import mock_decorators

debug = False

sys.path.append(os.path.abspath(__file__))

def debug_print(*args, **kwargs):
    if not debug:
        return
    print(file=sys.stderr, *args, **kwargs)

class BSTestRunner(object):

    SUCCESS = 0
    FAIL = 1
    TIMEOUT = 2
    CRASH = 3
    BEGINTIMEOUT = 4

    def __init__(self, spawn_obj, name, mocks):
        self.sp = spawn_obj
        self.tests = []
        self.reset_timeout = 2
        self.name = name
        self.mocks = mocks

    def get_test_list(self):
        self.sp.sendline('-1')
        self.tests = []
        timeout = 100
        while timeout > 0:
            res = self.sp.expect(['>>>>>bs_test_menu_begin', EOF, TIMEOUT])
            if res == 0:
                break
            timeout-=1
            time.sleep(0.1)
        if timeout <= 0:
            debug_print('begin timeout')
            return
        debug_print('got begin')
        while True:
            res = self.sp.expect([r'>>>>>bs_test_item id\=(\d+) name\="([^"]*?)" desc="([^"]*?)"',
                                  '>>>>>bs_test_menu_end',
                                  EOF, TIMEOUT])
            if res == 0:
                m = self.sp.match
                t = {'id': m.group(1), 'name': m.group(2), 'desc': m.group(3)}
                self.tests.append(t)
                debug_print('added test', t)
            elif res == 1:
                break
            elif res >= 2:
                time.sleep(0.1)

        debug_print('got {} tests'.format(len(self.tests)))

    def run_tests(self):
        test_cases = []
        for test in self.tests:
            desc = test['desc']
            name = test['name']
            index = test['id']
            test_case = TestCase(name, self.name)
            if '[.]' in desc:
                print('skipping test "{}"'.format(name))
                test_case.add_skipped_info(message="Skipped test marked with [.]")
            else:
                test_output = StringIO()
                self.sp.logfile = test_output
                if name in self.mocks:
                    print('setting up mocks')
                    self.mocks[name]['setup']()
                t_start = time.time()
                result = self.run_test(index)
                if name in self.mocks:
                    print('tearing down mocks')
                    self.mocks[name]['teardown']()
                t_stop = time.time()
                self.sp.logfile = None
                test_case.elapsed_sec = t_stop - t_start
                debug_print('test output was:')
                debug_print(test_output.getvalue())
                if result == BSTestRunner.SUCCESS:
                    test_case.stdout = test_output.getvalue()
                    print('test "{}" passed'.format(name))
                else:
                    print('test "{}" failed'.format(name))
                    test_case.add_failure_info('Test failed', output=test_output.getvalue())
                test_output.close()
            test_cases += [test_case];
        return TestSuite(self.name, test_cases)

    def run_test(self, index):
        self.sp.sendline('{}'.format(index))
        timeout = 10
        while timeout > 0:
            res = self.sp.expect(['>>>>>bs_test_start', EOF, TIMEOUT])
            if res == 0:
                break
            time.sleep(0.1)
            timeout -= 0.1
        if timeout <= 0:
            return BSTestRunner.BEGINTIMEOUT
        while timeout > 0:
            res = self.sp.expect([r'>>>>>bs_test_check_failure line=(\d+)',
                            r'>>>>>bs_test_end line=(\d+) result=(\d+) checks=(\d+) failed_checks=(\d+)',
                            TIMEOUT,
                            EOF,
                            'Exception',
                            'ets Jan  8 2013',
                            'wdt reset'])
            if res == 0:
                continue
            elif res == 1:
                test_result = self.sp.match.group(2)
                if test_result == '1':
                    return BSTestRunner.SUCCESS
                else:
                    if self.sp.match.group(1) != '0':
                        time.sleep(1.0)
                        self.sp.expect([TIMEOUT,
                                        'wdt reset',
                                        'Exception',
                                        'Panic',
                                        'Abort',
                                        'Soft WDT',
                                        EOF], timeout=self.reset_timeout)
                    return BSTestRunner.FAIL
            elif res == 2 or res == 3:
                time.sleep(0.1)
                timeout -= 0.1
                continue
            elif res > 3:
                return BSTestRunner.CRASH
        if timeout <= 0:
            return BSTestRunner.TIMEOUT

ser = None

def spawn_port(port_name, baudrate=115200):
    global ser
    ser = serial.serial_for_url(port_name, baudrate=baudrate)
    return fdpexpect.fdspawn(ser, 'wb', timeout=0)

def spawn_exec(name):
    return pexpect.spawn(name, timeout=0)

def run_tests(spawn, name, mocks):
    tw = BSTestRunner(spawn, name, mocks)
    tw.get_test_list()
    return tw.run_tests()

def parse_args():
    parser = argparse.ArgumentParser(description='BS test runner')
    parser.add_argument('-d', '--debug', help='Send test output to stderr', action='store_true')
    parser.add_argument('-p', '--port', help='Talk to the test over serial')
    parser.add_argument('-e', '--executable', help='Talk to the test executable')
    parser.add_argument('-n', '--name', help='Test run name')
    parser.add_argument('-o', '--output', help='Output JUnit format test report')
    parser.add_argument('-m', '--mock', help='Set python script to use for mocking purposes')
    return parser.parse_args()

def main():
    args = parse_args()
    spawn_func = None
    spawn_arg = None
    if args.port is not None:
        spawn_func = spawn_port
        spawn_arg = args.port
    elif args.executable is not None:
        spawn_func = spawn_exec
        spawn_arg = args.executable
    name = args.name or ""
    global debug
    if args.debug:
        debug = True
    if spawn_func is None:
        debug_print("Please specify port or executable", file=sys.stderr)
        return 1
    mocks = {}
    if args.mock is not None:
        mocks_mod = imp.load_source('mocks', args.mock)
        mocks = mock_decorators.env
    with spawn_func(spawn_arg) as sp:
        ts = run_tests(sp, name, mocks)
        if args.output:
            with open(args.output, "w") as f:
                TestSuite.to_file(f, [ts])
        return 0

if __name__ == '__main__':
    sys.exit(main())
