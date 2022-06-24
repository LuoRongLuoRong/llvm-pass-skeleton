cd ./build
rm -rf *
cmake ..
make

rtlib_dir='../instrument'
g++ -fPIC -c ${rtlib_dir}/rtlib.cpp

src_dir='../src'
pass_so_path='./skeleton/libSkeletonPass.so'

# clang++ -fPIC -emit-llvm -O0 -g -fno-discard-value-names -S -c ${src_dir}/test.cpp -o ${src_dir}/demo.ll

clang++ -fPIC -flegacy-pass-manager -O0 -g -fno-discard-value-names -Xclang -load -Xclang ${pass_so_path} -c ${src_dir}/demo.cpp

cd ../output
rm ./a.out
rm ./results.txt

build_src='../build'
g++ -fPIC ${build_src}/demo.o ${build_src}/rtlib.o

./a.out
