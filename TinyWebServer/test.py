import socket
import time

port = 9996
reqs = [
    'POST /2CGISQL.cgi HTTP/1.1\r\nHost: 10.177.21.53:9996\r\nConnection: keep-alive\r\nContent-length:21\r\n\r\nuser=123&password=123',
    'GET / HTTP/1.1\r\nHost:10.177.21.53:9996\r\nConnection:keep-alive\r\n\r\n',

]

def main():
    for req in reqs:
        so = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        so.connect(('10.177.21.53', port))
        print('send: \n{}'.format(req))
        so.send(req.encode())
        res = so.recv(10240).decode()
        print('recv: \n{}'.format(res.split('\n')[0]))
        time.sleep(1)
        
if __name__=='__main__':
    main()
    