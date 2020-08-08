#!/usr/bin/env python3

import os
import asyncio

# 512 bytes
message = 'abcdefghijklmnopabcdefghijklmnopabcdefghijklmnopabcdefghijklmnopabcdefghijklmnopabcdefghijklmnopabcdefghijklmnopabcdefghijklmnopabcdefghijklmnopabcdefghijklmnopabcdefghijklmnopabcdefghijklmnopabcdefghijklmnopabcdefghijklmnopabcdefghijklmnopabcdefghijklmnopabcdefghijklmnopabcdefghijklmnopabcdefghijklmnopabcdefghijklmnopabcdefghijklmnopabcdefghijklmnopabcdefghijklmnopabcdefghijklmnopabcdefghijklmnopabcdefghijklmnopabcdefghijklmnopabcdefghijklmnopabcdefghijklmnopabcdefghijklmnopabcdefghijklmnopabcdefghijklmnop'.encode('utf8') #bytearray(os.urandom(bufsize))
bufsize=len(message)
print('message len=', bufsize)

global recv
recv = 0

async def tcp_echo_open (ip, port, loop):
    return await asyncio.open_connection(ip, port, loop=loop)

async def tcp_echo_sender(message, writer, loop):
    print('Writer started')
    while True:
        writer.write(message)
        await writer.drain()

async def tcp_echo_receiver(message, reader, loop):
    global recv
    print('Reader started')
    while True:
        data = ''.encode('utf8')
        while len(data) < bufsize:
            data += await reader.read(bufsize - len(data))
        recv += len(data);
        if data != message:
            print('error')

async def tcp_stat(loop):
    global recv
    while True:
        last = recv
        await asyncio.sleep(2)
        print('BW=', (recv - last) * 8 / 1024, 'Kibits/s')

loop = asyncio.get_event_loop()
reader, writer = loop.run_until_complete(tcp_echo_open('echo23.local', 23, loop))
loop.create_task(tcp_echo_receiver(message, reader,loop))
loop.create_task(tcp_echo_sender(message, writer, loop))
loop.create_task(tcp_stat(loop))
loop.run_forever()
