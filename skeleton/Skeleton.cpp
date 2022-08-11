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
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/PassBuilder.h"

// h 会有重复定义的问题
// #include "json_util.h"
#include "../jsonutil/src/json_util.cpp"

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

    /* core function */
    llvm::PreservedAnalyses run(llvm::Module &M,
                                llvm::ModuleAnalysisManager &);
    bool runOnModule(llvm::Module &M);

    // 返回函数所在的[文件的路径+文件的名称]
    StringRef getSourceName(Function &F);
    // 返回文件所在的[文件的路径+文件的名称]
    StringRef getSourceFilePath(Module &M);
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
};

PreservedAnalyses Skeleton::run(llvm::Module &M,
                                llvm::ModuleAnalysisManager &)
{
    bool Changed = runOnModule(M);

    return (Changed ? llvm::PreservedAnalyses::none()
                    : llvm::PreservedAnalyses::all());
}

StringRef Skeleton::getSourceName(Function &F)
{
    errs() << "getSourceName: ";

    errs() << F.hasName();
    DISubprogram *DI = F.getSubprogram();
    if (!DI)
    {
        errs() << "Function " << F.getName() << " does not have a subprogram\n";
        return F.getName();
    }
    DIFile *DIF = DI->getFile();
    if (!DIF)
    {
        errs() << "Function " << F.getName() << " does not have a file\n";
        return F.getName();
    }
    return DIF->getFilename();
}

StringRef Skeleton::getSourceFilePath(Module &M)
{
    // !3 = !DIFile(filename: "/usr/lib/gcc/x86_64-linux-gnu/7.5.0/../../../../include/c++/7.5.0/iostream", directory: "")
    // !14 = !DIFile(filename: "http/http_conn.cpp", directory: "/home/fdse/luorong/LLVM/test/llvm-pass-skeleton/TinyWebServer")

    return "";
}

// IR 指令在源代码中的行号
int Skeleton::getLineNumber(Instruction *inst, LLVMContext &Ctx)
{
    if (DILocation *DILoc = inst->getDebugLoc())
    {
        return DILoc->getLine();
    }
    return 0;
}

// IR 指令在源代码中的行号的 Value
Value *Skeleton::getLine(Instruction *inst, LLVMContext &Ctx)
{
    return ConstantInt::get(Type::getInt32Ty(Ctx), getLineNumber(inst, Ctx));
}

// IR 指令在源代码中的 Column
int Skeleton::getColumnNumber(Instruction *inst, LLVMContext &Ctx)
{
    if (DILocation *DILoc = inst->getDebugLoc())
    {
        return DILoc->getColumn(); // here wrong
    }
    return 0;
}

// IR 指令在源代码中的 Column 的 Value
Value *Skeleton::getColumn(Instruction *inst, LLVMContext &Ctx)
{
    return ConstantInt::get(Type::getInt32Ty(Ctx), getColumnNumber(inst, Ctx));
}

// 检查该变量是否是状态变量或者关键变量
bool Skeleton::isKeyOrStateVar(Instruction *op, LLVMContext &Ctx, std::string filename, std::string varname, JsonUtil *ju)
{
    int line = getLineNumber(op, Ctx);
    int column = getColumnNumber(op, Ctx);
    errs() << "检查变量是否存在：" << filename << " " << varname << " " << line << " " << column << " " << *op << " ";
    if (line == 0 || column == 0 || !ju->hasVar(filename, line, column, varname))
    {
        errs() << " 不存在 \n";
        return false;
    }
    errs() << "\n存在 \n";
    return true;
}

