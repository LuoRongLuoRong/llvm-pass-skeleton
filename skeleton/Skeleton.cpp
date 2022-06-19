#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/ValueSymbolTable.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/DerivedTypes.h"

#include "../readjson/read_json.cpp"

#define MAX_INT (((unsigned int)(-1)) >> 1)

using namespace llvm;

namespace
{

  // 返回函数所在是文件的路径+文件的名称
  StringRef getSourceName(Function &F)
  {
    DISubprogram *DI = F.getSubprogram();
    if (!DI)
    {
      // errs() << "Function " << F.getName() << " does not have a subprogram\n";
      return F.getName();
    }
    DIFile *DIF = DI->getFile();
    if (!DIF)
    {
      // errs() << "Function " << F.getName() << " does not have a file\n";
    }
    return DIF->getFilename();
  }

  // IR 指令在源代码中的行号
  Value *getLine(Instruction *inst, LLVMContext &Ctx)
  {
    if (DILocation *DILoc = inst->getDebugLoc())
    {
      return ConstantInt::get(Type::getInt32Ty(Ctx), DILoc->getLine());
    }
    return ConstantInt::get(Type::getInt32Ty(Ctx), 0);
  }

  FunctionCallee getLogFunc(LLVMContext &Ctx, Function &F, Type *stateType, std::string rtFuncName)
  {
    // 函数参数：
    std::vector<Type *> paramTypes = {
        Type::getInt8PtrTy(Ctx), // filename
        Type::getInt32Ty(Ctx),   // line
        Type::getInt8PtrTy(Ctx), // name
        Type::getInt32Ty(Ctx),   // type
        stateType,               // state
        stateType                // old_state
    };
    // 函数返回值：
    Type *retType = Type::getVoidTy(Ctx);
    // 函数类型：
    FunctionType *logFuncType = FunctionType::get(retType, paramTypes, false);
    // 根据函数的名字获取该函数：
    FunctionCallee logFunc = F.getParent()->getOrInsertFunction(rtFuncName, logFuncType);
    return logFunc;
  }

  FunctionCallee getLogFunc(LLVMContext &Ctx, Function &F, int dataType)
  {
    switch (dataType)
    {
    case 1: // bool
      return getLogFunc(Ctx, F, Type::getInt8Ty(Ctx), "logbool");
    case 2: // char
      return getLogFunc(Ctx, F, Type::getInt8Ty(Ctx), "logchar");
    case 3: // string
      return getLogFunc(Ctx, F, Type::getInt8PtrTy(Ctx), "logstring");
    case 4: // float
      return getLogFunc(Ctx, F, Type::getInt8Ty(Ctx), "logbool");
    case 5: // double
      return getLogFunc(Ctx, F, Type::getInt8Ty(Ctx), "logchar");
    default: // 0: int
      return getLogFunc(Ctx, F, Type::getInt32Ty(Ctx), "logint");
    }
  }

  /****************** instrumented functions *******************/

  void log_int_load(std::string filename, std::string varname, int type, LoadInst *inst,
                    BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx)
  {
    IRBuilder<> builder(inst);
    builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

    Value *argfilename = builder.CreateGlobalString(filename);
    Value *argstr = builder.CreateGlobalString(varname);
    Value *argtype = ConstantInt::get(Type::getInt32Ty(Ctx), type);
    Value *argvalue = dyn_cast_or_null<Value>(inst); // state
    Value *argline = getLine(inst, Ctx);             //
    Value *argold = dyn_cast_or_null<Value>(inst);   // old state

    Value *args[] = {argfilename, argline, argstr, argtype, argvalue, argold};
    // instrumentation
    builder.CreateCall(logFunc, args);
  }

  void log_int_store(std::string filename, std::string varname, int type, StoreInst *inst, BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx)
  {
    IRBuilder<> builder(inst);
    builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

    // get right: name
    Value *arg2 = inst->getOperand(1);
    // errs() << "StoreInst R: " << *arg2 << ": [" << arg2->getName() << "]\n";
    Value *argfilename = builder.CreateGlobalString(filename);
    Value *argstr = builder.CreateGlobalString(varname);
    Value *argtype = ConstantInt::get(Type::getInt32Ty(Ctx), type);
    Value *argi = inst->getOperand(0);   // state
    Value *argline = getLine(inst, Ctx); //

    Value *argold;
    // errs() << "   arg2 的 Use 的数目" << arg2->getNumUses() << "\n";

    Value::use_iterator U = arg2->use_begin();

    bool isInitial = true;
    do
    {
      // 1. 全局变量，直接 load
      if (!isa<AllocaInst>(arg2))
      {
        isInitial = false;
        break;
      }
      // 2. 局部变量，检测是否曾被赋值过。如果前面全是 load 则认为是初始化。
      // 循环终止的条件：（1）遇到本 instruction【说明是初始化】；（2）遇到结尾。

      // 只能倒序遍历，所以不得不出此下策
      while (U->getUser() != inst)
      {
        ++U;
      }
      ++U; // 跳过 inst

      while (U != arg2->use_end())
      {
        // 之前已经被使用过了
        if (isa<StoreInst>(U->getUser()))
        {
          isInitial = false;
          // errs() << "之前已经被" << inst << "使用过了" << U->getUser() << " >>> Use：" << *(U->getUser()) << "\n";

          break;
        }
        ++U;
        // errs() << "  U->getUser() == inst: " << (U->getUser() == inst) << "\n";
      }
    } while (false);

    if (isInitial)
    {
      // 未被初始化
      argold = ConstantInt::get(Type::getInt32Ty(Ctx), MAX_INT);
    }
    else
    {
      LoadInst *loadInst = new LoadInst(arg2->getType(), arg2, arg2->getName(), inst);
      argold = dyn_cast_or_null<Value>(loadInst);
    }

    Value *args[] = {argfilename, argline, argstr, argtype, argi, argold}; //
    builder.CreateCall(logFunc, args);
  }

