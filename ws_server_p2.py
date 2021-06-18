import asyncio
import websockets
import pickle
import json
import socket
import threading
from threading import Thread
import _thread
import time

HOST = '192.168.43.235'  # The socket server's hostname or IP address
PORT = 54088       # The port used by the server
Gateway_IP = '192.168.43.8'  # for websocket server

data = ''
connect = 0
data = "{\"right\":0,\"left\":0,\"up\":0,\"down\":0,\"hit\":0}"
async def hello(websocket, path):
    
    '''print('echo')
    async for message in websocket:
        print(message,'received from client')
        greeting = f"Hello {message}!"
        await websocket.send(greeting)
        print(f"> {greeting}")
    '''
    while True:
        data = s.recv(1024).decode('utf-8')
        print(data)
        time.sleep(0.0001)
        await websocket.send(data)
        

            
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind((HOST, PORT))

try:
    start_server = websockets.serve(hello, "127.0.0.1", 8867, ping_interval=None)
    asyncio.get_event_loop().run_until_complete(start_server)
    asyncio.get_event_loop().run_forever()
    print("GG")
    data = "{right:0,left:0,up:0,down:0,hit:0}"
except KeyboardInterrupt:
    s.close()
