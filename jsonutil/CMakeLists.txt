project (JsonUtil)

add_definitions("-fPIC")

# Create a sources variable with a link to all cpp files to compile
set(SOURCES
        src/jsoncpp.cpp
        )

# Add a library with the above sources
add_library(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/json_util.cpp)

target_include_directories( ${PROJECT_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
        )

