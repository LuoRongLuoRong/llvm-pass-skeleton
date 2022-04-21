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

    virtual bool runOnFunction(Function &F) {
      // Get the function to call from our runtime library.
      LLVMContext &Ctx = F.getContext();
      // 函数参数：
      std::vector<Type*> paramTypes = {
        Type::getInt32Ty(Ctx),
        // Type::getPointerElementType(Ctx)
        // Type::getTokenTy(Ctx)
      };
      // 函数返回值：
      Type *retType = Type::getVoidTy(Ctx);
      // 函数类型：
      FunctionType *logFuncType = FunctionType::get(retType, paramTypes, false);
      // 根据函数的名字获取该函数：
      FunctionCallee logFunc = F.getParent()->getOrInsertFunction("logop", logFuncType);

      errs() << "\n\n" << "Function: " << *(logFunc.getCallee()) << '\n';

      for (auto &B : F) {
        for (auto &I : B) {          
          if (auto *op = dyn_cast<BinaryOperator>(&I)) {
            // Insert *after* `op`.
            IRBuilder<> builder(op);
            builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

            Value* args[] = {op};
            builder.CreateCall(logFunc, args);
          }

          if (auto *op = dyn_cast<LoadInst>(&I)) {
            int number;
            // op: %4 = load i32, i32* %3, align 4, !dbg !862
            // op->getOperand(0): %3 = alloca i32, align 4
            auto alloca = dyn_cast<AllocaInst>(op->getOperand(0));
            // errs() << *alloca << ".Load Alloca\n";

            for (auto user: alloca->users()) {
              // %4 = load i32, i32* %3, align 4
              // store i32 1, i32* %3, align 4
              if (auto store = dyn_cast<StoreInst>(user)) {
                auto constant_int = dyn_cast<ConstantInt>(store->getOperand(0));
                number = constant_int->getSExtValue();
                // errs() << number << "***\n";
              }
            }
          }

          if (auto *op = dyn_cast<StoreInst>(&I)) {            
            errs() << *op << ".StoreInst\n";
            Value *val = op->getValueOperand();
            if (auto constant_int = dyn_cast<ConstantInt>(val)) {
              int number = constant_int->getSExtValue();
              errs() << number << ".\n";
            } else if (auto constant_fp = dyn_cast<ConstantFP>(val)) {
              // float number = constant_fp->getValueAPF();
              // errs() << number << ".\n";
            }

            // metadata
            // store i32 %4, i32* %2, align 4, !dbg !863
            Value *arg1 = op->getOperand(0);  // %4 = xxx
            Value *arg2 = op->getOperand(1);  // %2 = xxx
            // Value *arg3 = op->getOperand(2);  // %2 = xxx
            // Value *arg4 = op->getOperand(3);  // %2 = xxx
            errs() << arg1 << "·" << arg2 << "\n";

            // !863 = !DILocation(line: 6, column: 7, scope: !857)
            unsigned mk = op->getContext().getMDKindID("dbg");
            MDNode *mdn = op->getMetadata(mk);

            if (mdn) {
              Metadata *mds = mdn->getOperand(0);
              StringRef str;
              if (MDString::classof(mds)) {
                str = (cast<MDString>(*mds)).getString();
                errs() << str;
              }
            } else {
              errs() << "no dbg!\n";
            }
          }
        }
      }
      
      return false;
    }
    


// ; Function Attrs: mustprogress noinline norecurse nounwind optnone uwtable
// define dso_local i32 @main() #4 !dbg !857 {
//   %1 = alloca i32, align 4
//   %2 = alloca i32, align 4
//   %3 = alloca i32, align 4
//   store i32 0, i32* %1, align 4
//   call void @llvm.dbg.declare(metadata i32* %2, metadata !858, metadata !DIExpression()), !dbg !859
//   call void @llvm.dbg.declare(metadata i32* %3, metadata !860, metadata !DIExpression()), !dbg !861
//   store i32 123, i32* %3, align 4, !dbg !861
//   %4 = load i32, i32* %3, align 4, !dbg !862
//   store i32 %4, i32* %2, align 4, !dbg !863
//   ret i32 0, !dbg !864
// }

// !857 = distinct !DISubprogram(name: "main", scope: !8, file: !8, line: 3, type: !539, scopeLine: 3, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !7, retainedNodes: !9)
// !858 = !DILocalVariable(name: "b", scope: !857, file: !8, line: 4, type: !20)
// !859 = !DILocation(line: 4, column: 9, scope: !857)
// !860 = !DILocalVariable(name: "a", scope: !857, file: !8, line: 5, type: !20)
// !861 = !DILocation(line: 5, column: 9, scope: !857)
// !862 = !DILocation(line: 6, column: 9, scope: !857)
// !863 = !DILocation(line: 6, column: 7, scope: !857)
// !864 = !DILocation(line: 7, column: 5, scope: !857)      

          // if (auto *op = dyn_cast<LoadInst>(&I)) {  
          //   // store i32 %4, i32* %2, align 4, !dbg !863
          //   Value *arg1 = op->getOperand(0);  // %4 = xxx
          //   Value *arg2 = op->getOperand(1);  // %2 = xxx

          //   // unsigned mk = op->getContext().getMDKindID("dbg");
          //   // MDNode *mdn = op->getMetadata(mk);

          //   // if (mdn) {
          //   //   Metadata *mds = mdn->getOperand(0);
          //   //   StringRef str;
          //   //   if (MDString::classof(mds)) {
          //   //     str = (cast<MDString>(*mds)).getString();
          //   //     errs() << str;
          //   //   }
          //   // } else {
          //   //   errs() << "no dbg!";
          //   // }

          //   // MDString *mds = dyn_cast_or_null<MDString>(mdn->getOperand(1));
          //   // std::string varName = mds->getString().str();
            
            
          //   errs() << "Store::";
          //   errs() << *arg1 << "\n";
          //   errs() << (op->getMetadata(op->getContext().getMDKindID("dbg"))) << "\n";
          //   errs() << "   Store::arg1->getName() : " << arg1->getName() << "\n\n";

          //   // Insert *after* `op`.
          //   IRBuilder<> builder(op);
          //   builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

          //   if(dyn_cast<ConstantInt>(arg1)) {
          //     errs() << "arg1 is constant int";              
          //   } else {
          //     errs() << "arg1 is not constant int";
          //   }

          //   // Value* args[] = {arg1, "name"};
          //   // builder.CreateCall(logFunc, args);
          // }
//         }
//       }

//       return false;
//     }
  };
}

char SkeletonPass::ID = 0;

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerSkeletonPass(const PassManagerBuilder &, legacy::PassManagerBase &PM) {
  PM.add(new SkeletonPass());
}
static RegisterStandardPasses RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerSkeletonPass);
