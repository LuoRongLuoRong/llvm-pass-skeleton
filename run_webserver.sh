cd ./build
rm -rf *
cmake ..
make

rtlib_dir='../instrument'
g++ -fPIC -c ${rtlib_dir}/rtlib.cpp

project_name='WebServer'
project_ir_file_name='webserver'

src_dir='../'${project_name}
echo src_dir
pass_so_path='./skeleton/libSkeletonPass.so'

clang++ -fPIC -flegacy-pass-manager -O0 -g -fno-discard-value-names -Xclang -load -Xclang ${pass_so_path} -c ${src_dir}/${project_ir_file_name}.ll

cd ../output
rm ./webserver
rm ./results.txt

build_src='../build'
# g++ -fPIC ${build_src}/${project_name}.o ${build_src}/rtlib.o -lpthread -lmysqlclient 
g++ -o webserver -fPIC ${build_src}/${project_ir_file_name}.o ${build_src}/rtlib.o -lpthread -lmysqlclient -D_PTHREADS -Wno-unused-parameter
#  -g -O0 -fno-discard-value-names -lpthread -lmysqlclient 
#  -D_PTHREADS -Wno-unused-parameter

# echo 'You can run test_after_run.sh in a new terminal now.'
echo "build connection"
./webserver -t 5 -p 8882 -l /home/fdse/luorong/LLVM/test/llvm-pass-skeleton/output/webserver.log

