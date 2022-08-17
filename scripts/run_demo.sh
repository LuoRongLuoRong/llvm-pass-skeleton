cd ..

if [ ! -d build  ];then
  mkdir build
else
  echo dir exist
fi
cd build && rm -rf *
cmake ..
make

rtlib_dir='../runtimelib/src'
g++ -fPIC -c ${rtlib_dir}/rtlib.cpp

src_dir='../test'
pass_so_path='./skeleton/libSkeletonPass.so'

# build/skeleton/libSkeletonPass.so

clang++ -fPIC -emit-llvm -O0 -g -fno-discard-value-names -S -c ${src_dir}/demo.cpp -o ${src_dir}/demo.ll

clang++ -fPIC -flegacy-pass-manager -O0 -g -fno-discard-value-names -Xclang -load -Xclang ${pass_so_path} -c ${src_dir}/demo.ll


cd ../output
rm ./a.out
rm ./results.txt

build_src='../build'
g++ -fPIC ${build_src}/demo.o ${build_src}/rtlib.o

./a.out
