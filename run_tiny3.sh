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
cd ./build
cmake ..
make
cd ../instrument/build
g++ -c ../rtlib.cpp

clang++ -flegacy-pass-manager -O0 -g -fPIC -fno-discard-value-names -Xclang -load ${pass_so_path} -c ${src_dir}/main.cpp   

clang++ -flegacy-pass-manager -O0 -g -fPIC -fno-discard-value-names -Xclang -load ${pass_so_path} -c ${src_dir}/timer/lst_timer.cpp

clang++ -flegacy-pass-manager -O0 -g -fPIC -fno-discard-value-names -Xclang -load ${pass_so_path} -c ${src_dir}/http/http_conn.cpp

clang++ -flegacy-pass-manager -O0 -g -fPIC -fno-discard-value-names -Xclang -load ${pass_so_path} -c ${src_dir}/log/log.cpp

clang++ -flegacy-pass-manager -O0 -g -fPIC -fno-discard-value-names -Xclang -load ${pass_so_path} -c ${src_dir}/CGImysql/sql_connection_pool.cpp

clang++ -flegacy-pass-manager -O0 -g -fPIC -fno-discard-value-names -Xclang -load ${pass_so_path} -c ${src_dir}/webserver.cpp

clang++ -flegacy-pass-manager -O0 -g -fPIC -fno-discard-value-names -Xclang -load ${pass_so_path} -c ${src_dir}/config.cpp

g++ main.o lst_timer.o http_conn.o log.o sql_connection_pool.o webserver.o config.o rtlib.o -lpthread -lmysqlclient 

# echo 'input a number:'
# ./a.out
