cd ./src
clang++ -O0 -g -S -fno-discard-value-names -emit-llvm demo.cpp -o demo.ll

