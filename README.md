# llvm-pass-skeleton

基于 LLVM 13及以上版本。

## 运行方式
（1）首先运行 build.sh；
（2）然后运行 run_ll.sh；
（3）最后运行 run.sh；

测试：运行文件夹 TinyWebserver 下的 test.py 文件。

# 项目框架

## 文件夹
build：项目编译过程中产生的中间文件。
instrument：运行时库函数，即被插桩的文件。
readjson：读取 json 文件的代码。
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
