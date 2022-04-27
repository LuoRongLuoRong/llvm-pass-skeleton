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
        // Type::getInt8PtrTy(Ctx)
        Type::getFloatTy(Ctx)
      };
      // 函数返回值：
      Type *retType = Type::getVoidTy(Ctx);
      // 函数类型：
      FunctionType *logFuncType = FunctionType::get(retType, paramTypes, false);
      // 根据函数的名字获取该函数：
      // FunctionCallee logFunc = F.getParent()->getOrInsertFunction("logvar", logFuncType);
      FunctionCallee logFunc = F.getParent()->getOrInsertFunction("logif", logFuncType);

      // errs() << "\n\n" << "Function: " << *(logFunc.getCallee()) << '\n';
      errs() << "\n\n" << "FUNC: " << F.getName() << '\n';
      // printFunctionName(F);
      // getMDNodes(F);
      // getFunSubprogram(F);
      for (auto &B : F) {
        for (auto &I : B) {
          
          getLocalVariables(F);
          errs() << "【" << I.getName() << "】" << I << " " << "\n.";

          // get metadata
          if (auto *inst = dyn_cast<ReturnInst>(&I)) {
            // call void @llvm.dbg.declare(metadata i32* %2, metadata !858, metadata !DIExpression()), !dbg !859
            // ret i32 0, !dbg !865
            errs() << "\n!!!return: " << *inst << "\n";

            if (DILocation *DILoc = inst->getDebugLoc()) {
              errs() << "   DILocation: " << *DILoc <<  ".\n";
              // DILocation: !DILocation(line: 74, column: 25, scope: <0x8ddaee0>) = !DILocation(line: 74, column: 25, scope: <0x8ddaee0>).
              errs() << "        line : " << DILoc->getLine() <<  ".\n";
              errs() << "        col  : " << DILoc->getColumn() <<  ".\n";
              // errs() << "        scope: " << *(DILoc->getScope()) <<  ".\n";
            }

            if (Value *retVal = inst->getReturnValue()) {
              errs() << "   ret_value: " << *retVal << "\n";

              if (Type *instTy = retVal->getType())
                errs() << "   ret_value_type: " << *instTy << "\n";

              if (auto constant_int = dyn_cast<ConstantInt>(retVal)) {
                errs() << "   val_number: " << constant_int->getSExtValue() << ".\n";
              }
            }
          }
          // get varibale name
          if (auto *instT = dyn_cast<StoreInst>(&I)) {
            // logif(instT, B, logFunc, Ctx);
            // Value* val = dyn_cast<Value>(op->getOperand(0));
            // while(val){
            //   Instruction* in = dyn_cast<Instruction>(val);
            //   Value *vl = in->getOperand(0);
            // }

            // 大哥的代码
            // while (instT && instT->getOpcode() != Instruction::Alloca) {
            //   Value* val = dyn_cast<Value>(instT);
            //   print(val->getName().str());
            //   instT = dyn_cast<Instruction>(instT->getOperand(0));
            // }
            
            // while (instT->getOpcode() == Instruction::Store) {
            //   errs() << "Optype:" << instT->getOpcodeName << ".\n";
            //   instT = dyn_cast<StoreInst>(instT->getOperand(1));
            // }
            // if (instT->getOpcode() == Instruction::Alloca) {
            //   Value* val = dyn_cast<Value>(instT);              
            //   errs() << "val name: " << val->getName().str() << ".\n";
            // }
          }

          if (auto *instT = dyn_cast<AllocaInst>(&I)) {
            Value* val = dyn_cast<Value>(instT);              
            // errs() << "Val name: " << val->getName().str() << ".\n";
            // alloca has no name
          }

          if (auto *op = dyn_cast<BinaryOperator>(&I)) {
            // Insert *after* `op`.
            boComputed(op, B, logFunc);
          }

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
            errs() << *arg1 << ": " << arg1->getName() << "·" << *arg2 << ": " << arg2->getName()  << "\n";
            
            auto alloca = dyn_cast<AllocaInst>(arg2);  // 找到 %2 的 metadata，即获取其名字
          
            if (alloca->hasMetadata()) {
              errs() << "...has md...";
            }
            unsigned mk = alloca->getContext().getMDKindID("dbg");
            if (MDNode *mdn = alloca->getMetadata(mk)) {
              Metadata *mds = mdn->getOperand(0);

              StringRef str;
              if (MDString::classof(mds)) {
                str = (cast<MDString>(*mds)).getString();
                errs() << str;
              }
            } else {
              errs() << "no mk!\n";
            }
          }
          if (auto *instT = dyn_cast<StoreInst>(&I)) {
            logif(instT, B, logFunc, Ctx);
          }
        }      
      }
      
      return false;
    }

          //   if(dyn_cast<ConstantInt>(arg1)) {
          //     errs() << "arg1 is constant int";              
          //   } else {
          //     errs() << "arg1 is not constant int";
          //   }

            // instrument: Insert *after* `op`.
            // IRBuilder<> builder(op);
            // builder.SetInsertPoint(&B, ++builder.GetInsertPoint());
            // Value *helloWorld = builder.CreateGlobalString("Hello World");
            // // Value *argStr = dyn_cast<ConstantP>(helloWorld);
                        
            // Value *valInt = ConstantInt::get(Type::getInt32Ty(Ctx), 1);
           

            // Value *args[] = {arg1, };
            // builder.CreateCall(logFunc, args);

    // int and float function test
    void logif(StoreInst *inst, BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx) {
      IRBuilder<> builder(inst);
      builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

      Value *argi = inst->getOperand(0);  // left
      if (auto constant_int = dyn_cast<ConstantInt>(argi)) {
        Value *argf = ConstantFP::get(Type::getFloatTy(Ctx), 1);
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
