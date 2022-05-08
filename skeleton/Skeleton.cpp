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

using namespace llvm;

namespace {  
  // 继承自 FunctionPass
  struct SkeletonPass : public FunctionPass {
    static char ID;
    SkeletonPass() : FunctionPass(ID) {}

    void printFunctionName(Function &F) {
      DISubprogram* DI = F.getSubprogram();
      if(!DI) {
        errs() << "Function " << F.getName() << " does not have a subprogram\n";
        return;
      } 
      StringRef IRName = F.getName();
      StringRef SourceName = DI->getName();
      StringRef LinkageName = DI->getLinkageName();

      errs() << "the ir name " << IRName << "\n";
      errs() << "the source name is " << SourceName << "\n";
      errs() << "the linkage name is " << LinkageName << "\n";
      errs() << "\n";
    }

    void getMDNodes(Function &F){
      // MDNode* meta;
      // int offset;
      SmallVector<std::pair<unsigned, MDNode *>, 4> MDs;
      F.getAllMetadata(MDs);  // MDs 的大小一直都是 0
      // errs() << "F 的 METADATA node 数目为 " << (MDs.size()) << " 嘤嘤嘤";
      for (auto &MD : MDs) {
        if (MDNode *N = MD.second) {
          if (auto *subProgram = dyn_cast<DISubprogram>(N)) {
            errs() << subProgram->getLine();
          }
        }
      }
    }

    void getLocalVariables(Function &F) {
      // not test yet
      ValueSymbolTable *vst = F.getValueSymbolTable();
      errs() << (*vst).size() << "\n.";

      for (ValueSymbolTable::iterator VI = vst->begin(), VE = vst->end(); VI != VE; ++VI) {
        Value *V = VI->getValue();
        if (!isa<GlobalValue>(V) || cast<GlobalValue>(V)->hasLocalLinkage()) {
          if (!V->getName().startswith("llvm.dbg"))
            // Set name to "", removing from symbol table!
            V->setName("");
        }
      }
    }

    void getFunSubprogram(Function &F) {
      if (auto *subprogram = F.getSubprogram())
        errs() << "Subprogram: " << subprogram->getLine() << "\n";
      else
        errs() << "has no subprogram" << "\n";
    }
    
    // IR 指令在源代码中的行号
    Value* getLine(StoreInst *inst, LLVMContext &Ctx) {
      if (DILocation *DILoc = inst->getDebugLoc()) {
        return ConstantInt::get(Type::getInt32Ty(Ctx), DILoc->getLine());
      }
      return ConstantInt::get(Type::getInt32Ty(Ctx), 0);
    }
    
