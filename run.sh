# no blank when assign a value
# pre_name='test1cout'

# echo 'input test file name:'
# read test_file_name
# # if string is ""
# if [ -z "$test_file_name" ]; then
#   test_file_name=$pre_name
# fi

# test_file_name='test0add'
test_file_name='demo'
# test_file_name='test1cout'
cd ./build
cmake ..
make
cd ../instrument/
g++ -c rtlib.cpp
# clang++ -flegacy-pass-manager -O0 -g -fno-discard-value-names -Xclang -load -Xclang ../build/skeleton/libSkeletonPass.so -c ../src/$test_file_name.cpp
clang++ -flegacy-pass-manager -O0 -g -fno-discard-value-names -Xclang -load -Xclang ../build/skeleton/libSkeletonPass.so -c ../src/$test_file_name.ll

g++ $test_file_name.o rtlib.o

echo 'input a number:'
./a.out
