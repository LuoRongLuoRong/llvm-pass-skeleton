#!/bin/sh

# set -x

SOURCE_DIR=`pwd`
BUILD_DIR=${BUILD_DIR:-./build}
BUILD_TYPE=${BUILD_TYPE:-Debug}

mkdir -p $BUILD_DIR/$BUILD_TYPE \
    && cd $BUILD_DIR/$BUILD_TYPE \
    && cmake \
            -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
            $SOURCE_DIR \
    && make $*

# mkdir -p $BUILD_DIR/$BUILD_TYPE 
# cd $BUILD_DIR/$BUILD_TYPE 
# cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE $SOURCE_DIR 
# make $*

cd $SOURCE_DIR
./build/Debug/WebServer/WebServer -t 10 -p 8881 -l /home/fdse/luorong/LLVM/test/test_project/WebServer/WebServer.log
# ./build/Debug/WebServer/WebServer 
