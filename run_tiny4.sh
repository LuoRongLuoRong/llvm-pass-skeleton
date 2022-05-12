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

src_dir='/home/fdse/luorong/LLVM/test/llvm-pass-skeleton/TinyWebServer'
pass_so_path='../../build/skeleton/libSkeletonPass.so'
server_ll_path='/home/fdse/luorong/LLVM/test/llvm-pass-skeleton/TinyWebServer'
compilation_record_path='/home/fdse/luorong/LLVM/test/llvm-pass-skeleton/instrument/compilation_record'

cd ./build
cmake ..
make
cd ../instrument/build

rm ./a.out
rm ./results.txt

g++ -fPIC -c ../rtlib.cpp

export LLVM_COMPILER=clang
CC=wllvm ./configure
# clang++ -flegacy-pass-manager -O0 -g -fPIC -fno-discard-value-names -Xclang -load -Xclang ${pass_so_path} -c
wllvm++ -flegacy-pass-manager -O0 -g -fPIC -fno-discard-value-names -Xclang -load -Xclang ${pass_so_path} -c ${src_dir}/main.cpp ${src_dir}/timer/lst_timer.cpp ${src_dir}/http/http_conn.cpp ${src_dir}/log/log.cpp ${src_dir}/CGImysql/sql_connection_pool.cpp ${src_dir}/webserver.cpp ${src_dir}/config.cpp
g++ main.o lst_timer.o http_conn.o log.o sql_connection_pool.o webserver.o config.o rtlib.o -lpthread -lmysqlclient

./a.out
