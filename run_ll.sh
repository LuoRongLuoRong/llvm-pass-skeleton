cd ./src/TinyWebServer
export LLVM_COMPILER=clang
wllvm++ -o server main.cpp timer/lst_timer.cpp http/http_conn.cpp log/log.cpp CGImysql/sql_connection_pool.cpp webserver.cpp config.cpp -g -O0 -fno-discard-value-names -lpthread -lmysqlclient
extract-bc server
llvm-dis server.bc 
