#g++ -c rtlib.cpp
## clang++ -flegacy-pass-manager -O0 -g -fno-discard-value-names -Xclang -load -Xclang ../build/skeleton/libSkeletonPass.so -c ../src/$test_file_name.cpp
#clang++ -flegacy-pass-manager -O0 -g -fPIC -fno-discard-value-names -Xclang -load -Xclang ../build/skeleton/libSkeletonPass.so -c ../src/${test_file_name}.cpp
#
#g++ ${test_file_name}.o rtlib.o
#
#echo 'input a number:'
#./a.out

add_executable(Demo demo.cpp)
target_link_libraries(Demo Rtlib)   # 为目标添加静态库
target_link_libraries(Demo SkeletonPass)

#clang++ -flegacy-pass-manager -O0 -g -fPIC -fno-discard-value-names -Xclang -load -Xclang ../build/skeleton/libSkeletonPass.so -c ../src/${test_file_name}.cpp

set_target_properties(Demo PROPERTIES
        COMPILE_FLAGS "-fno-rtti -O0 -g"
        )
