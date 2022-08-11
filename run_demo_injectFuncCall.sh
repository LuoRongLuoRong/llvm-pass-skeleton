export LLVM_DIR=/home/fdse/node_modules/llvm-13.0.0.obj/

cd ./build
rm -rf *
cmake ..
make

src_dir='../src'
echo "Start loading Pass..."
# opt -enable-new-pm=0 -load ./skeleton/libInjectFuncCall.so -legacy-inject-func-call ${src_dir}/demo.ll -o demo.bin
opt -load-pass-plugin ./skeleton/libInjectFuncCall.so --passes="inject-func-call" ../src/demo.ll -o demo.bin
echo "Finish loading Pass!"

echo "Start compiling codes..."
lli demo.bin
echo "Finish compiling codes!"