void addLogFunctionCallee(Module &M, LLVMContext &Ctx, Function &F, Type *stateType, std::string rtFuncName)
{

    auto &CTX = M.getContext();
    PointerType *PrintfArgTy = PointerType::getUnqual(Type::getInt8Ty(CTX));

    // STEP 1: Inject the declaration of printf
    // ----------------------------------------
    // Create (or _get_ in cases where it's already available) the following
    // declaration in the IR module:
    //    declare i32 @printf(i8*, ...)
    // It corresponds to the following C declaration:
    //    int printf(char *, ...)
    FunctionType *PrintfTy = FunctionType::get(
        IntegerType::getInt32Ty(CTX),
        PrintfArgTy,
        /*IsVarArgs=*/true);

    FunctionCallee Printf = M.getOrInsertFunction("printf", PrintfTy);

    // Set attributes as per inferLibFuncAttributes in BuildLibCalls.cpp
    Function *PrintfF = dyn_cast<Function>(Printf.getCallee());
    PrintfF->setDoesNotThrow();
    PrintfF->addParamAttr(0, Attribute::NoCapture);
    PrintfF->addParamAttr(0, Attribute::ReadOnly);

    // STEP 2: Inject a global variable that will hold the printf format string
    // ------------------------------------------------------------------------
    llvm::Constant *PrintfFormatStr = llvm::ConstantDataArray::getString(
        CTX, "(llvm-tutor) Hello from: %s\n(llvm-tutor)   number of arguments: %d\n");

    Constant *PrintfFormatStrVar =
        M.getOrInsertGlobal("PrintfFormatStr", PrintfFormatStr->getType());
    dyn_cast<GlobalVariable>(PrintfFormatStrVar)->setInitializer(PrintfFormatStr);

    // STEP 3: For each function in the module, inject a call to printf
    // ----------------------------------------------------------------
    // Get an IR builder. Sets the insertion point to the top of the function
    IRBuilder<> Builder(&*F.getEntryBlock().getFirstInsertionPt());

    // Inject a global variable that contains the function name
    auto FuncName = Builder.CreateGlobalStringPtr(F.getName());

    // Printf requires i8*, but PrintfFormatStrVar is an array: [n x i8]. Add
    // a cast: [n x i8] -> i8*
    llvm::Value *FormatStrPtr =
        Builder.CreatePointerCast(PrintfFormatStrVar, PrintfArgTy, "formatStr");

    // Finally, inject a call to printf
    Builder.CreateCall(
        Printf, {FormatStrPtr, FuncName, Builder.getInt32(F.arg_size())});
}

FunctionCallee Skeleton::getLogFunc(LLVMContext &Ctx, Function &F, Type *stateType, std::string rtFuncName)
{
    // 函数参数：
    std::vector<Type *> paramTypes = {
        PointerType::getUnqual(Type::getInt8Ty(Ctx)), // filename
        // Type::getInt8PtrTy(Ctx),
        Type::getInt32Ty(Ctx),                        // line
        PointerType::getUnqual(Type::getInt8Ty(Ctx)), // name
        Type::getInt32Ty(Ctx),                        // type
        stateType,                                    // state
        stateType                                     // old_state
    };
    // 函数返回值：
    Type *retType = Type::getVoidTy(Ctx);
    // 函数类型：
    FunctionType *logFuncType = FunctionType::get(retType, paramTypes, /*IsVarArgs=*/false);
    // 根据函数的名字获取该函数：
    FunctionCallee logFunc = F.getParent()->getOrInsertFunction(rtFuncName, logFuncType);
    return logFunc;
}

FunctionCallee Skeleton::getLogFunc(LLVMContext &Ctx, Function &F, int dataType)
{
    switch (dataType)
    {
    case LOG_BOOL: // bool
        return getLogFunc(Ctx, F, Type::getInt8Ty(Ctx), "logbool");
    case LOG_CHAR: // char
        return getLogFunc(Ctx, F, Type::getInt8Ty(Ctx), "logchar");
    case LOG_STRING: // string
        return getLogFunc(Ctx, F, Type::getInt8PtrTy(Ctx), "logstring");
    case LOG_FLOAT: // float
        return getLogFunc(Ctx, F, Type::getFloatTy(Ctx), "logfloat");
    case LOG_DOUBLE: // double
        return getLogFunc(Ctx, F, Type::getDoubleTy(Ctx), "logdouble");
    case LOG_CHAR_ATSRERISK: // char*
        return getLogFunc(Ctx, F, Type::getInt8PtrTy(Ctx), "logcharasterisk");
    case LOG_CHAR_ARRAY: // char[]
        return getLogFunc(Ctx, F, Type::getInt8PtrTy(Ctx), "logchararray");
    default: // LOG_INT: int
        return getLogFunc(Ctx, F, Type::getInt32Ty(Ctx), "logint");
    }
}

