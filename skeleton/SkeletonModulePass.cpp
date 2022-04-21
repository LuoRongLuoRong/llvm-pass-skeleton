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
  struct SkeletonModulePass : public ModulePass {
    static char ID;
    SkeletonModulePass() : ModulePass(ID) {}

    virtual bool runOnModule(Module &M) {
      // errs() << "Modelue!" << M.getName() << "\n";
      for (auto &F : M) {
        // errs() << F.getName() << "\n";

        // https://stackoverflow.com/questions/71542467/get-name-from-llvm-debug-metadata
        
        // DISubprogram* DI = F.getSubprogram();
        
        // if(!DI) {
        //   errs() << "Function " << F.getName() << " does not have a subprogram\n";
        //   continue;
        // } else {
        //   errs() << "DI" << DI->getName() << "\n";
        // }

        // StringRef IRName = F.getName();
        // StringRef SourceName = DI->getName();
        // StringRef LinkageName = DI->getLinkageName();

        // errs() << "the ir name " << IRName << "\n";
        // errs() << "the source name is " << SourceName << "\n";
        // errs() << "the linkage name is " << LinkageName << "\n";
        // errs() << "\n";

      }
      return false;
    }
  };
}

char SkeletonModulePass::ID = 0;

static void registerSkeletonModulePass(const PassManagerBuilder &, legacy::PassManagerBase &PM) {
  PM.add(new SkeletonModulePass());
}
static RegisterStandardPasses RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerSkeletonModulePass);
