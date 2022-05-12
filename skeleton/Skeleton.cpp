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

#include "readjson/read_json.cpp"

using namespace llvm;

namespace
{
  // 继承自 FunctionPass
  struct SkeletonPass : public FunctionPass
  {
    // 一些自定义的全局变量
    // ...

    static char ID;
    SkeletonPass() : FunctionPass(ID) {}

    // 返回函数所在文件的路径
    StringRef getFunctionSourceName(Function &F)
    {
      DISubprogram *DI = F.getSubprogram();
      if (!DI)
      {
        errs() << "Function " << F.getName() << " does not have a subprogram\n";
        return F.getName();
      }
      return DI->getName();
    }

    // 返回函数所在是文件的路径
    StringRef getSourceName(Function &F)
    {
      DISubprogram *DI = F.getSubprogram();
      if (!DI)
      {
        errs() << "Function " << F.getName() << " does not have a subprogram\n";
        return F.getName();
      }
      DIFile *DIF = DI->getFile();
      if (!DIF)
      {
        errs() << "Function " << F.getName() << " does not have a file\n";
      }
      return DIF->getFilename();
    }

    void printFunctionName(Function &F)
    {
      DISubprogram *DI = F.getSubprogram();
      if (!DI)
      {
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

    // 获取的是 IR 中的临时变量而非 source code 中的 local variables
    void getLocalVariables(Function &F)
    {
      // not test yet
      ValueSymbolTable *vst = F.getValueSymbolTable();
      errs() << (*vst).size() << "\n.";

      for (ValueSymbolTable::iterator VI = vst->begin(), VE = vst->end(); VI != VE; ++VI)
      {
        Value *V = VI->getValue();
        if (!isa<GlobalValue>(V) || cast<GlobalValue>(V)->hasLocalLinkage())
        {
          if (!V->getName().startswith("llvm.dbg"))
            // Set name to "", removing from symbol table!
            V->setName("");
        }
      }
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

    virtual bool runOnFunction(Function &F)
    {
      jsonutil ju;
      // 判断 filename 是否包含在 json 文件中
      std::string filename = getSourceName(F).str();
//      std::string jsonPath = "file_variable_line.json";
      std::string jsonPath = "SVsite.json";

      std::map<std::string, std::map<std::string, std::vector<int>>> mapFileVariable = ju.readSVsiteJson(jsonPath);

      // 该文件不值得继续探索
      if (!ju.hasFile(mapFileVariable, filename))
      {
        return false;
      }

      // Get the function to call from our runtime library.
      LLVMContext &Ctx = F.getContext();
      // 函数参数：
      std::vector<Type *> paramTypesInt = {
          Type::getInt8PtrTy(Ctx), // filename
          Type::getInt32Ty(Ctx),   // line
          Type::getInt8PtrTy(Ctx), // name
          Type::getInt32Ty(Ctx),   // state
          Type::getInt32Ty(Ctx)    // old_state
      };
      std::vector<Type *> paramTypesCharBool = {
          Type::getInt32Ty(Ctx),   // line
          Type::getInt8PtrTy(Ctx), // name
          Type::getInt8Ty(Ctx)     // state
      };
      std::vector<Type *> paramTypesString = {
          Type::getInt32Ty(Ctx),   // line
          Type::getInt8PtrTy(Ctx), // name
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
      FunctionCallee logFuncInt = F.getParent()->getOrInsertFunction("logint", logFuncIntType);
      FunctionCallee logFuncBool = F.getParent()->getOrInsertFunction("loglinevarbool", logFuncIntType);
      FunctionCallee logFuncChar = F.getParent()->getOrInsertFunction("loglinevarchar", logFuncIntType);
      FunctionCallee logFuncString = F.getParent()->getOrInsertFunction("loglinevarstring", logFuncIntType);

      // errs() << "\n\n" << "Function: " << *(logFunc.getCallee()) << '\n';
      // errs() << "" << "FUNC: " << F.getName() << '\n';
      //       printFunctionName(F);
      // getMDNodes(F);
      // getFunSubprogram(F);

      for (auto &B : F)
      {
        for (auto &I : B)
        {
          if (auto *op = dyn_cast<StoreInst>(&I))
          {
            // get left: value
            Value *arg1 = op->getOperand(0); // %4 = xxx
            Value *arg2 = op->getOperand(1);

            errs() << "变量" << arg2->getName().str();
            // 检查该变量是否存在
            if (!ju.hasVariable(mapFileVariable, filename, arg2->getName().str()))
            {
              continue;
            }

            errs() << "YES变量" << arg2->getName().str();

            errs() << "【" << I << "】"
                   << "\n";
            errs() << *(arg1->getType()) << '\n';
            errs() << "StoreInst L: " << *arg1 << ": [" << arg1->getName() << "]\n";
            Type *value_ir_type = arg1->getType();
            if (value_ir_type->isIntegerTy())
            {
              log_int(filename, op, B, logFuncInt, Ctx);

              if (true)
                continue;
              unsigned int_bit_width = value_ir_type->getIntegerBitWidth();
              errs() << "IntegerType" << int_bit_width << "\n";
              if (int_bit_width == 1)
              {
                log_line_var_bool(op, B, logFuncBool, Ctx);
              }
              else if (int_bit_width == 8)
              {
                // 1. bool
                if (auto constant_int = dyn_cast<ConstantInt>(arg1))
                {
                  int value = constant_int->getSExtValue();
                  if (value == 0 | value == 1)
                  {
                    log_line_var_bool(op, B, logFuncBool, Ctx);
                  }
                  else
                  {
                    log_line_var_char(op, B, logFuncChar, Ctx);
                  }
                }
                else
                {
                  // TODO: errs() << "ERROR: i8 无 int 值。\n";
                  // 此处将 bool 视为 char 的特殊情况。因为在未读取值的前提下，value 是未知的。
                  log_line_var_char(op, B, logFuncChar, Ctx);
                }
              }
              else if (int_bit_width == 32)
              {
                log_line_var_int(op, B, logFuncInt, Ctx);
              }
              else
              {
                errs() << "ERROR: integer 无归属。\n";
              }
            }
            else if (value_ir_type->isPointerTy())
            {
              PointerType *pt = dyn_cast<PointerType>(value_ir_type);
              //              errs() << "!!! PointerType: "
              //                << pt->isAggregateType() << "; "
              //                << "\n";
              // i8*

              // log_line_var_string(op, B, logFuncString, Ctx);
            }
            else if (value_ir_type->isFloatTy())
            {
              errs() << "FloatType"
                     << "\n";
            }
            else if (value_ir_type->isDoubleTy())
            {
              errs() << "DoubleType"
                     << "\n";
            }
          }
        }
      }
      // 对源码进行了修改返回 true
      return true;
    }

    /****************** instrumented functions *******************/

    void log_int(std::string filename, StoreInst *inst, BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx)
    {
      IRBuilder<> builder(inst);
      builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

      // get right: name
      Value *arg2 = inst->getOperand(1);
      errs() << "StoreInst R: " << *arg2 << ": [" << arg2->getName() << "]\n";
      Value *argfilename = builder.CreateGlobalString(filename);
      Value *argstr = builder.CreateGlobalString(arg2->getName());
      Value *argi = inst->getOperand(0);   // state
      Value *argline = getLine(inst, Ctx); //

      Value *argold;
      errs() << "   arg2 的 Use 的数目" << arg2->getNumUses() << "\n";

      Value::use_iterator U = arg2->use_begin();

      bool isInitial = true;
      do {
        // 1. 全局变量，直接 load
        if (!isa<AllocaInst>(arg2)) {
          isInitial = false;
          break;
        }
        // 2. 局部变量，检测是否曾被赋值过。如果前面全是 load 则认为是初始化。
        // 循环终止的条件：（1）遇到本 instruction【说明是初始化】；（2）遇到结尾。

        // 只能倒序遍历，所以不得不出此下策
        while (U->getUser() != inst) {
          ++U;
        }
        ++U; // 跳过 inst

        while (U != arg2->use_end())
        {
          // 之前已经被使用过了
          if (isa<StoreInst>(U->getUser()))
          {
            isInitial = false;
            errs() << "之前已经被" << inst << "使用过了" << U->getUser() << " >>> Use：" << *(U->getUser()) << "\n";

            break;
          }
          ++U;
          errs() << "  U->getUser() == inst: " << (U->getUser() == inst) << "\n";
        }
        break;
      } while (true);


      for (U = arg2->use_begin(); U != arg2->use_end(); ++U)
      {
        errs() << " >>> Use：" << *(U->getUser()) << "\n";
      }

      if (isInitial)
      {
        // 未被初始化
        argold = ConstantInt::get(Type::getInt32Ty(Ctx), -1);
      }
      else
      {
        LoadInst *loadInst = new LoadInst(arg2->getType(), arg2, arg2->getName(), inst);
        argold = dyn_cast_or_null<Value>(loadInst);
      }

      Value *args[] = {argfilename, argline, argstr, argi, argold}; //
      builder.CreateCall(logFunc, args);
    }


    void log_line_var_int_load(LoadInst *inst, BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx)
    {
      IRBuilder<> builder(inst);
      builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

      // get right: name
      Value *arg2 = inst->getPointerOperand();
      errs() << "StoreInst R: " << *arg2 << ": [" << arg2->getName() << "]\n";

      Value *argstr = builder.CreateGlobalString(arg2->getName());
      Value *argline = getLine(inst, Ctx);

      Value *args[] = {argline, argstr, dyn_cast_or_null<Value>(inst), dyn_cast_or_null<Value>(inst)}; //
      builder.CreateCall(logFunc, args);
    }

    void log_line_var_int(StoreInst *inst, BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx)
    {
      IRBuilder<> builder(inst);
      builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

      // get right: name
      Value *arg2 = inst->getOperand(1);
      errs() << "StoreInst R: " << *arg2 << ": [" << arg2->getName() << "]\n";
      errs() << *arg2 << "\n";

      Value *argstr = builder.CreateGlobalString(arg2->getName());
      Value *argi = inst->getOperand(0);
      if (auto *callins = dyn_cast<CallInst>(argi))
      {
        Value *v = callins->getCalledOperandUse();
        errs() << "  CALL: " << *v << "\n";
      }
      Value *argline = getLine(inst, Ctx);
      // store i32 0, i32* %state_int, align 4, !dbg !859

      // 获取指令运行之前值
      //      Value* argold;
      //      if (auto alloca = dyn_cast<AllocaInst>(arg2)) {
      //        for (auto user: alloca->users()) {
      //          // %4 = load i32, i32* %3, align 4
      //          // store i32 1, i32* %3, align 4
      //          errs() << " User:" << *user << "\n";
      //          if (auto store = dyn_cast<StoreInst>(user)) {
      //            if (auto constant_int = dyn_cast<ConstantInt>(store->getOperand(0))) {
      //              int number = constant_int->getSExtValue();
      //              errs() << number << "***\n";
      //              argold = constant_int;
      //              break;
      //            } else {  // 外界输入，动态获取
      //              errs() << "外界输入，动态获取\n.";
      //              argold  = ConstantInt::get(Type::getInt32Ty(Ctx), 555);
      //            }
      //          } else {
      //            argold = ConstantInt::get(Type::getInt32Ty(Ctx), 9999);  // 初始化
      //          }
      //        }
      //      } else {
      //        argold = ConstantInt::get(Type::getInt32Ty(Ctx), 9999999);  // 初始化
      //      }

      Value *argold;
      if (arg2->use_empty())
      {
        argold = ConstantInt::get(Type::getInt32Ty(Ctx), 999);
      }
      else
      {
        // LoadInst (Type *Ty, Value *Ptr, const Twine &NameStr, Instruction *InsertBefore)
        LoadInst *loadInst = new LoadInst(arg2->getType(), arg2, arg2->getName(), inst);
        argold = dyn_cast_or_null<Value>(loadInst);
        //        argold = dyn_cast_or_null<Value>(arg2);

        //        builder.CreateLoad(arg2, arg2->getName());
      }

      Value *args[] = {argline, argstr, argi, argold}; //
      builder.CreateCall(logFunc, args);
    }

    void log_line_var_bool(StoreInst *inst, BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx)
    {
      IRBuilder<> builder(inst);
      builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

      // get right: name
      Value *arg2 = inst->getOperand(1);
      errs() << "StoreInst R: " << *arg2 << ": [" << arg2->getName() << "]\n";

      Value *argstr = builder.CreateGlobalString(arg2->getName());
      Value *argi = inst->getOperand(0);
      Value *argline = getLine(inst, Ctx);

      Value *args[] = {argline, argstr, argi}; //
      builder.CreateCall(logFunc, args);
    }

    void log_line_var_char(StoreInst *inst, BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx)
    {
      IRBuilder<> builder(inst);
      builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

      // get right: name
      Value *arg2 = inst->getOperand(1);
      errs() << "StoreInst R: " << *arg2 << ": [" << arg2->getName() << "]\n";

      Value *argstr = builder.CreateGlobalString(arg2->getName());
      Value *argi = inst->getOperand(0);
      Value *argline = getLine(inst, Ctx);

      Value *args[] = {argline, argstr, argi}; //
      builder.CreateCall(logFunc, args);
    }

    /**
     * 没有完成的函数
     * @param inst
     * @param B
     * @param logFunc
     * @param Ctx
     */
    void log_line_var_string(StoreInst *inst, BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx)
    {
      IRBuilder<> builder(inst);
      builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

      Value *arg1 = inst->getOperand(0);
      Value *arg2 = inst->getOperand(1); // 可能并非变量的真实名称。
      errs() << "StoreInst R: " << *arg2 << ": [" << arg2->getName() << "]\n";

      Value *argstr = builder.CreateGlobalString(arg2->getName()); // name
      Value *argline = getLine(inst, Ctx);                         // line

      // string value
      // store i8* %0, i8** %result.ptr, align 8
      Value *argi; // TODO!

      Value *args[] = {argline, argstr, argi}; //
      builder.CreateCall(logFunc, args);
    }

    void log_line_var(StoreInst *inst, BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx)
    {
      IRBuilder<> builder(inst);
      builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

      // get right: name
      Value *arg2 = inst->getOperand(1);
      errs() << "StoreInst R: " << *arg2 << ": [" << arg2->getName() << "]\n";

      Value *argstr = builder.CreateGlobalString(arg2->getName());
      Value *argline = getLine(inst, Ctx);

      Value *argi = inst->getOperand(0);

      Value *args[] = {argline, argstr, argi}; //
      builder.CreateCall(logFunc, args);
    }

    void log_line_var_old(StoreInst *inst, BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx)
    {
      IRBuilder<> builder(inst);
      builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

      // get right: name
      Value *arg2 = inst->getOperand(1); // %2 = xxx
      Instruction *arg2ins = dyn_cast<Instruction>(arg2);
      while (arg2ins->getOpcode() != Instruction::Alloca)
      {
        // errs() << "        R: " << *arg2ins << "\n";
        arg2ins = dyn_cast<Instruction>(arg2ins->getOperand(1));
      }
      errs() << "StoreInst R: " << *arg2ins << ": [" << arg2ins->getName() << "]\n";
      Value *argstr = builder.CreateGlobalString(arg2ins->getName());

      Value *argi = inst->getOperand(0); // left

      // argline

      Value *argline = getLine(inst, Ctx);

      Value *args[] = {argline, argstr, argi}; //
      builder.CreateCall(logFunc, args);
      if (auto constant_int = dyn_cast<ConstantInt>(argi))
      {
        errs() << "store inst has instance number" << constant_int << "\n";
      }
      else
      {
        errs() << "store inst has no instance number" << "\n";
      }
    }

    void logvar(StoreInst *inst, BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx)
    {
      IRBuilder<> builder(inst);
      builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

      // get right: name
      Value *arg2 = inst->getOperand(1); // %2 = xxx
      Instruction *arg2ins = dyn_cast<Instruction>(arg2);
      while (arg2ins->getOpcode() != Instruction::Alloca)
      {
        // errs() << "        R: " << *arg2ins << "\n";
        arg2ins = dyn_cast<Instruction>(arg2ins->getOperand(1));
      }
      errs() << "StoreInst R: " << *arg2ins << ": [" << arg2ins->getName() << "]\n";
      Value *argstr = builder.CreateGlobalString(arg2ins->getName());

      Value *argi = inst->getOperand(0); // left
      Value *args[] = {argi, argstr};    //
      builder.CreateCall(logFunc, args);
      if (auto constant_int = dyn_cast<ConstantInt>(argi))
      {
        errs() << "store inst has instance number" << constant_int << "\n";
      }
      else
      {
        errs() << "store inst has no instance number"
               << "\n";
      }
    }

    // int and float function test
    void logif(StoreInst *inst, BasicBlock &B, FunctionCallee logFunc, LLVMContext &Ctx)
    {
      IRBuilder<> builder(inst);
      builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

      Value *argi = inst->getOperand(0); // left
      if (auto constant_int = dyn_cast<ConstantInt>(argi))
      {
        Value *argf = ConstantFP::get(Type::getFloatTy(Ctx), 1);
        Value *args[] = {argi, argf}; //
        builder.CreateCall(logFunc, args);
      }
      else
      {
        errs() << "store inst has no instance number"
               << "\n";
      }
    }

    void boComputed(BinaryOperator *op, BasicBlock &B, FunctionCallee logFunc)
    {
      IRBuilder<> builder(op);
      builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

      Value *args[] = {op};
      builder.CreateCall(logFunc, args);
    }
  };
}

// 注册 pass 并且自启动
char SkeletonPass::ID = 0;

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerSkeletonPass(const PassManagerBuilder &, legacy::PassManagerBase &PM)
{
  PM.add(new SkeletonPass());
}
static RegisterStandardPasses RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerSkeletonPass);