void Skeleton::getLocalVariables(Function &F)
{
    ValueSymbolTable *vst = F.getValueSymbolTable();
    // errs() << "LocalVariables:" << vst << '\n';
    for (auto vb = vst->begin(); vb != vst->end(); vb++)
    {
        std::list<Instruction *> useList;
        Value *val = vb->second;
        Instruction *inst = dyn_cast<Instruction>(val);
        Type *type = val->getType();
    }
}

/****************** instrumented functions *******************/
/**    int    **/
void Skeleton::log_int_load(std::string filename, std::string varname, int type, LoadInst *inst,
                            BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx)
{
    IRBuilder<> builder(inst);
    builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

    // Value *argfilename = builder.CreateGlobalString(filename);
    Value *argfilename = builder.CreatePointerCast(
        builder.CreateGlobalString(filename),
        PointerType::getUnqual(Type::getInt8Ty(Ctx)), "argfilename");
    // Value *argstr = builder.CreateGlobalString(varname);
    Value *argstr = builder.CreatePointerCast(
        builder.CreateGlobalString(varname),
        PointerType::getUnqual(Type::getInt8Ty(Ctx)), "argvarname");
    Value *argtype = ConstantInt::get(Type::getInt32Ty(Ctx), type);
    Value *argvalue = dyn_cast_or_null<Value>(inst); // state
    Value *argline = getLine(inst, Ctx);             //
    Value *argold = dyn_cast_or_null<Value>(inst);   // old state

    Value *args[] = {argfilename, argline, argstr, argtype, argvalue, argvalue};
    // Value *args[] = {ConstantInt::get(Type::getInt32Ty(Ctx), 1)};
    // instrumentation
    builder.CreateCall(logFunc, args);
}

// TODO: for injectFuncCall
void insertInt(Module &M)
{
    auto &CTX = M.getContext();
    PointerType *PrintfArgTy = PointerType::getUnqual(Type::getInt8Ty(CTX));
    FunctionType *PrintfTy = FunctionType::get(
        IntegerType::getInt32Ty(CTX),
        PrintfArgTy,
        /*IsVarArgs=*/true);
}

void Skeleton::log_int_store(std::string filename, std::string varname, int type, StoreInst *inst, BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx)
{
    errs() << "看这个看这个！";
    IRBuilder<> builder(inst);
    builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

    // get right: name
    Value *arg2 = inst->getOperand(1);
    // errs() << "StoreInst R: " << *arg2 << ": [" << arg2->getName() << "]\n";
    // Value *argfilename = builder.CreateGlobalString(filename);
    Value *argfilename = builder.CreatePointerCast(
        builder.CreateGlobalString(filename),
        PointerType::getUnqual(Type::getInt8Ty(Ctx)), /*name=*/"argfilename");
    // Value *argstr = builder.CreateGlobalString(varname);
    Value *argstr = builder.CreatePointerCast(
        builder.CreateGlobalString(varname),
        PointerType::getUnqual(Type::getInt8Ty(Ctx)), "argvarname");
    Value *argtype = ConstantInt::get(Type::getInt32Ty(Ctx), type);
    Value *argi = inst->getOperand(0);   // state
    Value *argline = getLine(inst, Ctx); //

    Value *argold;
    // errs() << "   arg2 的 Use 的数目" << arg2->getNumUses() << "\n";

    Value::use_iterator U = arg2->use_begin();

    bool isInitial = true;
    do
    {
        // 1. 全局变量，直接 load
        if (!isa<AllocaInst>(arg2))
        {
            isInitial = false;
            break;
        }
        // 2. 局部变量，检测是否曾被赋值过。如果前面全是 load 则认为是初始化。
        // 循环终止的条件：（1）遇到本 instruction【说明是初始化】；（2）遇到结尾。

        // 只能倒序遍历，所以不得不出此下策
        while (U->getUser() != inst)
        {
            ++U;
        }
        ++U; // 跳过 inst

        while (U != arg2->use_end())
        {
            // 之前已经被使用过了
            if (isa<StoreInst>(U->getUser()))
            {
                isInitial = false;
                // errs() << "之前已经被" << inst << "使用过了" << U->getUser() << " >>> Use：" << *(U->getUser()) << "\n";

                break;
            }
            ++U;
            // errs() << "  U->getUser() == inst: " << (U->getUser() == inst) << "\n";
        }
    } while (false);

    if (isInitial)
    {
        // 未被初始化
        argold = ConstantInt::get(Type::getInt32Ty(Ctx), MAX_INT);
    }
    else
    {
        LoadInst *loadInst = new LoadInst(arg2->getType(), arg2, arg2->getName(), inst);
        argold = dyn_cast_or_null<Value>(loadInst);
    }

    Value *args[] = {argfilename, argline, argfilename, argtype, argi, argi}; //
    builder.CreateCall(logFunc, args);
}

