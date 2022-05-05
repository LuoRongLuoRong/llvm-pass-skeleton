#include <iostream>

int foo() {
    int m_check_state = 0;
    // ...
    m_check_state = 1;
    // ...
    std::cin >> m_check_state;
    return m_check_state;
}

int main() {
    int m_check_state = foo();
}


// define dso_local i32 @_Z3foov() #4 !dbg !857 {
// entry:
//   %m_check_state = alloca i32, align 4
//   call void @llvm.dbg.declare(metadata i32* %m_check_state, metadata !858, metadata !DIExpression()), !dbg !859
//   store i32 0, i32* %m_check_state, align 4, !dbg !859
//   store i32 1, i32* %m_check_state, align 4, !dbg !860
//   %call = call nonnull align 8 dereferenceable(16) %"class.std::basic_istream"* @_ZNSirsERi(%"class.std::basic_istream"* nonnull align 8 dereferenceable(16) @_ZSt3cin, i32* nonnull align 4 dereferenceable(4) %m_check_state), !dbg !861
//   %0 = load i32, i32* %m_check_state, align 4, !dbg !862
//   ret i32 %0, !dbg !863
// }

// define dso_local i32 @main() #6 !dbg !864 {
// entry:
//   %m_check_state = alloca i32, align 4
//   call void @llvm.dbg.declare(metadata i32* %m_check_state, metadata !865, metadata !DIExpression()), !dbg !866
//   %call = call i32 @_Z3foov(), !dbg !867
//   store i32 %call, i32* %m_check_state, align 4, !dbg !866
//   ret i32 0, !dbg !868
// }