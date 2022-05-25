# for test
#add_executable(ReadJson read_json.cpp)
#link_libraries(ReadJson -ljsoncpp)

project (ReadJson)

# Create a sources variable with a link to all cpp files to compile
set(SOURCES
        src/jsoncpp.cpp
        )

# Add a library with the above sources
add_library(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/read_json.cpp)
#add_library(sub::libreadjson ALIAS ${PROJECT_NAME})

target_include_directories( ${PROJECT_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
        )

