#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/IR/Module.h"
using namespace llvm;

namespace {
  // 继承自 FunctionPass
  struct SkeletonPass : public FunctionPass {
    static char ID;
    SkeletonPass() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
      // Get the function to call from our runtime library.
      LLVMContext &Ctx = F.getContext();
      // 函数参数：
      std::vector<Type*> paramTypes = {
        Type::getInt32Ty(Ctx),
        Type::getTokenTy(Ctx)
      };
      // 函数返回值：
      Type *retType = Type::getVoidTy(Ctx);
      // 构造函数类型：
      FunctionType *logFuncType = FunctionType::get(retType, paramTypes, false);
      // 构造函数：
      FunctionCallee logFunc = 
        F.getParent()->getOrInsertFunction("_Z6logvariNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE", logFuncType);
        // F.getParent()->getOrInsertFunction("_Z5logopi", logFuncType);

      errs() << "\n\n" << "LUORONG: Function: " << *(logFunc.getCallee()) << '\n';

      // errs() << "LUORONG: Function: ";
      // errs().write_escaped(F.getName()) << '\n';

      for (auto &B : F) {
        for (auto &I : B) {
          
          // errs() << I << ".\n";
          // errs() << I.getOperand(0) << ".\n";
          // errs() << I.getOpcode() << ".\n";
          // errs() << I.getOpcodeName() << ".\n\n";

          if (auto *op = dyn_cast<StoreInst>(&I)) {

            errs() << *op << ".\n";
            errs() << *(I.getOperand(0)) << ".\n";
            errs() << *(I.getOperand(1)) << ".\n";
            errs() << *(op->getValueOperand()) << ".\n";
            errs() << *(op->getPointerOperand()) << ".\n";
            errs() << *(op->getPointerOperandType()) << ".\n\n";

            // Insert *after* `op`.
            IRBuilder<> builder(op);
            builder.SetInsertPoint(&B, ++builder.GetInsertPoint());
            
            // Insert a call to our function.
            Value* args[] = {, };
            builder.CreateCall(logFunc, args);
            // builder.CreateCall(logFunc);
          }
        }
      }

      // return false;
      return true;
    }
  };
}

char SkeletonPass::ID = 0;

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerSkeletonPass(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
  PM.add(new SkeletonPass());
}
static RegisterStandardPasses
  RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                 registerSkeletonPass);