/**    double    **/
void Skeleton::log_double_load(std::string filename, std::string varname, int type, LoadInst *inst,
                               BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx)
{
    IRBuilder<> builder(inst);
    builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

    Value *argfilename = builder.CreateGlobalString(filename);
    Value *argstr = builder.CreateGlobalString(varname);
    Value *argtype = ConstantInt::get(Type::getInt32Ty(Ctx), type);

    // Value *argvalue = ConstantFP::get(Type::getDoubleTy(Ctx), 2.0);
    Value *argvalue = dyn_cast_or_null<Value>(inst); // state
    // Value *argvalue = builder.CreateUIToFP(ConstantInt::get(Type::getInt32Ty(Ctx), 4), Type::getFloatTy(Ctx));
    // Value *argvalue = inst->getOperand(0); // state

    Value *argline = getLine(inst, Ctx); //
    // Value *argold = dyn_cast_or_null<Value>(inst);   // old state
    // Value *argold = inst->getOperand(0); // old state
    Value *argold = builder.CreateUIToFP(ConstantInt::get(Type::getInt32Ty(Ctx), 4), Type::getFloatTy(Ctx));

    Value *args[] = {argfilename, argline, argstr, argtype, argvalue, argold};
    // instrumentation
    builder.CreateCall(logFunc, args);
}

void Skeleton::log_double_store(std::string filename, std::string varname, int type, StoreInst *inst, BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx)
{
    IRBuilder<> builder(inst);
    builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

    Value *argfilename = builder.CreateGlobalString(filename);
    Value *argstr = builder.CreateGlobalString(varname);
    Value *argtype = ConstantInt::get(Type::getInt32Ty(Ctx), type);
    Value *argi = inst->getOperand(0);
    Value *argline = getLine(inst, Ctx); //

    Value *args[] = {argfilename, argline, argstr, argtype, argi, argi}; //
    builder.CreateCall(logFunc, args);
}

/**    float    **/
void Skeleton::log_float_load(std::string filename, std::string varname, int type, LoadInst *inst,
                              BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx)
{
    IRBuilder<> builder(inst);
    builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

    Value *argfilename = builder.CreateGlobalString(filename);
    Value *argstr = builder.CreateGlobalString(varname);
    Value *argtype = ConstantInt::get(Type::getInt32Ty(Ctx), type);
    Value *argvalue = dyn_cast_or_null<Value>(inst); // state
    Value *argline = getLine(inst, Ctx);             //
    Value *argold = dyn_cast_or_null<Value>(inst);   // old state

    Value *args[] = {argfilename, argline, argstr, argtype, argvalue, argold};
    // instrumentation
    builder.CreateCall(logFunc, args);
}

