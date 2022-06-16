src_dir='/home/fdse/luorong/LLVM/test/llvm-pass-skeleton/src/TinyWebServer'
skeleton_pass_so_path='../../build/skeleton/libSkeletonPass.so'
valuetrace_pass_so_path='../../build/skeleton/libValueTracePass.so'
compilation_record_path='/home/fdse/luorong/LLVM/test/llvm-pass-skeleton/instrument/compilation_record'

cd ./build
rm -rf ./skeleton
cmake ..
make
cd ../instrument/build

rm ./a.out
rm ./results.txt
rm ./rtlib.o

g++ -fPIC -c ../rtlib.cpp

export LLVM_COMPILER=clang
# CC=wllvm ./configure
# clang++ -flegacy-pass-manager -O0 -g -fPIC -fno-discard-value-names -Xclang -load -Xclang ${skeleton_pass_so_path} -c
wllvm++ -flegacy-pass-manager -O0 -g -fPIC -fno-discard-value-names -Xclang -load -Xclang ${skeleton_pass_so_path} -c ${src_dir}/main.cpp ${src_dir}/timer/lst_timer.cpp ${src_dir}/http/http_conn.cpp ${src_dir}/log/log.cpp ${src_dir}/CGImysql/sql_connection_pool.cpp ${src_dir}/webserver.cpp ${src_dir}/config.cpp

g++ main.o lst_timer.o http_conn.o log.o sql_connection_pool.o webserver.o config.o rtlib.o -lpthread -lmysqlclient

./a.out
