cd ..

# 如果不存在 build 文件夹，就新建该文件夹。
if [ ! -d build  ];then
  mkdir build
fi
cd build && rm -rf *
# 编译 llvm-skeleton-pass 项目
cmake ..
make

# 编译运行时库函数（用于将变量信息存储到文件中）
rtlib_dir='../runtimelib/src'
g++ -fPIC -c ${rtlib_dir}/rtlib.cpp

src_dir='../input/TinyWebServer'
pass_so_path='./skeleton/libSkeletonPass.so'
# 进行插桩，生成 server.o 文件
clang++ -fPIC -flegacy-pass-manager -O0 -g -fno-discard-value-names -Xclang -load -Xclang ${pass_so_path} -c ${src_dir}/server.ll

# 如果不存在 output 文件夹，就新建该文件夹。
if [ ! -d ../output ];then
  mkdir ../output
fi
cd ../output
rm ./a.out
rm ./results.txt

build_src='../build'
g++ -fPIC ${build_src}/server.o ${build_src}/rtlib.o -lpthread -lmysqlclient 

echo 'You can run test_tinywebserver.sh in a new terminal now.'
./a.out