void Skeleton::log_float_store(std::string filename, std::string varname, int type, StoreInst *inst, BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx)
{
    IRBuilder<> builder(inst);
    builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

    Value *argfilename = builder.CreateGlobalString(filename);
    Value *argstr = builder.CreateGlobalString(varname);
    Value *argtype = ConstantInt::get(Type::getInt32Ty(Ctx), type);
    Value *argi = inst->getOperand(0);
    Value *argline = getLine(inst, Ctx); //

    Value *args[] = {argfilename, argline, argstr, argtype, argi, argi}; //
    builder.CreateCall(logFunc, args);
}

/*    FP: float and double   */
void Skeleton::log_fp_load(std::string filename, std::string varname, int type, LoadInst *inst,
                           BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx)
{
    IRBuilder<> builder(inst);
    builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

    Value *argfilename = builder.CreateGlobalString(filename);
    Value *argstr = builder.CreateGlobalString(varname);
    Value *argtype = ConstantInt::get(Type::getInt32Ty(Ctx), type);
    Value *argvalue = dyn_cast_or_null<Value>(inst); // state
    Value *argline = getLine(inst, Ctx);             //
    Value *argold = dyn_cast_or_null<Value>(inst);   // old state

    Value *args[] = {argfilename, argline, argstr, argtype, argvalue, argold};
    // instrumentation
    builder.CreateCall(logFunc, args);
}

void Skeleton::log_fp_store(std::string filename, std::string varname, int type, StoreInst *inst, BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx)
{
    IRBuilder<> builder(inst);
    builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

    Value *argfilename = builder.CreateGlobalString(filename);
    Value *argstr = builder.CreateGlobalString(varname);
    Value *argtype = ConstantInt::get(Type::getInt32Ty(Ctx), type);
    Value *argi = inst->getOperand(0);
    Value *argline = getLine(inst, Ctx); //

    Value *args[] = {argfilename, argline, argstr, argtype, argi, argi}; //
    builder.CreateCall(logFunc, args);
}

/*    char asterisk    */

// TODO
bool hasBeenInitialized(Value *v, Value *inst, LLVMContext &Ctx)
{

    Value::use_iterator U = v->use_begin();

    // 1. 全局变量，直接 load
    if (!isa<AllocaInst>(v))
    {
        // todo: not sure
        return true;
    }
    // 2. 局部变量，检测是否曾被赋值过。如果前面全是 load 则认为是初始化。
    // 循环终止的条件：（1）遇到本 instruction【说明是初始化】；（2）遇到结尾。

    // 只能倒序遍历，所以不得不先把后面的 instruction 跳过
    while (U->getUser() != inst)
    {
        ++U;
    }
    ++U; // 跳过 v 所在的 inst

    while (U != v->use_end())
    {
        errs() << "v 的 user" << U->getUser() << "\n";
        // 之前已经被使用过了
        if (isa<StoreInst>(U->getUser()))
        {
            // errs() << "之前已经被" << inst << "使用过了" << U->getUser() << " >>> Use：" << *(U->getUser()) << "\n";
            return true;
        }
        ++U;
        // errs() << "  U->getUser() == inst: " << (U->getUser() == inst) << "\n";
    }
}

void Skeleton::log_char_asterisk_load(std::string filename, std::string varname, int type, LoadInst *inst,
                                      BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx)
{
    IRBuilder<> builder(inst);
    builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

    Value *argfilename = builder.CreateGlobalString(filename);
    Value *argstr = builder.CreateGlobalString(varname);
    Value *argtype = ConstantInt::get(Type::getInt32Ty(Ctx), type);
    Value *argvalue = dyn_cast_or_null<Value>(inst); // state
    Value *argline = getLine(inst, Ctx);             //
    Value *argold = dyn_cast_or_null<Value>(inst);   // old state

    Value *args[] = {argfilename, argline, argstr, argtype, argvalue, argold};
    // instrumentation
    builder.CreateCall(logFunc, args);
}

