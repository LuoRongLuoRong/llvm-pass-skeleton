cd ..

if [ ! -d build  ];then
  mkdir build
fi
cd build && rm -rf *
cmake ..
make

rtlib_dir='../runtimelib/src'
g++ -fPIC -c ${rtlib_dir}/rtlib.cpp

src_dir='../TinyWebServer'
pass_so_path='./skeleton/libSkeletonPass.so'

clang++ -fPIC -flegacy-pass-manager -O0 -g -fno-discard-value-names -Xclang -load -Xclang ${pass_so_path} -c ${src_dir}/server.ll

if [ ! -d ../output ];then
  mkdir ../output
fi
cd ../output
rm ./a.out
rm ./results.txt

build_src='../build'
g++ -fPIC ${build_src}/server.o ${build_src}/rtlib.o -lpthread -lmysqlclient 

echo 'You can run test_after_run.sh in a new terminal now.'
./a.out
