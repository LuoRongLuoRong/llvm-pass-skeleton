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
#include "llvm/IR/DerivedTypes.h"

// #include "../readjson/read_json.cpp"
#include "../readjson/json_util.cpp"

using namespace llvm;

class Skeleton : public PassInfoMixin<Skeleton>
{
public:
  /* deal with input json file */
  JsonUtil ju;

  /* functional function */
  StringRef getFilepath(Function &F);
  Value *getLine(Instruction *inst, LLVMContext &Ctx);
  int getLineNumber(Instruction *inst, LLVMContext &Ctx);
  Value *getColumn(Instruction *inst, LLVMContext &Ctx);
  int getColumnNumber(Instruction *inst, LLVMContext &Ctx);
  bool isKeyOrStateVar(Instruction *op, LLVMContext &Ctx, std::string filename);
  Variable getVariable(Instruction *svinst, LLVMContext &Ctx, std::string filename);

  /* core function */
  bool runImpl(Function &F);

  /* for different data type */
  FunctionCallee getCalleeInt(Function &F);

  /* for different instructions */
  void log_int_store(std::string filename, StoreInst *inst, BasicBlock &B, Function &F, Variable v);
  void log_int_load(std::string filename, LoadInst *inst, BasicBlock &B, Function &F, Variable v);
};

Variable Skeleton::getVariable(Instruction *svinst, LLVMContext &Ctx, std::string filename)
{
  // 获取 Variable
  int line = getLineNumber(svinst, Ctx);
  int column = getColumnNumber(svinst, Ctx);
  Variable v = ju.getVar(filename, line, column);
  return v;
}

// 检查该变量是否是状态变量或者关键变量
bool Skeleton::isKeyOrStateVar(Instruction *op, LLVMContext &Ctx, std::string filename)
{
  int line = getLineNumber(op, Ctx);
  int column = getColumnNumber(op, Ctx);
  if (line == 0 || column == 0 || !ju.hasVar(filename, line, column))
  {
    return false;
  }
  return ju.hasVar(filename, line, column);
}

FunctionCallee Skeleton::getCalleeInt(Function &F)
{
  // 获取运行时上下文
  LLVMContext &Ctx = F.getContext();
  // 函数参数：
  std::vector<Type *> paramTypesInt = {
      Type::getInt8PtrTy(Ctx), // filename
      Type::getInt32Ty(Ctx),   // line
      Type::getInt8PtrTy(Ctx), // name
      Type::getInt32Ty(Ctx),   // type
      Type::getInt32Ty(Ctx),   // state
      Type::getInt32Ty(Ctx)    // old_state
  };
  // 函数返回值：
  Type *retType = Type::getVoidTy(Ctx);
  // 函数类型：
  FunctionType *logFuncIntType = FunctionType::get(retType, paramTypesInt, false);
  // 根据函数的名字获取该函数：
  FunctionCallee logFunc = F.getParent()->getOrInsertFunction("logint", logFuncIntType);

  return logFunc;
}

// 返回函数所在文件的路径 filepath
StringRef Skeleton::getFilepath(Function &F)
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
    return F.getName();
  }
  return DIF->getFilename(); // 在中间表示中，filename 其实是文件的相对路径。
}

// IR 指令在源代码中的行号
int Skeleton::getLineNumber(Instruction *inst, LLVMContext &Ctx)
{
  if (DILocation *DILoc = inst->getDebugLoc())
  {
    return DILoc->getLine();
  }
  return 0;
}

// IR 指令在源代码中的行号的 Value
Value *Skeleton::getLine(Instruction *inst, LLVMContext &Ctx)
{
  return ConstantInt::get(Type::getInt32Ty(Ctx), getLineNumber(inst, Ctx));
}

// IR 指令在源代码中的 Column
int Skeleton::getColumnNumber(Instruction *inst, LLVMContext &Ctx)
{
  if (DILocation *DILoc = inst->getDebugLoc())
  {
    return DILoc->getColumn(); // here wrong
  }
  return 0;
}

// IR 指令在源代码中的 Column 的 Value
Value *Skeleton::getColumn(Instruction *inst, LLVMContext &Ctx)
{
  return ConstantInt::get(Type::getInt32Ty(Ctx), getColumnNumber(inst, Ctx));
}

// 非侵入式插桩
void Skeleton::log_int_load(std::string filename, LoadInst *inst, BasicBlock &B, Function &F, Variable v)
{
  // Get the function to call from our runtime library.
  LLVMContext &Ctx = F.getContext();
  // 创建运行时 Callee
  FunctionCallee logFunc = getCalleeInt(F);
  // 获取 IR 的插入点
  IRBuilder<> builder(inst);
  builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

  // rtlib 函数: char* filename, int line, char* name, int type, int state, int old_state
  Value *argfilename = builder.CreateGlobalString(filename);
  Value *argline = getLine(inst, Ctx);               // line number
  Value *argname = builder.CreateGlobalString(v.sv); // name
  Value *argtype = ConstantInt::get(Type::getInt32Ty(Ctx), v.type);
  Value *argstate = dyn_cast_or_null<Value>(inst);    // state
  Value *argoldstate = dyn_cast_or_null<Value>(inst); // old state

  errs() << filename << ", " << v.line << ", " << v.column << ", " << argname << "\n";
  Value *args[] = {argfilename, argline, argname, argtype, argstate, argoldstate};
  builder.CreateCall(logFunc, args);
}

