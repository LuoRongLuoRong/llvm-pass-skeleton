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
      ValueSymbolTable *vst = F.getValueSymbolTable();
      // errs() << "LocalVariables:" << vst << '\n';

      // for (auto vs : &vst) {
      //   auto s = vs.getKey();
      //   auto v = vs.getValue();
      // }
      // for (auto it : &vst){
	    //   errs() << it.first <<" "<< it.second << '\n';
      // }

    }

    void getFunSubprogram(Function &F) {
      if (auto *subprogram = F.getSubprogram())
        errs() << "Subprogram: " << subprogram->getLine() << "\n";
      else
        errs() << "has no subprogram" << "\n";
    }
    
    virtual bool runOnFunction(Function &F) {
      // Get the function to call from our runtime library.
      LLVMContext &Ctx = F.getContext();
      // 函数参数：
      std::vector<Type*> paramTypes = {
        Type::getInt32Ty(Ctx),
        Type::getInt8PtrTy(Ctx)
        // Type::getFloatTy(Ctx)
      };
      // 函数返回值：
      Type *retType = Type::getVoidTy(Ctx);
      // 函数类型：
      FunctionType *logFuncType = FunctionType::get(retType, paramTypes, false);
      // 根据函数的名字获取该函数：
      FunctionCallee logFunc = F.getParent()->getOrInsertFunction("logvar", logFuncType);
      // FunctionCallee logFunc = F.getParent()->getOrInsertFunction("logif", logFuncType);

      // errs() << "\n\n" << "Function: " << *(logFunc.getCallee()) << '\n';
      errs() << "\n\n" << "FUNC: " << F.getName() << '\n';
      // printFunctionName(F);
      // getMDNodes(F);
      // getFunSubprogram(F);
      for (auto &B : F) {
        for (auto &I : B) {
          errs() << "【" << I.getName() << "】" << I << " " << "\n";

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
            errs() << "StoreInst L: " << *arg1 << ": [" << arg1->getName() << "]\n";
            // 1. is a int value
            if (auto constant_int = dyn_cast<ConstantInt>(arg1)) {
              int number = constant_int->getSExtValue();
              errs() << "StoreInst L: value = [" << number << "].\n";
            } 
            // 2. is a float value
            else if (auto constant_fp = dyn_cast<ConstantFP>(arg1)) {
              // float number = constant_fp->getValueAPF();
              // errs() << number << ".\n";
            } 
            // 3. ...
            else {

            }
            
            logvar(op, B, logFunc, Ctx);
          }
        }
      }
      
      return false;
    }

    void logvar(StoreInst *inst, BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx) {
      IRBuilder<> builder(inst);
      builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

      // get right: name            
      Value *arg2 = inst->getOperand(1);  // %2 = xxx
      Instruction *arg2ins = dyn_cast<Instruction>(arg2);
      while (arg2ins->getOpcode() != Instruction::Alloca) {
        errs() << "        R: " << *arg2ins << "\n";
        arg2ins = dyn_cast<Instruction>(arg2ins->getOperand(1));
      }
      errs() << "StoreInst R: " << *arg2ins << ": [" << arg2ins->getName() << "]\n";
      Value *argstr = builder.CreateGlobalString(arg2ins->getName());    

      Value *argi = inst->getOperand(0);  // left
      if (auto constant_int = dyn_cast<ConstantInt>(argi)) {
        errs() << "store inst has instance number" << constant_int << "\n";
        
        Value* args[] = {argi, argstr};  // 
        builder.CreateCall(logFunc, args);
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
