//========================================================================
// FILE:
//    InjectFuncCall.cpp
//
// DESCRIPTION:
//    For each function defined in the input IR module, InjectFuncCall inserts
//    a call to printf (from the C standard I/O library). The injected IR code
//    corresponds to the following function call in ANSI C:
//    ```C
//      printf("(llvm-tutor) Hello from: %s\n(llvm-tutor)   number of arguments: %d\n",
//             FuncName, FuncNumArgs);
//    ```
//    This code is inserted at the beginning of each function, i.e. before any
//    other instruction is executed.
//
//    To illustrate, for `void foo(int a, int b, int c)`, the code added by InjectFuncCall
//    will generated the following output at runtime:
//    ```
//    (llvm-tutor) Hello World from: foo
//    (llvm-tutor)   number of arguments: 3
//    ```
//
// USAGE:
//    1. Legacy pass manager:
//      $ opt -load <BUILD_DIR>/lib/libInjectFuncCall.so `\`
//        --legacy-inject-func-call <bitcode-file>
//    2. New pass maanger:
//      $ opt -load-pass-plugin <BUILD_DIR>/lib/libInjectFunctCall.so `\`
//        -passes=-"inject-func-call" <bitcode-file>
//
// License: MIT
//========================================================================

#include "InjectFuncCall.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/PassBuilder.h"

// using namespace llvm;

#define DEBUG_TYPE "inject-func-call"

//-----------------------------------------------------------------------------
// 构造函数：初始化全局变量
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Helper function provided by LLVM
//-----------------------------------------------------------------------------

// IR 指令在源代码中的行号
int InjectFuncCall::getLineNumber(Instruction *inst, LLVMContext &CTX)
{
  if (DILocation *DILoc = inst->getDebugLoc())
  {
    return DILoc->getLine();
  }
  return 0;
}

// IR 指令在源代码中的行号的 Value
Value *InjectFuncCall::getLine(Instruction *inst, LLVMContext &CTX)
{
  return ConstantInt::get(Type::getInt32Ty(CTX), getLineNumber(inst, CTX));
}

//-----------------------------------------------------------------------------
// InjectFuncCall implementation
//-----------------------------------------------------------------------------

FunctionCallee InjectFuncCall::getPrintf(Module &M)
{

  auto &CTX = M.getContext();

  // Create (or _get_ in cases where it's already available) the following
  // declaration in the IR module:
  //    declare i32 @printf(i8*, ...)
  // It corresponds to the following C declaration:
  //    int printf(char *, ...)
  FunctionType *PrintfTy = FunctionType::get(
      IntegerType::getInt32Ty(CTX),                 // return type
      PointerType::getUnqual(Type::getInt8Ty(CTX)), // char *
      /*IsVarArgs=*/true);

  FunctionCallee Printf = M.getOrInsertFunction("printf", PrintfTy);
  // Set attributes as per inferLibFuncAttributes in BuildLibCalls.cpp
  Function *PrintfF = dyn_cast<Function>(Printf.getCallee());
  PrintfF->setDoesNotThrow();
  PrintfF->addParamAttr(0, Attribute::NoCapture);
  PrintfF->addParamAttr(0, Attribute::ReadOnly);

  return Printf;
}

