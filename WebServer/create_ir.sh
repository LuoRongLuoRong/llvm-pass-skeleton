export LLVM_COMPILER=clang
wllvm++ -fPIC -o webserver ./WebServer/Main.cpp ./WebServer/Channel.cpp ./WebServer/Epoll.cpp ./WebServer/EventLoop.cpp ./WebServer/EventLoopThread.cpp ./WebServer/EventLoopThreadPool.cpp ./WebServer/HttpData.cpp ./WebServer/Server.cpp ./WebServer/ThreadPool.cpp ./WebServer/Timer.cpp ./WebServer/Util.cpp ./WebServer/base/AsyncLogging.cpp ./WebServer/base/CountDownLatch.cpp ./WebServer/base/FileUtil.cpp ./WebServer/base/LogFile.cpp ./WebServer/base/Logging.cpp ./WebServer/base/LogStream.cpp ./WebServer/base/Thread.cpp -g -O0 -fno-discard-value-names -lpthread -lmysqlclient -D_PTHREADS -Wno-unused-parameter
extract-bc webserver
llvm-dis webserver.bc 
