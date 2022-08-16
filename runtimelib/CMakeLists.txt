add_library(
    Rtlib 
    src/rtlib.cpp
)

target_include_directories(
    Rtlib
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# LLVM is (typically) built with no C++ RTTI. We need to match that;
# otherwise, we'll get linker errors about missing RTTI data.
set_target_properties(Rtlib PROPERTIES
    COMPILE_FLAGS "-fno-rtti"
    )
