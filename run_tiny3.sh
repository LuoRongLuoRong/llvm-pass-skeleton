src_dir='/home/fdse/luorong/LLVM/test/llvm-pass-skeleton/TinyWebServer'
pass_so_path='../../build/skeleton/libSkeletonPass.so'
server_ll_path='../../src/TinyWebServer'
compilation_record_path='/home/fdse/luorong/LLVM/test/llvm-pass-skeleton/instrument/compilation_record'

cd ./build
cmake ..
make
cd ../instrument/build
g++ -fPIC -c ../rtlib.cpp

clang++ -flegacy-pass-manager -O0 -g -fPIC -fno-discard-value-names -Xclang -load -Xclang ${pass_so_path} -c ${src_dir}/server.ll

g++ server.o rtlib.o -lpthread -lmysqlclient 

./a.out -p 9989