# no blank when assign a value
# pre_name='test1cout'

# echo 'input test file name:'
# read test_file_name
# # if string is ""
# if [ -z "$test_file_name" ]; then
#   test_file_name=$pre_name
# fi

# test_file_name='test0add'
# test_file_name='test1cout'

src_dir='../../src/TinyWebServer'
pass_so_path='../../build/skeleton/libSkeletonPass.so'
server_ll_path='../../src/TinyWebServer'

cd ./build
cmake ..
make
cd ../instrument/build
g++ -c ../rtlib.cpp

project=" main.cpp ./timer/lst_timer.cpp ./http/http_conn.cpp ./log/log.cpp ./CGImysql/sql_connection_pool.cpp webserver.cpp config.cpp "

clang++ -flegacy-pass-manager -O0 -g -fPIC -fno-discard-value-names -Xclang -load -Xclang ${pass_so_path} -c ${src_dir}/server.ll
# ${src_dir}/main.cpp 
# ${src_dir}/timer/lst_timer.cpp ${src_dir}/http/http_conn.cpp ${src_dir}/log/log.cpp ${src_dir}/CGImysql/sql_connection_pool.cpp ${src_dir}/webserver.cpp ${src_dir}/config.cpp 
# -o tiny3serevr -lpthread -lmysqlclient 

# opt -load ${pass_so_path} < ${server_ll_path}/server.ll > /dev/null

# g++ main.o lst_timer.o http_conn.o log.o sql_connection_pool.o webserver.o config.o rtlib.o -lpthread -lmysqlclient 

# echo 'input a number:'
# ./a.out
