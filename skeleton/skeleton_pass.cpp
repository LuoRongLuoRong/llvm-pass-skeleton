#include "include/skeleton.h"
// #include "src/skeleton.cpp"

/********************************  pass 的主体  *******************************/

namespace
{
  // 继承自 FunctionPass
  struct SkeletonPass : public FunctionPass
  {
    static char ID;
    SkeletonPass() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F)
    {
      // return false;
      if (F.isIntrinsic())
      {
        return false;
      }
      return Impl.runImpl(F);
    }

  private:
    Skeleton Impl;
  };
}

/********************************  pass 的注册  *******************************/
// 注册 pass 并且自启动
char SkeletonPass::ID = 0;

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerSkeletonPass(const PassManagerBuilder &, legacy::PassManagerBase &PM)
{
  PM.add(new SkeletonPass());
}
static RegisterStandardPasses RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerSkeletonPass);


