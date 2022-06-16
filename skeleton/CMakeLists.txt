### 1. Skeleton.cpp
add_library(SkeletonPass SHARED
    # List your source files here.
    Skeleton.cpp
)

# Use C++14 to compile our pass (i.e., supply -std=c++11).
target_compile_features(SkeletonPass PRIVATE cxx_range_for cxx_auto_type)

target_link_libraries(SkeletonPass
        ReadJson
        )

# LLVM is (typically) built with no C++ RTTI. We need to match that;
# otherwise, we'll get linker errors about missing RTTI data.
set_target_properties(SkeletonPass PROPERTIES
    COMPILE_FLAGS "-fno-rtti"
)

############## 2. ValueTracePass.cpp  ###############################
# add_library(ValueTracePass SHARED
#     # List your source files here.
#     ValueTrace.cpp
# )

# # Use C++14 to compile our pass (i.e., supply -std=c++11).
# target_compile_features(ValueTracePass PRIVATE cxx_range_for cxx_auto_type)

# target_link_libraries(ValueTracePass
#         ReadJson
#         )

# # LLVM is (typically) built with no C++ RTTI. We need to match that;
# # otherwise, we'll get linker errors about missing RTTI data.
# set_target_properties(ValueTracePass PROPERTIES
#     COMPILE_FLAGS "-fno-rtti"
# )
