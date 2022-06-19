## （1） 编译 LLVM Pass 文件
cd ./build
rm -rf ./skeleton
cmake ..
make

cd ../instrument/build

g++ -fPIC -c ../rtlib.cpp

src_dir='/home/fdse/luorong/LLVM/test/llvm-pass-skeleton/src'
pass_so_path='../../build/skeleton/libSkeletonPass.so'

clang++ -fPIC -emit-llvm -S -c ${src_dir}/demo.cpp -o ${src_dir}/demo.ll

clang++ -flegacy-pass-manager -O0 -g -fPIC -fno-discard-value-names -Xclang -load -Xclang ${pass_so_path} -c ${src_dir}/demo.ll

g++ -fPIC demo.o

./a.out
