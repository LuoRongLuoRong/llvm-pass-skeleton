cd ..

if [ ! -d build  ];then
  mkdir build
fi
cd build && rm -rf *
cmake ..
make


rtlib_dir='../runtimelib/src'
g++ -fPIC -c ${rtlib_dir}/rtlib.cpp

project_name='WebServer'
project_ir_file_name='wserver'

src_dir='../input/WebServer'
echo ${src_dir}
pass_so_path='./skeleton/libSkeletonPass.so'

clang++ -fPIC -flegacy-pass-manager -O0 -g -fno-discard-value-names -Xclang -load -Xclang ${pass_so_path} -c ${src_dir}/${project_ir_file_name}.ll

if [ ! -d ../output ];then
  mkdir ../output
fi
cd ../output
rm ./wserver
rm ./results.txt
rm ./log.txt

build_src='../build'
g++ -fPIC -o ${project_ir_file_name} ${build_src}/${project_ir_file_name}.o ${build_src}/rtlib.o -lpthread -lmysqlclient -D_PTHREADS -Wno-unused-parameter
#  -g -O0 -fno-discard-value-names -lpthread -lmysqlclient 
#  -D_PTHREADS -Wno-unused-parameter

# echo 'You can run test_after_run.sh in a new terminal now.'
echo "build connection..."
echo "you can run test_webserver.sh"
./${project_ir_file_name} -t 5 -p 8882 -l /home/fdse/luorong/llvm-pass-skeleton/input/WebServer/WebServer.log
# webserver -t 5 -p 8882 -l /home/fdse/luorong/LLVM/llvm-pass-skeleton/output/webserver.log

