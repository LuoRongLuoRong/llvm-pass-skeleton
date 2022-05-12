#include <iostream>

int foo_int() {
    int state_int = 1;
    // ...
    state_int = 2;
    // ...
    std::cin >> state_int;
    state_int = state_int + 1;
    return state_int;
}

/*
define dso_local i32 @_Z7foo_intv() #4 !dbg !857 {
entry:
  %state_int = alloca i32, align 4
  call void @llvm.dbg.declare(metadata i32* %state_int, metadata !858, metadata !DIExpression()), !dbg !859
  store i32 1, i32* %state_int, align 4, !dbg !859
  store i32 2, i32* %state_int, align 4, !dbg !860
  %call = call nonnull align 8 dereferenceable(16) %"class.std::basic_istream"* @_ZNSirsERi(%"class.std::basic_istream"* nonnull align 8 dereferenceable(16) @_ZSt3cin, i32* nonnull align 4 dereferenceable(4) %state_int), !dbg !861
  %0 = load i32, i32* %state_int, align 4, !dbg !862
  %add = add nsw i32 %0, 1, !dbg !863
  store i32 %add, i32* %state_int, align 4, !dbg !864
  %1 = load i32, i32* %state_int, align 4, !dbg !865
  ret i32 %1, !dbg !866
}
 */

// bool foo_bool() {
//     bool state_bool = true;
//     // ...
//     state_bool = false;
//     // ...
//     std::cin >> state_bool;
//     return state_bool;
// }

// char foo_char() {
//     char state_char = 'l';
//     // ...
//     state_char = 'r';
//     // ...
//     std::cin >> state_char;
//     return state_char;
// }

// std::string foo_string() {
//     std::string state_string = "ILoveLLVM";
//     // ...
//     state_string = "IDon'tLoveLLVM";
//     // ...
//     std::cin >> state_string;
//     return state_string;
// }

// float foo_float() {
//     float state_float = 1.23;
//     // ...
//     state_float = -1.23;
//     // ...
//     std::cin >> state_float;
//     return state_float;
// }

// double foo_double() {
//     double state_double = 1.23;
//     // ...
//     state_double = -1.23;
//     // ...
//     std::cin >> state_double;
//     return state_double;
// }

int main() {
  int state_int = foo_int();
  state_int = foo_int();

    // bool state_bool = foo_bool();
    // char state_char = foo_char();
    // std::string state_string = foo_string();
}
