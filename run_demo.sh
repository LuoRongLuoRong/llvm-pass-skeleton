export LLVM_DIR=/home/fdse/node_modules/llvm-13.0.0.obj/

echo "(INFO) Start compiling the whole project..."
cd ./build
rm -rf *
cmake ..
make
echo "(INFO) Finish  compiling the whole project..."

src_dir='../src'
pass_so_path='./skeleton/libSkeletonPass.so'

clang++ -fPIC -emit-llvm -O0 -g -fno-discard-value-names -S -c ${src_dir}/demo.cpp -o ${src_dir}/demo.ll
touch aaa.txt
objdump -T ${pass_so_path} | grep logint
# objdump -T ${pass_so_path} > aaa.txt

echo "(INFO) Start loading Pass..."
opt -O0 -load-pass-plugin ${pass_so_path} --passes="skeleton" ${src_dir}/demo.ll -o demo

echo "(INFO) Finish loading Pass!"

echo "(INFO) Start compiling codes..."
lli -O0 demo
echo "(INFO) Finish compiling codes!"