    virtual bool runOnFunction(Function &F) {
      // Get the function to call from our runtime library.
      LLVMContext &Ctx = F.getContext();
      // 函数参数：
      std::vector<Type*> paramTypesInt = {
        Type::getInt32Ty(Ctx),  // line
        Type::getInt8PtrTy(Ctx),  // name
        Type::getInt32Ty(Ctx)  // state
      };
      std::vector<Type*> paramTypesCharBool = {
        Type::getInt32Ty(Ctx),  // line
        Type::getInt8PtrTy(Ctx),  // name
        Type::getInt8Ty(Ctx)  // state
      };
      std::vector<Type*> paramTypesString = {
        Type::getInt32Ty(Ctx),  // line
        Type::getInt8PtrTy(Ctx),  // name
        Type::getInt8PtrTy(Ctx)  // state
        // Type::getFloatTy(Ctx)
      };
      // 函数返回值：
      Type *retType = Type::getVoidTy(Ctx);
      // 函数类型：
      FunctionType *logFuncIntType = FunctionType::get(retType, paramTypesInt, false);
      FunctionType *logFuncStringType = FunctionType::get(retType, paramTypesString, false);
      FunctionType *logFuncCharBoolType = FunctionType::get(retType, paramTypesCharBool, false);
      // 根据函数的名字获取该函数：
      FunctionCallee logFuncInt = F.getParent()->getOrInsertFunction("loglinevarint", logFuncIntType);
      FunctionCallee logFuncBool = F.getParent()->getOrInsertFunction("loglinevarbool", logFuncIntType);
      FunctionCallee logFuncChar = F.getParent()->getOrInsertFunction("loglinevarchar", logFuncIntType);
      FunctionCallee logFuncString = F.getParent()->getOrInsertFunction("loglinevarstring", logFuncIntType);

      // errs() << "\n\n" << "Function: " << *(logFunc.getCallee()) << '\n';
      // errs() << "" << "FUNC: " << F.getName() << '\n';
      // printFunctionName(F);
      // getMDNodes(F);
      // getFunSubprogram(F);

      for (auto &B : F) {
        for (auto &I : B) {

          // if (auto *op = dyn_cast<LoadInst>(&I)) {
          //   int number;
          //   // op: %4 = load i32, i32* %3, align 4, !dbg !862
          //   // op->getOperand(0): %3 = alloca i32, align 4
          //   auto alloca = dyn_cast<AllocaInst>(op->getOperand(0));
          //   // errs() << *alloca << ".Load Alloca\n";

          //   for (auto user: alloca->users()) {
          //     // %4 = load i32, i32* %3, align 4
          //     // store i32 1, i32* %3, align 4
          //     if (auto store = dyn_cast<StoreInst>(user)) {
          //       auto constant_int = dyn_cast<ConstantInt>(store->getOperand(0));
          //       number = constant_int->getSExtValue();
          //       // errs() << number << "***\n";
          //     }
          //   }
          // }

          if (auto *op = dyn_cast<StoreInst>(&I)) {
            // get left: value
            Value *arg1 = op->getOperand(0);  // %4 = xxx
            Value *arg2 = op->getOperand(1);
            
            // for project
            // if (arg2->getName().str() != "m_check_state") {
            //   continue;
            // }

            errs() << "【" << I << "】" << "\n"; 
            errs() << *(arg1->getType()) << '\n';
            errs() << "StoreInst L: " << *arg1 << ": [" << arg1->getName() << "]\n";
            Type* value_ir_type = arg1->getType();
            if (value_ir_type->isIntegerTy()) {
              unsigned int_bit_width = value_ir_type->getIntegerBitWidth();              
              errs() << "IntegerType" << int_bit_width << "\n";
              if (int_bit_width == 1) {
                log_line_var_bool(op, B, logFuncBool, Ctx);
              }
              else if (int_bit_width == 8) {
                // 1. bool
                if (auto constant_int = dyn_cast<ConstantInt>(arg1)) {
                  int value = constant_int->getSExtValue();                  
                  if (value == 0 | value == 1) {
                    log_line_var_bool(op, B, logFuncBool, Ctx);
                  } else {
                    log_line_var_char(op, B, logFuncChar, Ctx);
                  } 
                } 
                else {
                  // errs() << "ERROR: i8 无 int 值。\n";
                  log_line_var_char(op, B, logFuncChar, Ctx);
                }
              } 
              else if (int_bit_width == 32) {
                log_line_var_int(op, B, logFuncInt, Ctx);
              }
              else {
                errs() << "ERROR: integer 无归属。\n";
              }
            } 
            else if (value_ir_type->isPointerTy()) {
              errs() << "PointerType" << "\n";
              // log_line_var_int(op, B, logFuncString, Ctx);
            }
            else if (value_ir_type->isFloatTy()) {
              errs() << "FloatType" << "\n";
            }
            else if (value_ir_type->isDoubleTy()) {
              errs() << "DoubleType" << "\n";
            }            
          }
        }
      }
      
      return true;
    }

    /****************** instrumented functions *******************/
    void log_line_var_int(StoreInst *inst, BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx) {
      IRBuilder<> builder(inst);
      builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

      // get right: name            
      Value *arg2 = inst->getOperand(1);  
      errs() << "StoreInst R: " << *arg2 << ": [" << arg2->getName() << "]\n";

      Value* argstr = builder.CreateGlobalString(arg2->getName());    
      Value* argi = inst->getOperand(0);
      Value* argline = getLine(inst, Ctx);

      Value* args[] = {argline, argstr, argi};  // 
      builder.CreateCall(logFunc, args);
    }

    
    void log_line_var_bool(StoreInst *inst, BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx) {
      IRBuilder<> builder(inst);
      builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

      // get right: name            
      Value *arg2 = inst->getOperand(1);  
      errs() << "StoreInst R: " << *arg2 << ": [" << arg2->getName() << "]\n";

      Value* argstr = builder.CreateGlobalString(arg2->getName());    
      Value* argi = inst->getOperand(0);
      Value* argline = getLine(inst, Ctx);

      Value* args[] = {argline, argstr, argi};  // 
      builder.CreateCall(logFunc, args);
    }

    
    void log_line_var_char(StoreInst *inst, BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx) {
      IRBuilder<> builder(inst);
      builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

      // get right: name            
      Value *arg2 = inst->getOperand(1);  
      errs() << "StoreInst R: " << *arg2 << ": [" << arg2->getName() << "]\n";

      Value* argstr = builder.CreateGlobalString(arg2->getName());    
      Value* argi = inst->getOperand(0);
      Value* argline = getLine(inst, Ctx);

      Value* args[] = {argline, argstr, argi};  // 
      builder.CreateCall(logFunc, args);
    }

    
    void log_line_var_string(StoreInst *inst, BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx) {
      IRBuilder<> builder(inst);
      builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

      // get right: name            
      Value *arg2 = inst->getOperand(1);  
      errs() << "StoreInst R: " << *arg2 << ": [" << arg2->getName() << "]\n";

      Value* argstr = builder.CreateGlobalString(arg2->getName());    
      Value* argi = inst->getOperand(0);
      Value* argline = getLine(inst, Ctx);

      Value* args[] = {argline, argstr, argi};  // 
      builder.CreateCall(logFunc, args);
    }