void insertRedirection(Module &M)
{
  auto &CTX = M.getContext();
  // redirect2file
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
    Constant *PrintfFormatStr = llvm::ConstantDataArray::getString(
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
    Value *FormatStrPtr =
        Builder.CreatePointerCast(PrintfFormatStrVar, PrintfArgTy, "formatStr");

    // Finally, inject a call to printf
    Builder.CreateCall(
        Printf, {FormatStrPtr, FuncName, Builder.getInt32(F.arg_size())});
}

void InjectFuncCall::insertPrintf(LLVMContext &CTX, Function &F, Instruction *inst, BasicBlock &B,
                                  FunctionCallee Printf, Constant *PrintfFormatStrVar,
                                  std::string filename, std::string varname, int type,
                                  Value *variableValue)
{
  // Get an IR builder. Sets the insertion point to the top of the function
  IRBuilder<> Builder(inst);
  Builder.SetInsertPoint(&B, ++Builder.GetInsertPoint());

  // Printf requires i8*, but PrintfFormatStrVar is an array: [n x i8]. Add
  // a cast: [n x i8] -> i8*
  llvm::Value *FormatStrPtr = Builder.CreatePointerCast(
      PrintfFormatStrVar,
      PointerType::getUnqual(Type::getInt8Ty(CTX)),
      "formatStr");

  Value *args[] = {
      FormatStrPtr,
      /*filename=*/Builder.CreateGlobalStringPtr(filename),
      /*value_type=*/ConstantInt::get(Type::getInt32Ty(CTX), type),
      /*variable_name=*/Builder.CreateGlobalStringPtr(varname),
      /*line_of_code=*/getLine(inst, CTX),
      /*variable_value=*/variableValue};

  // Finally, inject a call to printf
  Builder.CreateCall(Printf, args);
}

bool InjectFuncCall::runOnModule(Module &M)
{
  // STEP 1: Inject the declaration of printf
  // ----------------------------------------
  JsonUtil *ju = JsonUtil::getInstance();
  ju->save();

  std::string filename = M.getSourceFileName();
  // 该文件不值得继续探索
  if (!ju->inFilepaths(filename))
  {
    errs() << "该文件不值得继续探索: " << filename << "\n";
    return false;
  }
  errs() << "该文件值得继续探索: " << filename << "\n";

  auto &CTX = M.getContext();

  // STEP 1: Inject the declaration of printf
  // ----------------------------------------
  auto Printf = getPrintf(M);

  // STEP 2: Inject a global variable that will hold the printf format string
  // ------------------------------------------------------------------------
  llvm::Constant *PrintfFormatStr = 
    llvm::ConstantDataArray::getString(CTX, "(luorong) %s, %d, %s, %d, %d\n");

  Constant *PrintfFormatStrVar = M.getOrInsertGlobal("PrintfFormatStr", PrintfFormatStr->getType());
  dyn_cast<GlobalVariable>(PrintfFormatStrVar)->setInitializer(PrintfFormatStr);

  // STEP 3: For each function in the module, inject a call to printf
  // ----------------------------------------------------------------
  for (auto &F : M)
  {
    // 防止重复遍历
    if (F.isDeclaration())
    {
      continue;
    }
    for (auto &B : F)
    {
      for (auto &I : B)
      {
        if (auto *inst = dyn_cast<LoadInst>(&I))
        {
          // For example, %5 = load i32, i32* %i1, align 4, !dbg !861
          // arg1 is %i1
          Value *arg1 = inst->getOperand(0);
          // e.g., %i1 name is i1
          std::string varName = arg1->getName().str();
          // e.g., %i1 type is i32
          Type *varIrType = inst->getPointerOperandType()->getContainedType(0);

          if (varIrType->isIntegerTy())
          {
            insertPrintf(CTX, F, inst, B, Printf, PrintfFormatStrVar,
                         filename, varName, 1,
                         /*run time value of the variable*/ dyn_cast_or_null<Value>(inst));
          }
        }

        if (auto *inst = dyn_cast<StoreInst>(&I))
        {
          // For example, store i32 2, i32* %i1, align 4, !dbg !860
          Value *arg1 = inst->getOperand(0); // i32 2
          Value *arg2 = inst->getOperand(1); // i32* %i1

          std::string varName = arg2->getName().str();
          // e.g., %i1 type is i32
          Type *varIrType = inst->getPointerOperandType()->getContainedType(0);

          if (varIrType->isIntegerTy())
          {
            insertPrintf(CTX, F, inst, B, Printf, PrintfFormatStrVar,
                         filename, varName, 1,
                         /*run time value of the variable*/ arg1);
          }
        }
      }
    }
  }
  // has instrumented
  return true;
}

PreservedAnalyses InjectFuncCall::run(llvm::Module &M,
                                      llvm::ModuleAnalysisManager &)
{
  bool Changed = runOnModule(M);

  return (Changed ? llvm::PreservedAnalyses::none()
                  : llvm::PreservedAnalyses::all());
}

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getInjectFuncCallPluginInfo()
{
  return {LLVM_PLUGIN_API_VERSION, "inject-func-call", LLVM_VERSION_STRING,
          [](PassBuilder &PB)
          {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>)
                {
                  if (Name == "inject-func-call")
                  {
                    MPM.addPass(InjectFuncCall());
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