  // 继承自 FunctionPass
  struct SkeletonPass : public FunctionPass
  {

    static char ID;
    SkeletonPass() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F)
    {
      jsonutil ju;
      // 判断 filename 是否包含在 json 文件中
      std::string filename = getSourceName(F).str();
      std::string jsonPath = "SVsite.json";

      std::map<std::string, std::map<std::string, std::vector<int>>> mapFileVariable = ju.readSVsiteJson(jsonPath);

      // 该文件不值得继续探索
      if (!ju.hasFile(mapFileVariable, filename))
      {
        return false;
      }

      // Get the function to call from our runtime library.
      LLVMContext &Ctx = F.getContext();

      FunctionCallee logFuncInt = getLogFunc(Ctx, F, 0);
      FunctionCallee logFuncBool = getLogFunc(Ctx, F, 1);
      FunctionCallee logFuncChar = getLogFunc(Ctx, F, 2);
      FunctionCallee logFuncString = getLogFunc(Ctx, F, 3);
      FunctionCallee logFuncFloat = getLogFunc(Ctx, F, 4);
      FunctionCallee logFuncDouble = getLogFunc(Ctx, F, 5);

      for (auto &B : F)
      {
        for (auto &I : B)
        {
          if (!isa<StoreInst>(&I) && !isa<LoadInst>(&I))
          {
            continue;
          }

          if (auto *op = dyn_cast<LoadInst>(&I))
          {
            Value *arg2 = op->getOperand(0);

            // 检查该变量是否存在
            if (!ju.hasVariable(mapFileVariable, filename, arg2->getName().str()))
            {
              continue;
            }

            std::string varname = ju.getVarname(mapFileVariable, filename, arg2->getName().str());
            int type = ju.mapSvType[varname];
            
            log_int_load(filename, varname, type, op, B, logFuncInt, Ctx);
          }

          if (auto *op = dyn_cast<StoreInst>(&I))
          {
            // get left: value
            Value *arg1 = op->getOperand(0); // %4 = xxx
            Value *arg2 = op->getOperand(1);

            // 检查该变量是否存在
            if (!ju.hasVariable(mapFileVariable, filename, arg2->getName().str()))
            {
              continue;
            }

            std::string varname = ju.getVarname(mapFileVariable, filename, arg2->getName().str());
            int type = ju.mapSvType[varname];

            Type *value_ir_type = arg1->getType();
            if (value_ir_type->isIntegerTy())
            {
              // log_int_store(filename, varname, type, op, B, logFuncInt, Ctx);

              unsigned int_bit_width = value_ir_type->getIntegerBitWidth();
              errs() << "IntegerType" << int_bit_width << "\n";
              if (int_bit_width == 1)
              {
                log_int_store(filename, varname, type, op, B, logFuncBool, Ctx);
              }
              else if (int_bit_width == 8)
              {
                // 1. bool
                if (auto constant_int = dyn_cast<ConstantInt>(arg1))
                {
                  int value = constant_int->getSExtValue();
                  if (value == 0 | value == 1)
                  {
                    log_int_store(filename, varname, type, op, B, logFuncBool, Ctx);
                  }
                  else
                  {
                    log_int_store(filename, varname, type, op, B, logFuncChar, Ctx);
                  }
                }
                else
                {
                  // errs() << "ERROR: i8 无 int 值。\n";
                  log_int_store(filename, varname, type, op, B, logFuncChar, Ctx);
                }
              }
              else if (int_bit_width == 32)
              {
                log_int_store(filename, varname, type, op, B, logFuncInt, Ctx);
              }
              else
              {
                errs() << "ERROR: integer 无归属。\n";
                log_int_store(filename, varname, type, op, B, logFuncInt, Ctx);
              }
            }
            else if (value_ir_type->isPointerTy())
            {
              PointerType *pt = dyn_cast<PointerType>(value_ir_type);
              //              // errs() << "!!! PointerType: "
              //                << pt->isAggregateType() << "; "
              //                << "\n";
              // i8*

              // log_line_var_string(op, B, logFuncString, Ctx);
            }
            else if (value_ir_type->isFloatTy())
            {

            }
            else if (value_ir_type->isDoubleTy())
            {

            }
          }
        }
      }

      // 对源码进行了修改返回 true
      return true;
    }
  };
};

// 注册 pass 并且自启动
char SkeletonPass::ID = 0;

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerSkeletonPass(const PassManagerBuilder &, legacy::PassManagerBase &PM)
{
  PM.add(new SkeletonPass());
}
static RegisterStandardPasses RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerSkeletonPass);
