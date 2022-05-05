cmake_minimum_required(VERSION 3.1)
# 设定工程名和版本号
project(Skeleton VERSION 1.0)

# support C++14 features used by LLVM 10.0.0
set(CMAKE_CXX_STANDARD 14)

find_package(LLVM REQUIRED CONFIG)
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

add_subdirectory(skeleton)  # Use your pass name here.

## for project TinyWebserver
# add_subdirectory(src/TinyWebServer)

# 将库文件链接到可执行程序上
# target_link_libraries(ServerLR SkeletonPass)
