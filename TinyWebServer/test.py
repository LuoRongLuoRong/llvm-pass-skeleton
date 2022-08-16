import socket
import time

port = 9996
reqs = [
    'POST /2CGISQL.cgi HTTP/1.1\r\nHost: 10.177.21.53:9996\r\nConnection: keep-alive\r\nContent-length:21\r\n\r\nuser=123&password=123',
    'GET / HTTP/1.1\r\nHost:10.177.21.53:9996\r\nConnection:keep-alive\r\n\r\n',

]



def main():
    # count = [2500, 5000, 10000, 25000, 50000]
    count = [2500]
    for i in count:
        so = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        so.connect(('10.177.21.53', port))
        total_time = 0
        for j in range(0, i):
            for req in reqs:
                # print('send: \n{}'.format(req))
                cur = time.time()
                so.send(req.encode())
                res = so.recv(10240).decode()
                total_time += time.time()-cur
                print('recv: \n{}'.format(res.split('\n')[0]))
                time.sleep(0.1)
        print("send {} times total time: {}".format(i, total_time))
if __name__=='__main__':
    main()
    