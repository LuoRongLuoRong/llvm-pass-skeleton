# llvm-pass-skeleton

A completely useless LLVM pass.
It's for LLVM 14.

rtlib 分支下：

Build:
```shell
$ cd llvm-pass-skeleton
$ mkdir build
$ cd build
$ cmake ..
$ make
$ cd ..
```

Run in `root directory`:

$ clang++ -flegacy-pass-manager -Xclang -load -Xclang build/skeleton/libSkeletonPass.* example.cpp

CPP 的命令

```shell
$ clang++ -Xclang -load -Xclang build/skeleton/libSkeletonPass.so -c example.cpp
$ g++ -c rtlib.cpp
$ g++ example.o rtlib.o
```

C 的命令

```shell
$ clang -Xclang -load -Xclang build/skeleton/libSkeletonPass.so -c example.c
$ gcc -c rtlib.c
$ gcc example.o rtlib.o
```
