
add_library(SkeletonPass SHARED
    Skeleton.cpp
)

add_definitions("-fPIC")

# Use C++14 to compile our pass (i.e., supply -std=c++11).
target_compile_features(SkeletonPass PRIVATE cxx_range_for cxx_auto_type)

target_link_libraries(SkeletonPass
    Rtlib
    JsonUtil
)

# LLVM is (typically) built with no C++ RTTI. We need to match that;
# otherwise, we'll get linker errors about missing RTTI data.
set_target_properties(SkeletonPass PROPERTIES
COMPILE_FLAGS "-fno-rtti"
)


# # test for injectFuncCall

# add_library(
#     InjectFuncCall
#     SHARED
#     ./src/InjectFuncCall.cpp
# )

#     # Configure include directories for 'plugin'
# target_include_directories(
#     InjectFuncCall
#     PRIVATE
#     "${CMAKE_CURRENT_SOURCE_DIR}/include"
# )

# set_target_properties(InjectFuncCall PROPERTIES
# COMPILE_FLAGS "-fno-rtti"
# )
