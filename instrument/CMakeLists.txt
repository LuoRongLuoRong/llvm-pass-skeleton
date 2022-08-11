add_library(
        Rtlib STATIC
        rtlib.cpp
)    # 静态库

# LLVM is (typically) built with no C++ RTTI. We need to match that;
# otherwise, we'll get linker errors about missing RTTI data.
set_target_properties(Rtlib PROPERTIES
        COMPILE_FLAGS "-fno-rtti"
        )