    void log_line_var(StoreInst *inst, BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx) {
      IRBuilder<> builder(inst);
      builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

      // get right: name            
      Value *arg2 = inst->getOperand(1);  
      errs() << "StoreInst R: " << *arg2 << ": [" << arg2->getName() << "]\n";

      Value* argstr = builder.CreateGlobalString(arg2->getName());    
      Value* argline = getLine(inst, Ctx);

      Value* argi = inst->getOperand(0);

      Value* args[] = {argline, argstr, argi};  // 
      builder.CreateCall(logFunc, args);
    }

    void log_line_var_old(StoreInst *inst, BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx) {
      IRBuilder<> builder(inst);
      builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

      // get right: name            
      Value *arg2 = inst->getOperand(1);  // %2 = xxx
      Instruction *arg2ins = dyn_cast<Instruction>(arg2);
      while (arg2ins->getOpcode() != Instruction::Alloca) {
        // errs() << "        R: " << *arg2ins << "\n";
        arg2ins = dyn_cast<Instruction>(arg2ins->getOperand(1));
      }
      errs() << "StoreInst R: " << *arg2ins << ": [" << arg2ins->getName() << "]\n";
      Value *argstr = builder.CreateGlobalString(arg2ins->getName());    

      Value *argi = inst->getOperand(0);  // left

      // argline

      Value* argline = getLine(inst, Ctx);

      Value* args[] = {argline, argstr, argi};  // 
      builder.CreateCall(logFunc, args);
      if (auto constant_int = dyn_cast<ConstantInt>(argi)) {
        errs() << "store inst has instance number" << constant_int << "\n";
      } else {
        errs() << "store inst has no instance number" << "\n";
      }
    }

    void logvar(StoreInst *inst, BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx) {
      IRBuilder<> builder(inst);
      builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

      // get right: name            
      Value *arg2 = inst->getOperand(1);  // %2 = xxx
      Instruction *arg2ins = dyn_cast<Instruction>(arg2);
      while (arg2ins->getOpcode() != Instruction::Alloca) {
        // errs() << "        R: " << *arg2ins << "\n";
        arg2ins = dyn_cast<Instruction>(arg2ins->getOperand(1));
      }
      errs() << "StoreInst R: " << *arg2ins << ": [" << arg2ins->getName() << "]\n";
      Value* argstr = builder.CreateGlobalString(arg2ins->getName());    

      Value* argi = inst->getOperand(0);  // left
      Value* args[] = {argi, argstr};  // 
      builder.CreateCall(logFunc, args);
      if (auto constant_int = dyn_cast<ConstantInt>(argi)) {
        errs() << "store inst has instance number" << constant_int << "\n";
      } else {
        errs() << "store inst has no instance number" << "\n";
      }
    }

    // int and float function test
    void logif(StoreInst *inst, BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx) {
      IRBuilder<> builder(inst);
      builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

      Value *argi = inst->getOperand(0);  // left
      if (auto constant_int = dyn_cast<ConstantInt>(argi)) {
        Value* argf = ConstantFP::get(Type::getFloatTy(Ctx), 1);
        Value* args[] = {argi, argf};  // 
        builder.CreateCall(logFunc, args);
      } else {
        errs() << "store inst has no instance number" << "\n";
      }
    }

    void boComputed(BinaryOperator *op, BasicBlock &B, FunctionCallee logFunc) {
      IRBuilder<> builder(op);
      builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

      Value* args[] = {op};
      builder.CreateCall(logFunc, args);
    }
  };
}

char SkeletonPass::ID = 0;

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerSkeletonPass(const PassManagerBuilder &, legacy::PassManagerBase &PM) {
  PM.add(new SkeletonPass());
}
static RegisterStandardPasses RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerSkeletonPass);