void Skeleton::log_char_asterisk_store(std::string filename, std::string varname, int type, StoreInst *inst, BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx)
{
    return;
    IRBuilder<> builder(inst);
    builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

    Value *argfilename = builder.CreateGlobalString(filename);
    Value *argstr = builder.CreateGlobalString(varname);
    Value *argtype = ConstantInt::get(Type::getInt32Ty(Ctx), type);
    Value *argi = inst->getOperand(0);
    Value *argline = getLine(inst, Ctx); //

    Value *args[] = {argfilename, argline, argstr, argtype, argi, argi}; //
    builder.CreateCall(logFunc, args);
}

/*    char array   */
void Skeleton::log_char_array_gep(std::string filename, std::string varname, int type, GetElementPtrInst *inst,
                                  BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx)
{
    IRBuilder<> builder(inst);
    builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

    Value *argfilename = builder.CreateGlobalString(filename);
    Value *argstr = builder.CreateGlobalString(varname);
    Value *argtype = ConstantInt::get(Type::getInt32Ty(Ctx), type);
    Value *argvalue = dyn_cast_or_null<Value>(inst); // state
    Value *argline = getLine(inst, Ctx);             //
    Value *argold = dyn_cast_or_null<Value>(inst);   // old state

    Value *args[] = {argfilename, argline, argstr, argtype, argvalue, argold};
    // instrumentation
    builder.CreateCall(logFunc, args);
}

void Skeleton::log_char_array_call(std::string filename, std::string varname, int type, CallInst *inst, BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx)
{
    IRBuilder<> builder(inst);
    builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

    Value *argfilename = builder.CreateGlobalString(filename);
    Value *argstr = builder.CreateGlobalString(varname);
    Value *argtype = ConstantInt::get(Type::getInt32Ty(Ctx), type);
    Value *argi = inst->getOperand(0);
    Value *argline = getLine(inst, Ctx); //

    Value *args[] = {argfilename, argline, argstr, argtype, argi, argi}; //
    builder.CreateCall(logFunc, args);
}

/* core function */