void Skeleton::log_int_store(std::string filename, StoreInst *inst, BasicBlock &B, Function &F, Variable v)
{
  // Get the function to call from our runtime library.
  LLVMContext &Ctx = F.getContext();
  // 创建运行时 Callee
  FunctionCallee logFunc = getCalleeInt(F);

  IRBuilder<> builder(inst);
  builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

  // get right: name
  Value *arg2 = inst->getOperand(1);

  // errs() << "StoreInst R: " << *arg2 << ": [" << arg2->getName() << "]\n";
  Value *argfilename = builder.CreateGlobalString(filename);
  Value *argname = builder.CreateGlobalString(v.sv);
  Value *argtype = ConstantInt::get(Type::getInt32Ty(Ctx), v.type);
  Value *argstate = inst->getOperand(0); // state
  Value *argline = getLine(inst, Ctx);   //

  Value *argoldstate;
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
    break;
  } while (true);

  if (isInitial)
  {
    // 未被初始化，赋值为 0
    argoldstate = ConstantInt::get(Type::getInt32Ty(Ctx), 0);
  }
  else
  {
    LoadInst *loadInst = new LoadInst(arg2->getType(), arg2, arg2->getName(), inst);
    argoldstate = dyn_cast_or_null<Value>(loadInst);
  }

  errs() << filename << ", " << v.line << ", " << v.column << ", " << argname << "\n";
  Value *args[] = {argfilename, argline, argname, argtype, argstate, argoldstate};
  builder.CreateCall(logFunc, args);
}

// 将 LLVM Pass 的 runOnFunction 解构到这个函数中，
// 目的是减少成员变量的无效赋值，以提高项目效率。
bool Skeleton::runImpl(Function &F)
{
  // 检查文件名是否在关注的 filepaths 中
  std::string filename = getFilepath(F).str();

  ju.save();
  if (!ju.inFilepaths(filename))
  {
    return false;
  }

  LLVMContext &Ctx = F.getContext();

  for (auto &B : F)
  {
    for (auto &I : B)
    {
      // 目前，对于 IR Instruction，我们重点关注 Store 和 Load
      if (!isa<StoreInst>(&I) && !isa<LoadInst>(&I))
      {
        continue;
      }

      if (auto *op = dyn_cast<LoadInst>(&I))
      {
        Value *arg1 = op->getOperand(0); 
        Variable v;
        // 如果不是关键变量或者状态变量，则 continue
        if (auto *svinst = dyn_cast<GetElementPtrInst>(arg1))
        {
          if (!isKeyOrStateVar(svinst, Ctx, filename))
          {
            continue;
          }
          v = getVariable(svinst, Ctx, filename);
        }
        else if (auto *svinst = dyn_cast<LoadInst>(arg1))
        {
          if (!isKeyOrStateVar(svinst, Ctx, filename))
          {
            continue;
          }
          v = getVariable(svinst, Ctx, filename);
        }
        else
        {
          continue;
        }

        log_int_load(filename, op, B, F, v);
      }

      else if (auto *op = dyn_cast<StoreInst>(&I))
      {
        // get left: value
        Value *arg1 = op->getOperand(0); // %4 = xxx
        Value *arg2 = op->getOperand(1);

        Variable v;

        // 判断变量是不是 keyOrStateValue
        if (auto *svinst = dyn_cast<GetElementPtrInst>(arg2))
        {
          if (!isKeyOrStateVar(svinst, Ctx, filename))
          {
            continue;
          }
          v = getVariable(svinst, Ctx, filename);
        }
        else if (auto *svinst = dyn_cast<LoadInst>(arg2))
        {
          if (!isKeyOrStateVar(svinst, Ctx, filename))
          {
            continue;
          }
          v = getVariable(svinst, Ctx, filename);
        }
        else
        {
          continue;
        }

        // errs() << "【" << I << "】"
        //        << "\n";
        // errs() << *(arg1->getType()) << '\n';
        // errs() << "StoreInst L: " << *arg1 << ": [" << arg1->getName() << "]\n";
        Type *value_ir_type = arg1->getType();
        std::cout << value_ir_type->isIntegerTy() << " " << v.sv << v.line << "震惊！ \n";
        if (value_ir_type->isIntegerTy())
        {

          log_int_store(filename, op, B, F, v);
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
          // errs() << "FloatType" << "\n";
        }
        else if (value_ir_type->isDoubleTy())
        {
          // errs() << "DoubleType" << "\n";
        }
      }
    }
  }
  return false;
}

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
