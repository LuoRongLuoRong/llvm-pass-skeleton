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
src_dir='../src/TinyWebServer'
cd ./build
cmake ..
make
cd ../instrument/
g++ -c rtlib.cpp
g++ -c -fPIC ${src_dir}/timer/lst_timer.cpp
g++ -c -fPIC ${src_dir}/http/http_conn.cpp
g++ -c -fPIC ${src_dir}/log/log.cpp
g++ -c -fPIC ${src_dir}/CGImysql/sql_connection_pool.cpp
g++ -c -fPIC ${src_dir}/webserver.cpp
g++ -c -fPIC ${src_dir}/config.cpp

clang++ -flegacy-pass-manager -O0 -g -fPIC -fno-discard-value-names -Xclang -load ../build/skeleton/libSkeletonPass.so -c ${src_dir}/main.cpp   

g++ main.o lst_timer.o http_conn.o log.o sql_connection_pool.o webserver.o config.o rtlib.o -lpthread -lmysqlclient

echo 'input a number:'
# ./a.out
