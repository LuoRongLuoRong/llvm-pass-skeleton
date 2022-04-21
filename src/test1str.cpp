#include <iostream>
#include <string>

int main() {
    std::string str = "luorong is smart.";
    return 0;
}

// define dso_local i32 @main() #4 personality i8* bitcast (i32 (...)* @__gxx_personality_v0 to i8*) !dbg !857 {
//   %1 = alloca i32, align 4
//   %2 = alloca %"class.std::__cxx11::basic_string", align 8
//   %3 = alloca %"class.std::allocator", align 1
//   %4 = alloca i8*, align 8
//   %5 = alloca i32, align 4
//   store i32 0, i32* %1, align 4
//   call void @llvm.dbg.declare(metadata %"class.std::__cxx11::basic_string"* %2, metadata !858, metadata !DIExpression()), !dbg !864
//   call void @_ZNSaIcEC1Ev(%"class.std::allocator"* nonnull align 1 dereferenceable(1) %3) #3, !dbg !865
//   invoke void @_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EPKcRKS3_(%"class.std::__cxx11::basic_string"* nonnull align 8 dereferenceable(32) %2, i8* getelementptr inbounds ([18 x i8], [18 x i8]* @.str, i64 0, i64 0), %"class.std::allocator"* nonnull align 1 dereferenceable(1) %3)
//           to label %6 unwind label %8, !dbg !865

// 6:                                                ; preds = %0
//   call void @_ZNSaIcED1Ev(%"class.std::allocator"* nonnull align 1 dereferenceable(1) %3) #3, !dbg !864
//   store i32 0, i32* %1, align 4, !dbg !866
//   call void @_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED1Ev(%"class.std::__cxx11::basic_string"* nonnull align 8 dereferenceable(32) %2) #3, !dbg !867
//   %7 = load i32, i32* %1, align 4, !dbg !867
//   ret i32 %7, !dbg !867

// 8:                                                ; preds = %0
//   %9 = landingpad { i8*, i32 }
//           cleanup, !dbg !867
//   %10 = extractvalue { i8*, i32 } %9, 0, !dbg !867
//   store i8* %10, i8** %4, align 8, !dbg !867
//   %11 = extractvalue { i8*, i32 } %9, 1, !dbg !867
//   store i32 %11, i32* %5, align 4, !dbg !867
//   call void @_ZNSaIcED1Ev(%"class.std::allocator"* nonnull align 1 dereferenceable(1) %3) #3, !dbg !864
//   br label %12, !dbg !864

// 12:                                               ; preds = %8
//   %13 = load i8*, i8** %4, align 8, !dbg !864
//   %14 = load i32, i32* %5, align 4, !dbg !864
//   %15 = insertvalue { i8*, i32 } undef, i8* %13, 0, !dbg !864
//   %16 = insertvalue { i8*, i32 } %15, i32 %14, 1, !dbg !864
//   resume { i8*, i32 } %16, !dbg !864
// }
