//==============================================================================
// FILE:
//    InjectFuncCall.h
//
// DESCRIPTION:
//    Declares the InjectFuncCall pass for the new and the legacy pass managers.
//
// License: MIT
//==============================================================================
#ifndef LLVM_TUTOR_INSTRUMENT_BASIC_H
#define LLVM_TUTOR_INSTRUMENT_BASIC_H

#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"

using namespace llvm;

//------------------------------------------------------------------------------
// New PM interface
//------------------------------------------------------------------------------
struct InjectFuncCall : public PassInfoMixin<InjectFuncCall>
{
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

  int getLineNumber(Instruction *inst, LLVMContext &CTX);
  Value *getLine(Instruction *inst, LLVMContext &CTX);

  FunctionCallee getPrintf(Module &M);
  void insertPrintf(LLVMContext &CTX, Function &F, Instruction *inst, BasicBlock &B,
                    FunctionCallee Printf, Constant *PrintfFormatStrVar,
                    std::string filename, std::string varname, int type,
                    Value* variableValue);

  PreservedAnalyses run(Module &M, ModuleAnalysisManager &);
  bool runOnModule(Module &M);
};

#endif
