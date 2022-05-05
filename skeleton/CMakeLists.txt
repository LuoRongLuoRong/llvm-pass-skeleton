add_library(SkeletonPass MODULE
    # List your source files here.
    Skeleton.cpp
)

# Use C++14 to compile our pass (i.e., supply -std=c++11).
target_compile_features(SkeletonPass PRIVATE cxx_range_for cxx_auto_type)

# LLVM is (typically) built with no C++ RTTI. We need to match that;
# otherwise, we'll get linker errors about missing RTTI data.
set_target_properties(SkeletonPass PROPERTIES
    COMPILE_FLAGS "-fno-rtti"
)

# # Make sure the compiler can find include files for our SkeletonPass library
# # when other libraries or executables link to Hello
# target_include_directories (SkeletonPass PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Get proper shared-library behavior (where symbols are not necessarily
# resolved when the shared library is linked) on OS X.
if(APPLE)
    set_target_properties(SkeletonPass PROPERTIES
        LINK_FLAGS "-undefined dynamic_lookup"
    )
endif(APPLE)
