# 指定 cmake 的最小版本
cmake_minimum_required(VERSION 3.5)
# 设定工程名和版本号
project(Skeleton VERSION 1.0)

# support C++14 features used by LLVM 10.0.0
set(CMAKE_CXX_STANDARD 14)

#===============================================================================
# LOAD LLVM CONFIGURATION
#    For more: http://llvm.org/docs/CMake.html#embedding-llvm-in-your-project
#===============================================================================
# Add the location of LLVMConfig.cmake to CMake search paths (so that
# find_package can locate it)

find_package(LLVM REQUIRED CONFIG)

# Set the LLVM header and library paths
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

# JsonUtil inlcude
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/jsonutil/include)

message("LLVM STATUS:
  Definitions ${LLVM_DEFINITIONS}
  Includes    ${LLVM_INCLUDE_DIRS}
  Libraries   ${LLVM_LIBRARY_DIRS}
  Targets     ${LLVM_TARGETS_TO_BUILD}"
)

# 代码位置无关
add_definitions("-fPIC")

#===============================================================================
# ADD SUB-TARGETS
# Doing this at the end so that all definitions and link/include paths are
# available for the sub-projects.
#===============================================================================
add_subdirectory(runtimelib)
add_subdirectory(jsonutil)
add_subdirectory(skeleton)  # Use your pass name here.
