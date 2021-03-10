#!/usr/bin/env python3

import os
import asyncio

# 512 bytes
message = bytearray(512);
bufsize=len(message)
print('message len=', bufsize)

global recv
recv = 0

async def tcp_echo_open (ip, port):
    return await asyncio.open_connection(ip, port)

async def tcp_echo_sender(message, writer):
    print('Writer started')
    while True:
        writer.write(message)
        await writer.drain()

async def tcp_echo_receiver(message, reader):
    global recv
    print('Reader started')
    while True:
        data = ''.encode('utf8')
        while len(data) < bufsize:
            data += await reader.read(bufsize - len(data))
        recv += len(data);
        if data != message:
            print('error')

async def tcp_stat():
    global recv
    dur = 0
    loopsec = 2
    while True:
        last = recv
        await asyncio.sleep(loopsec) # drifting
        dur += loopsec
        print('BW=', (recv - last) * 2 * 8 / 1024 / loopsec, 'Kibits/s avg=', recv * 2 * 8 / 1024 / dur)

loop = asyncio.get_event_loop()
reader, writer = loop.run_until_complete(tcp_echo_open('echo23.local', 23))
loop.create_task(tcp_echo_receiver(message, reader))
loop.create_task(tcp_echo_sender(message, writer))
loop.create_task(tcp_stat())
loop.run_forever()
