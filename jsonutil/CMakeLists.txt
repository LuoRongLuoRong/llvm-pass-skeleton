add_definitions("-fPIC")

# Add a library with the above sources
add_library(
    JsonUtil 
    ./src/json_util.cpp
)
message("${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(
    JsonUtil
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
