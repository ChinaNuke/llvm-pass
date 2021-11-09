//===- Hello.cpp - Example code from "Writing an LLVM Pass" ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements two versions of the LLVM "Hello World" pass described
// in docs/WritingAnLLVMPass.html
//
//===----------------------------------------------------------------------===//

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/ToolOutputFile.h>

#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Utils.h>

#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Pass.h>
#include <llvm/Support/raw_ostream.h>

#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/Bitcode/BitcodeWriter.h>

#ifdef DEBUG
#define LOG_DEBUG(msg)                                                         \
  do {                                                                         \
    errs() << "[DEBUG] " << msg << "\n";                                       \
  } while (0)
#else
#define LOG_DEBUG(msg)                                                         \
  do {                                                                         \
  } while (0)
#endif

using namespace llvm;
static ManagedStatic<LLVMContext> GlobalContext;
static LLVMContext &getGlobalContext() { return *GlobalContext; }
/* In LLVM 5.0, when  -O0 passed to clang , the functions generated with clang
 * will have optnone attribute which would lead to some transform passes
 * disabled, like mem2reg.
 */
struct EnableFunctionOptPass : public FunctionPass {
  static char ID;
  EnableFunctionOptPass() : FunctionPass(ID) {}
  bool runOnFunction(Function &F) override {
    if (F.hasFnAttribute(Attribute::OptimizeNone)) {
      F.removeFnAttr(Attribute::OptimizeNone);
    }
    return true;
  }
};

char EnableFunctionOptPass::ID = 0;

///!TODO TO BE COMPLETED BY YOU FOR ASSIGNMENT 2
/// Updated 11/10/2017 by fargo: make all functions
/// processed by mem2reg before this pass.
struct FuncPtrPass : public ModulePass {
  static char ID; // Pass identification, replacement for typeid
  FuncPtrPass() : ModulePass(ID) {}
  std::map<unsigned int, std::vector<std::string>> results;
  std::vector<std::string> tempFuncNames;

private:
  void addFuncName(const std::string funcName) {
    tempFuncNames.push_back(funcName);
  }

  void saveResultAndClearTemp(const unsigned int lineno) {
    auto result = results.find(lineno);
    if (result == results.end()) {
      // 看起来 pair 是把作为参数的 vector 又复制了一份，
      // 所以后面将 vector 清空不会影响输出
      results.insert(std::pair<unsigned int, std::vector<std::string>>(
          lineno, tempFuncNames));
    } else {
      LOG_DEBUG("I think this will never appear!");
      // auto &funcNames = result.second;
      // funcNames.insert(funcNames.end(), tempFuncNames.begin(),
      // tempFuncNames.end());
    }
    tempFuncNames.clear();
  }

  void printResults() const {
    for (const auto &out : results) {
      errs() << out.first << ": ";
      const auto &funcNames = out.second;
      for (auto iter = funcNames.begin(); iter != funcNames.end(); iter++) {
        if (iter != funcNames.begin()) {
          errs() << ",";
        }
        errs() << *iter;
      }
      errs() << "\n";
    }
  }

  void handlePHINode(const PHINode *phiNode) {
    for (const Value *value : phiNode->incoming_values()) {
      LOG_DEBUG("Incoming value for PHINode: " << *value);
      if (const Function *func = dyn_cast<Function>(value)) {
        // LOG_DEBUG("Function in PHINode values: " << func->getName().data());
        addFuncName(func->getName().data());
      } else if (const PHINode *innerPHINode = dyn_cast<PHINode>(value)) {
        handlePHINode(innerPHINode);
      } else {
        LOG_DEBUG("Unhandled PHINode incoming value: " << *value);
      }
    }
  }

  void handleCallInst(const CallInst *callInst) {
    // getCalledFunction returns the function called,
    // or null if this is an indirect function invocation.
    if (const Function *func = callInst->getCalledFunction()) {
      // 直接函数调用（或者已经被优化成直接调用的间接函数调用）
      std::string funcName = func->getName().data();
      if (funcName == "llvm.dbg.value") {
        return;
      }
      addFuncName(funcName);
    } else { // 非直接函数调用
      LOG_DEBUG("Indirect function invocation: " << *callInst);
      const Value *value = callInst->getCalledOperand();
      LOG_DEBUG("Value of indirect function invocation: " << *value);
      if (const PHINode *phiNode = dyn_cast<PHINode>(value)) {
        handlePHINode(phiNode);
      } else {
        LOG_DEBUG("Unhandled CallOperand Value: " << *value);
      }
    }
    saveResultAndClearTemp(callInst->getDebugLoc().getLine());
  }

public:
  bool runOnModule(Module &M) override {
    errs() << "Hello: ";
    errs().write_escaped(M.getName()) << '\n';
    M.dump();
    errs() << "------------------------------\n";

    for (const Function &f : M) {
      for (const BasicBlock &bb : f) {
        for (const Instruction &i : bb) {
          if (const CallInst *callInst = dyn_cast<CallInst>(&i)) {
            handleCallInst(callInst);
          }
        }
      }
    }
    printResults();

    return false;
  }
};

char FuncPtrPass::ID = 0;
static RegisterPass<FuncPtrPass> X("funcptrpass",
                                   "Print function call instruction");

static cl::opt<std::string>
    InputFilename(cl::Positional, cl::desc("<filename>.bc"), cl::init(""));

int main(int argc, char **argv) {
  LLVMContext &Context = getGlobalContext();
  SMDiagnostic Err;
  // Parse the command line to read the Inputfilename
  cl::ParseCommandLineOptions(
      argc, argv, "FuncPtrPass \n My first LLVM too which does not do much.\n");

  // Load the input module
  std::unique_ptr<Module> M = parseIRFile(InputFilename, Err, Context);
  if (!M) {
    Err.print(argv[0], errs());
    return 1;
  }

  llvm::legacy::PassManager Passes;

  /// Remove functions' optnone attribute in LLVM5.0
  Passes.add(new EnableFunctionOptPass());
  /// Transform it to SSA
  Passes.add(llvm::createPromoteMemoryToRegisterPass());

  /// Your pass to print Function and Call Instructions
  Passes.add(new FuncPtrPass());
  Passes.run(*M.get());
}
