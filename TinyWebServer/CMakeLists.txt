# aux_source_directory(. SRC_LIST)  # 把当前目录下的源文件存在 SRC_LIST 变量中

# 设置编译参数


# add the executable 
#add_executable(ServerLR main.cpp ./timer/lst_timer.cpp ./http/http_conn.cpp ./log/log.cpp ./CGImysql/sql_connection_pool.cpp webserver.cpp config.cpp)

# Link the executable to the SkeletonPass library. Since the SkeletonPass library has
# public include directories we will use those link directories when building
# target_link_libraries (ServerLR SkeletonPass)


cmake_minimum_required(VERSION 3.10)
# 设定工程名和版本号
project(TinyWebServer VERSION 1.0)

#add_subdirectory(timer)
#add_subdirectory(http)
#add_subdirectory(log)
#add_subdirectory(CGImysql)
#add_subdirectory(lock)
#add_subdirectory(threadpool)

link_libraries(-lpthread -lmysqlclient)

add_executable(Server main.cpp ./timer/lst_timer.cpp ./http/http_conn.cpp ./log/log.cpp ./CGImysql/sql_connection_pool.cpp webserver.cpp config.cpp rtlib.cpp)

add_library(Rtlib STATIC rtlib.cpp)    # 静态库

# 指定编译给定目标时要使用的 include 目录。
# 这里即在编译生成 server 可执行文件时，
# 使用 CMakeLists.txt 所在的目录下的 include 目录。
target_include_directories(Server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# 添加链接库
target_link_libraries(Server SkeletonPass)
target_link_libraries(Server Rtlib)   # 为目标添加静态库
