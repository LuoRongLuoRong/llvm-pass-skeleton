## （1） 编译 LLVM Pass 文件
cd ./build
rm -rf ./skeleton
cmake ..
make

cd ../instrument/build
rm ./a.out
rm ./results.txt
rm ./rtlib.o

g++ -fPIC -c ../rtlib.cpp

src_dir='../../TinyWebServer'
pass_so_path='../../build/skeleton/libSkeletonPass.so'

clang++ -fPIC -flegacy-pass-manager -O0 -g -fno-discard-value-names -Xclang -load -Xclang ${pass_so_path} -c ${src_dir}/server.ll

g++ -fPIC server.o rtlib.o -lpthread -lmysqlclient 

./a.out
