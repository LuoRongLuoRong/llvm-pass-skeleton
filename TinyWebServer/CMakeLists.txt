# aux_source_directory(. SRC_LIST)  # 把当前目录下的源文件存在 SRC_LIST 变量中

# 设置编译参数
#LINK_LIBRARIES(-lpthread -lmysqlclient)

# add the executable 
#add_executable(ServerLR main.cpp ./timer/lst_timer.cpp ./http/http_conn.cpp ./log/log.cpp ./CGImysql/sql_connection_pool.cpp webserver.cpp config.cpp)

# Link the executable to the SkeletonPass library. Since the SkeletonPass library has
# public include directories we will use those link directories when building
# target_link_libraries (ServerLR SkeletonPass)


cmake_minimum_required(VERSION 3.10)

# 设定工程名和版本号
project(CalculateSqrt VERSION 1.0)
add_executable(TinyWebServer main.cpp)

