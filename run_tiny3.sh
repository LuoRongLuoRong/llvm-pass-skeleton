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

clang++ -flegacy-pass-manager -O0 -g -fPIC -fno-discard-value-names -Xclang -load -Xclang ${skeleton_pass_so_path} -c ${src_dir}/server.ll > a.txt

g++ server.o rtlib.o -lpthread -lmysqlclient 

./a.out