// 将 LLVM Pass 的 runOnFunction 解构到这个函数中，
// 目的是减少成员变量的无效赋值，以提高项目效率。
bool Skeleton::runOnModule(Module &M)
{
    JsonUtil *ju = JsonUtil::getInstance();
    ju->save();
    // 判断 filename 是否包含在 json 文件中
    std::string jsonPath = "../SVsite.json";

    std::string filename = M.getSourceFileName();

    // 该文件不值得继续探索
    if (!ju->inFilepaths(filename))
    {
        errs() << "该文件不值得继续探索: " << filename << "\n";
        return false;
    }
    errs() << "该文件值得继续探索: " << filename << "\n";

    for (auto &F : M)
    {
        if (F.isDeclaration() || F.isIntrinsic())
        {
            continue;
        }

        auto argumnents = F.arg_size();
        errs() << F.getName() << argumnents << "\n";

        // Get the function to call from our runtime library.
        LLVMContext &Ctx = F.getContext();

        FunctionCallee logFuncInt = getLogFunc(Ctx, F, LOG_INT);
        FunctionCallee logFuncBool = getLogFunc(Ctx, F, LOG_BOOL);
        FunctionCallee logFuncChar = getLogFunc(Ctx, F, LOG_CHAR);
        FunctionCallee logFuncFloat = getLogFunc(Ctx, F, LOG_FLOAT);
        FunctionCallee logFuncDouble = getLogFunc(Ctx, F, LOG_DOUBLE);
        FunctionCallee logFuncString = getLogFunc(Ctx, F, LOG_STRING);
        FunctionCallee logFuncCharArray = getLogFunc(Ctx, F, LOG_CHAR_ARRAY);
        FunctionCallee logFuncCharAsterisk = getLogFunc(Ctx, F, LOG_CHAR_ATSRERISK);

        for (auto &B : F)
        {
            for (auto &I : B)
            {
                if (auto *op = dyn_cast<GetElementPtrInst>(&I))
                {
                    Value *arg1 = op->getOperand(0); // %4 = xxx
                    std::string varName = arg1->getName().str();
                    // 检查该变量是否存在
                    if (!isKeyOrStateVar(op, Ctx, filename, varName, ju))
                    {
                        continue;
                    }
                    std::string sv = ju->getVarname(filename, varName);
                    int type = ju->getType(filename, sv);

                    log_char_array_gep(filename, sv, type, op, B, logFuncCharArray, Ctx);
                }

                // 针对数组，数组型变量会被 bitcast 转换成基本数据类型，如 char[] 转换成 i8
                // LLVM 会给这个变量一个临时名称。在这个函数
                if (auto *op = dyn_cast<BitCastInst>(&I))
                {
                    continue;
                    errs() << "bitcast" << I << '\n';
                    Value *arg1 = op->getOperand(0); // %4 = xxx
                    errs() << "   " << *arg1 << '\n';

                    std::string varName = arg1->getName().str();
                    // 检查该变量是否存在
                    if (!isKeyOrStateVar(op, Ctx, filename, varName, ju))
                    {
                        continue;
                    }
                    std::string sv = ju->getVarname(filename, varName);
                    int type = ju->getType(filename, sv);

                    // %0 = bitcast [4 x i8]* %s2 to i8*, !dbg !860
                    // call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 1 %0, i8* align 1 getelementptr inbounds ([4 x i8], [4 x i8]* @__const.main.s2, i32 0, i32 0), i64 4, i1 false), !dbg !860
                    Value::use_iterator U = op->use_begin();

                    while (U != op->use_end())
                    {
                        errs() << "  op->use: " << *(U->getUser()) << "\n";

                        if (auto *callOp = dyn_cast<CallInst>(U->getUser()))
                        {
                            log_char_array_call(filename, sv, type, callOp, B, logFuncCharArray, callOp->getContext());
                        }

                        ++U;
                    }
                }

                // if (!isa<StoreInst>(&I) && !isa<LoadInst>(&I))
                // {
                //   continue;
                // }

                if (auto *op = dyn_cast<LoadInst>(&I))
                {
                    Value *arg1 = op->getOperand(0);
                    std::string varName = arg1->getName().str();
                    // 检查该变量是否存在
                    if (!isKeyOrStateVar(op, Ctx, filename, varName, ju))
                    {
                        continue;
                    }
                    std::string sv = ju->getVarname(filename, varName);
                    int type = ju->getType(filename, sv);

                    Type *value_ir_type = op->getPointerOperandType()->getContainedType(0);
                    if (value_ir_type->isIntegerTy())
                    {
                        log_int_load(filename, sv, type, op, B, logFuncInt, Ctx);
                    }
                    else if (value_ir_type->isPointerTy())
                    {
                        // pointer
                        // 1. char*
                        log_char_asterisk_load(filename, sv, type, op, B, logFuncCharAsterisk, Ctx);

                        // 2. string
                    }
                    else if (value_ir_type->isFloatTy())
                    {
                        // float
                        log_fp_load(filename, sv, type, op, B, logFuncFloat, Ctx);
                    }
                    else if (value_ir_type->isDoubleTy())
                    {
                        // double
                        log_fp_load(filename, sv, type, op, B, logFuncDouble, Ctx);
                    }
                    // int
                }

                if (auto *op = dyn_cast<StoreInst>(&I))
                {

                    // get left: value
                    Value *arg1 = op->getOperand(0); // %4 = xxx
                    Value *arg2 = op->getOperand(1);

                    // errs() << "store" << *op << '\n';

                    std::string varName = arg2->getName().str();
                    // 检查该变量是否存在
                    if (!isKeyOrStateVar(op, Ctx, filename, varName, ju))
                    {
                        continue;
                    }
                    std::string sv = ju->getVarname(filename, varName);
                    int type = ju->getType(filename, sv);

                    Type *value_ir_type = arg1->getType();
                    // store i32 2, i32* %i1, align 4, !dbg !886
                    if (value_ir_type->isIntegerTy())
                    {
                        // log_int_store(filename, varname, type, op, B, logFuncInt, Ctx);

                        unsigned int_bit_width = value_ir_type->getIntegerBitWidth();
                        errs() << "IntegerType" << int_bit_width << "\n";
                        if (int_bit_width == 1)
                        {
                            log_int_store(filename, sv, type, op, B, logFuncBool, Ctx);
                        }
                        else if (int_bit_width == 8)
                        {
                            // 1. bool
                            if (auto constant_int = dyn_cast<ConstantInt>(arg1))
                            {
                                int value = constant_int->getSExtValue();
                                if (value == 0 | value == 1)
                                {
                                    log_int_store(filename, sv, type, op, B, logFuncBool, Ctx);
                                }
                                else
                                {
                                    log_int_store(filename, sv, type, op, B, logFuncChar, Ctx);
                                }
                            }
                            else
                            {
                                // errs() << "ERROR: i8 无 int 值。\n";
                                log_int_store(filename, sv, type, op, B, logFuncChar, Ctx);
                            }
                        }
                        else if (int_bit_width == 32)
                        {
                            log_int_store(filename, sv, type, op, B, logFuncInt, Ctx);
                        }
                        else
                        {
                            // errs() << "ERROR: integer 无归属。\n";
                            // 可能是 i64
                            log_int_store(filename, sv, type, op, B, logFuncInt, Ctx);
                        }
                    }
                    else if (value_ir_type->isPointerTy())
                    {
                        // pointer: char*, char[], string

                        // char *
                        // char* s3 = "1234";

                        // store
                        // arg1: i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str, i64 0, i64 0),
                        // arg2: i8** %s3, align 8, !dbg !860
                        // errs() << ">>> " << *arg1 << "    " << *arg2 << "\n;";

                        log_fp_store(filename, sv, type, op, B, logFuncCharAsterisk, Ctx);
                    }
                    else if (value_ir_type->isFloatTy())
                    {
                        // float
                        log_fp_store(filename, sv, type, op, B, logFuncFloat, Ctx);
                    }
                    else if (value_ir_type->isDoubleTy())
                    {
                        // double
                        log_fp_store(filename, sv, type, op, B, logFuncDouble, Ctx);
                    }
                }
            }
        }
        errs() << "FINISH " << F.getName() << argumnents << "\n";
    }

    // 对源码进行了修改返回 true
    return true;
}

