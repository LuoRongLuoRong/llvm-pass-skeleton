
add_library(SkeletonPass SHARED
    # List your source files here.
    Skeleton.cpp
)

add_definitions("-fPIC")

# Use C++14 to compile our pass (i.e., supply -std=c++11).
target_compile_features(SkeletonPass PRIVATE cxx_range_for cxx_auto_type)

target_link_libraries(SkeletonPass
        JsonUtil
        )

# LLVM is (typically) built with no C++ RTTI. We need to match that;
# otherwise, we'll get linker errors about missing RTTI data.
set_target_properties(SkeletonPass PROPERTIES
    COMPILE_FLAGS "-fno-rtti"
)

# # Make sure the compiler can find include files for our SkeletonPass library
# # when other libraries or executables link to Hello
