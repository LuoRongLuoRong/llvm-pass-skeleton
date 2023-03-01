cd ../input/WebServer
export LLVM_COMPILER=clang
wllvm++ -fPIC -o wserver WebServer/Main.cpp WebServer/Channel.cpp WebServer/Epoll.cpp WebServer/EventLoop.cpp WebServer/EventLoopThread.cpp WebServer/EventLoopThreadPool.cpp WebServer/HttpData.cpp WebServer/Server.cpp WebServer/ThreadPool.cpp WebServer/Timer.cpp WebServer/Util.cpp WebServer/base/AsyncLogging.cpp WebServer/base/CountDownLatch.cpp WebServer/base/FileUtil.cpp WebServer/base/LogFile.cpp WebServer/base/Logging.cpp WebServer/base/LogStream.cpp WebServer/base/Thread.cpp -g -O0 -fno-discard-value-names -lpthread -lmysqlclient -D_PTHREADS -Wno-unused-parameter
# 上述命令编译时添加的参数
#  -g -O0 -fno-discard-value-names -lpthread -lmysqlclient 
#  -D_PTHREADS -Wno-unused-parameter
extract-bc wserver
llvm-dis wserver.bc 
