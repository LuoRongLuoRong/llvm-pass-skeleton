#ifndef SKELETON_H
#define SKELETON_H

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/ValueSymbolTable.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/DerivedTypes.h"

// 为什么这儿用 h 会报重复定义的错误？
#include "../../jsonutil/include/json_util.h"
// #include "../../jsonutil/json_util.cpp"

#define MAX_INT (((unsigned int)(-1)) >> 1)

using namespace llvm;

class Skeleton : public PassInfoMixin<Skeleton>
{
public:
    enum DATA_TYPE
    {
        LOG_BOOL,
        LOG_INT,
        LOG_CHAR,
        LOG_FLOAT,
        LOG_DOUBLE,
        LOG_STRING,
        LOG_CHAR_ATSRERISK,
        LOG_CHAR_ARRAY
    };

    // 返回函数所在是[文件的路径+文件的名称]
    StringRef getSourceName(Function &F);
    // IR 指令在源代码中的行号
    int getLineNumber(Instruction *inst, LLVMContext &Ctx);
    // IR 指令在源代码中的行号的 Value
    Value *getLine(Instruction *inst, LLVMContext &Ctx);
    // IR 指令在源代码中的 Column
    int getColumnNumber(Instruction *inst, LLVMContext &Ctx);
    // IR 指令在源代码中的 Column 的 Value
    Value *getColumn(Instruction *inst, LLVMContext &Ctx);
    // 检查该变量是否是状态变量或者关键变量
    bool isKeyOrStateVar(Instruction *op, LLVMContext &Ctx, std::string filename, std::string varname, JsonUtil *ju);
    // 创建运行时函数：基本创建方法
    FunctionCallee getLogFunc(LLVMContext &Ctx, Function &F, Type *stateType, std::string rtFuncName);
    // 创建运行时函数：根据数据类型创建
    FunctionCallee getLogFunc(LLVMContext &Ctx, Function &F, int dataType);
    // IR 中某个函数的 local variable 表，和源代码中的局部变量是不一样的
    void getLocalVariables(Function &F);

    /**    int    **/
    void log_int_load(std::string filename, std::string varname, int type, LoadInst *inst,
                      BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx);
    void log_int_store(std::string filename, std::string varname, int type, StoreInst *inst,
                       BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx);

    /**    double    **/
    void log_double_load(std::string filename, std::string varname, int type, LoadInst *inst,
                         BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx);
    void log_double_store(std::string filename, std::string varname, int type, StoreInst *inst,
                          BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx);

    /**    float    **/
    void log_float_load(std::string filename, std::string varname, int type, LoadInst *inst,
                        BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx);
    void log_float_store(std::string filename, std::string varname, int type, StoreInst *inst,
                         BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx);

    /*    FP: float and double   */
    void log_fp_load(std::string filename, std::string varname, int type, LoadInst *inst,
                     BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx);
    void log_fp_store(std::string filename, std::string varname, int type, StoreInst *inst,
                      BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx);

    /*    char asterisk    */
    void log_char_asterisk_load(std::string filename, std::string varname, int type, LoadInst *inst,
                                BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx);
    void log_char_asterisk_store(std::string filename, std::string varname, int type, StoreInst *inst,
                                 BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx);

    /*    char array   */
    void log_char_array_gep(std::string filename, std::string varname, int type, GetElementPtrInst *inst,
                            BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx);
    void log_char_array_call(std::string filename, std::string varname, int type, CallInst *inst,
                             BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx);

    /* core function */
    bool runImpl(Function &F);
};

#endif // SKELETON_H
