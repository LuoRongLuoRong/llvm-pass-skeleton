#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/CFG.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/MemoryDependenceAnalysis.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/ValueSymbolTable.h"
#include "llvm/IR/Instruction.h"

using namespace std;
using namespace llvm;

class ValueTrace : public PassInfoMixin<ValueTrace> {
private:
public:

    bool runImpl(Module &M);
    void getValueSymbolTable(Function &F);
};

void ValueTrace::getValueSymbolTable(Function &F) {
    const ValueSymbolTable *vst = F.getValueSymbolTable();
    for (auto vb = vst-> begin(); vb != vst->end(); vb++) {
        list<Instruction*> useList;
        Value *val = vb->second;
        const Instruction* inst = dyn_cast<Instruction>(val);
        Type* type = val->getType();
        if (inst != nullptr && type != nullptr && 
            inst->getOpcode() == Instruction::Alloca &&
            (type->getPointerElementType() != nullptr) &&
            (type->getPointerElementType() -> isPointerTy())
        ) {
            errs() << "def: " << *val << "\n";
            // 对当前的 value 构建正序的 userList
            // TODO
        }
    }
}

bool ValueTrace::runImpl(Module &M) {
    for (Module::iterator Mit = M.begin(), Mite = M.end(); Mit != Mite; Mit++) {
        errs() << "Function:" << Mit->getName() << "\n";
        for (llvm::Function::iterator Fit = Mit->begin(), Fite = Mit->end(); Fit != Fite; Fit++) {
            llvm::BasicBlock *bb = dyn_cast<BasicBlock>(Fit);
            errs() << "----BasicBlock:" << bb->getName() << "\n";
            for(auto instIter = bb->begin(); instIter != bb->end(); instIter++){
                if(instIter->getOpcode() == Instruction::Store){
                    outs()<<"==========================================================="<<"\n";
                    outs()<<*instIter<<"\n";
                    outs()<< "right is pointer? " << instIter->getOperand(0)->getType()->isPointerTy() << *instIter->getOperand(0)<<"\n";
                    outs()<< "left is pointer? " << instIter->getOperand(1)->getType()->isPointerTy() << *instIter->getOperand(1)<<"\n";
                    outs()<<"==========================================================="<<"\n";
                }
            }
        }
    }
    return false;
}

namespace {
    struct ValueTraceLegacyPass : public ModulePass {

        static char ID;

        ValueTraceLegacyPass() : ModulePass(ID) {}

        bool runOnModule(Module &M) override {
            errs() << "Hello Value Trace";
            if (skipModule(M))
                return false;
            return Impl.runImpl(M);
        }

    private:
        ValueTrace Impl;
    };
} // namespace

char ValueTraceLegacyPass::ID = 0;

static RegisterPass<ValueTraceLegacyPass> X("valuetrace", /* a command line argument*/
                                            "ValueTracePass", /* name */
                                            false /* Only looks at CFG */,
                                            false /* Analysis Pass */);

static llvm::RegisterStandardPasses Y(llvm::PassManagerBuilder::EP_EarlyAsPossible,
                                      [](const llvm::PassManagerBuilder &Builder,
                                         llvm::legacy::PassManagerBase &PM) {
                                          PM.add(new ValueTraceLegacyPass());
                                      });

// 运行方式：opt -load ./build/skeleton/libValueTracePass.so -valuetrace TinyWebServer/server.ll -enable-new-pm=0
