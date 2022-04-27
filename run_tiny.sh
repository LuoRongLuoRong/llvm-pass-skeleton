# no blank when assign a value
# pre_name='test1cout'

# echo 'input test file name:'
# read test_file_name
# # if string is ""
# if [ -z "$test_file_name" ]; then
#   test_file_name=$pre_name
# fi

# test_file_name='test0add'
test_file_name='test1cout'
src_dir='../src/TinyWebServer'
cd ./build
cmake ..
make
cd ../instrument/
g++ -c rtlib.cpp

clang++ -flegacy-pass-manager -O0 -g -fno-discard-value-names -Xclang -load -Xclang ../build/skeleton/libSkeletonPass.so -c ${src_dir}/main.cpp ${src_dir}/timer/lst_timer.cpp ${src_dir}/http/http_conn.cpp ${src_dir}/log/log.cpp ${src_dir}/CGImysql/sql_connection_pool.cpp  ${src_dir}/webserver.cpp ${src_dir}/config.cpp

g++ $test_file_name.o rtlib.o

echo 'input a number:'
./a.out
