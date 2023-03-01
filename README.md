# llvm-pass-skeleton

基于 LLVM 13 及以上版本。

## 运行方式
#### （1）首先运行 build.sh；
build.sh : 编译整个项目，生成动态链接库文件 `libSkeletonPass.so`，其位于 `./build/skeleton/libSkeletonPass.so` 。

#### （2）然后运行 run_ll.sh；
run_ll.sh : 使用 WLLVM 打包整个 `TinyWebserver` 项目，生成其中间表示 `server.ll`，其位于 `./TinyWebServer/server.ll`。

#### （3）最后运行 run.sh；
run.sh : 编译 `TinyWebserver` 项目并运行该项目。该脚本主要包括如下三部分：
(I) 使用 `Clang++`，输入为 `server.ll` 和 `libSkeletonPass.so`，输出为目标文件 `server.o`；
(II) 使用 `g++` 链接 `server.o` 和运行时库函数 `rtlib.o`，输出为可执行文件 `./a.out`；
(III) 运行可执行文件 `./a.out`，启动 `TinyWebserver` 项目，等待客户端发起请求。

如果整个项目已经运行过，并且不需要修改代码，我们可以仅运行 `./a.out`。

#### 测试：发起单个请求。
新开一个 console，运行文件夹 `TinyWebserver` 下的 test.py 文件，可发起 POST 请求或者 GET 请求。
此时，服务器会打印相应状态变量的信息。

# 项目框架

## 文件夹
build：项目编译过程中产生的中间文件。
instrument：运行时库函数，即被插桩的文件。
jsonutil：读取 json 文件的代码。
skeleton：LLVM Pass 代码，本项目的核心文件。
TinyWebserver：开源项目。

## 脚本
build.sh：用于编译插桩代码。
run_ll.sh：构建开源项目 ll 文件。
run.sh：编译并运行（被插桩的）开源项目。

## 输入和输出
- 输入是与可执行文件同目录的 SVsite.json，其内容如下：
```json
[
    {
        "LOC": 348,
        "SV": "m_check_state",
        "filepath": "http/http_conn.cpp"
    },
    {
        "LOC": 141,
        "SV": "m_check_state",
        "filepath": "http/http_conn.cpp"
    },
    {
        "LOC": 353,
        "SV": "m_check_state",
        "filepath": "http/http_conn.cpp"
    },
    {
        "LOC": 285,
        "SV": "m_check_state",
        "filepath": "http/http_conn.cpp"
    },
    {
        "LOC": 296,
        "SV": "m_check_state",
        "filepath": "http/http_conn.cpp"
    }
]

```

- 输出是与可执行文件同目录的 results.txt，其内容如下：
```
http/http_conn.cpp, 141, m_check_state, 0, 0
http/http_conn.cpp, 348, m_check_state, 0, 0
http/http_conn.cpp, 353, m_check_state, 0, 0
http/http_conn.cpp, 285, m_check_state, 0, 1
http/http_conn.cpp, 348, m_check_state, 1, 1
http/http_conn.cpp, 353, m_check_state, 1, 1
http/http_conn.cpp, 348, m_check_state, 1, 1
http/http_conn.cpp, 353, m_check_state, 1, 1
http/http_conn.cpp, 348, m_check_state, 1, 1
http/http_conn.cpp, 353, m_check_state, 1, 1
http/http_conn.cpp, 348, m_check_state, 1, 1
http/http_conn.cpp, 353, m_check_state, 1, 1
http/http_conn.cpp, 296, m_check_state, 1, 2
http/http_conn.cpp, 348, m_check_state, 2, 2
http/http_conn.cpp, 353, m_check_state, 2, 2

```
以上是发送一条 POST 请求的结果。





# NOTICE
由于我们在整个项目中，路径是十分重要的，所以我们需要注意文件的分隔符。

# WebServer 测试结果记录


# Scripts

- TinyWebServer
- 
