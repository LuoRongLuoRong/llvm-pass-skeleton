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

```shell
$ clang++ -Xclang -load -Xclang build/mypass/libMypassPass.so -c example.cpp
$ g++ -c rtlib.cpp
$ g++ example.o rtlib.o
```
