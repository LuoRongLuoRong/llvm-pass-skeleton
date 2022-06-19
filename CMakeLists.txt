# 指定 cmake 的最小版本
cmake_minimum_required(VERSION 3.1)
# 设定工程名和版本号
project(Skeleton VERSION 1.0)

# support C++14 features used by LLVM 10.0.0
set(CMAKE_CXX_STANDARD 14)

find_package(LLVM REQUIRED CONFIG)
add_definitions(${LLVM_DEFINITIONS})
add_definitions("-fPIC")
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

#add_subdirectory(instrument)
add_subdirectory(readjson)
add_subdirectory(skeleton)  # Use your pass name here.
#add_subdirectory(TinyWebServer)
#add_subdirectory(src)

#target_compile_definitions()

# 将库文件链接到可执行程序上
# target_link_libraries(ServerLR SkeletonPass)
