# no blank when assign a value
# pre_name='test1cout'

# echo 'input test file name:'
# read test_file_name
# # if string is ""
# if [ -z "$test_file_name" ]; then
#   test_file_name=$pre_name
# fi

test_file_name='test1cout'
cd ./build
cmake ..
make
cd ../instrument/
clang++ -flegacy-pass-manager -Xclang -load -Xclang ../build/skeleton/libSkeletonPass.so ../src/$test_file_name.cpp

echo 'input a number:'
./a.out