/********************************  pass 的主体  *******************************/

// namespace
// {
//     // 继承自 FunctionPass
//     struct SkeletonPass : public ModulePass
//     {
//         static char ID;
//         SkeletonPass() : ModulePass(ID) {}

//         virtual bool runOnModule(Module &M)
//         {
//             return Impl.runImpl(M);
//         }

//     private:
//         Skeleton Impl;
//     };
// }

/********************************  pass 的注册  *******************************/
// // 注册 pass 并且自启动
// char SkeletonPass::ID = 0;

// // Automatically enable the pass.
// // http://adriansampson.net/blog/clangpass.html
// static void registerSkeletonPass(const PassManagerBuilder &, legacy::PassManagerBase &PM)
// {
//     PM.add(new SkeletonPass());
// }
// static RegisterStandardPasses RegisterMyPass(
//     PassManagerBuilder::EP_EarlyAsPossible,
//     registerSkeletonPass);

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getInjectFuncCallPluginInfo()
{
    return {LLVM_PLUGIN_API_VERSION, "skeleton", LLVM_VERSION_STRING,
            [](PassBuilder &PB)
            {
                PB.registerPipelineParsingCallback(
                    [](StringRef Name, ModulePassManager &MPM,
                       ArrayRef<PassBuilder::PipelineElement>)
                    {
                        if (Name == "skeleton")
                        {
                            MPM.addPass(Skeleton());
                            return true;
                        }
                        return false;
                    });
            }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo()
{
    return getInjectFuncCallPluginInfo();
}
