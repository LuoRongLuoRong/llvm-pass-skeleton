test_file_name='test1cout'

cd ../build/
cmake ..
make
cd ../instrument/
g++ -c rtlib.cpp
clang++ -flegacy-pass-manager -Xclang -load -Xclang ../build/skeleton/libSkeletonPass.so -c ../src/$test_file_name.cpp
g++ $test_file_name.o rtlib.o
./a.out
