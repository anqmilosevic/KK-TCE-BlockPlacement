#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

#include "OurTailCallEliminationPass.h"

#include <unordered_map>
#include <vector>

using namespace llvm;

namespace {
  struct OurTailCallEliminationPass : public FunctionPass {

    std::unordered_map<Value *, Value *> VariablesMap;

    std::vector<Value *> ArgumentsAllocas;

    Value *ReturnValueAlloca;

    std::vector<CallInst *> TailCalls;

    std::vector<Instruction *> InstructionsToRemove;

    BasicBlock *LoopHeader;

    static char ID;
    OurTailCallEliminationPass() : FunctionPass(ID) {}

    void mapVariables(Function &F) {
      for (BasicBlock &BB : F) {
        for (Instruction &I : BB) {
          if (isa<LoadInst>(&I)) {
            VariablesMap[&I] = I.getOperand(0);
          }
        }
      }
    }

    void findArgumentsAllocas(Function &F) {
      for (Argument &Arg : F.args()) {
        Value *Alloca = nullptr;

        for (Instruction &I : F.front()) {
          if (StoreInst *Store = dyn_cast<StoreInst>(&I)) {
            if (Store->getOperand(0) == &Arg) {

              if (isa<AllocaInst>(Store->getOperand(1))) {
                Alloca = Store->getOperand(1);
              }
              break;
            }
          }
        }

        ArgumentsAllocas.push_back(Alloca);
      }
    }

    void findReturnValueAlloca(Function &F) {
      ReturnValueAlloca = nullptr;

      for (BasicBlock &BB : F) {
        if (ReturnInst *Ret = dyn_cast<ReturnInst>(BB.getTerminator())) {
          if (Ret->getReturnValue() != nullptr && isa<LoadInst>(Ret->getReturnValue())) {
            ReturnValueAlloca = VariablesMap[Ret->getReturnValue()];
          }
        }
      }
    }

    bool isReturnBasicBlock(BasicBlock *BB) {
      if (!isa<ReturnInst>(BB->getTerminator())) {
        return false;
      }

      for (Instruction &I : *BB) {
        if (isa<ReturnInst>(&I)) {
          continue;
        }
        if (isa<LoadInst>(&I) && VariablesMap[&I] == ReturnValueAlloca) {
          continue;
        }
        return false;
      }

      return true;
    }

    bool isSafeToEliminate(Function &F) {
      for (BasicBlock &BB : F) {
        for (Instruction &I : BB) {
          if (!isa<AllocaInst>(&I)) {
            continue;
          }

          if (&BB != &F.front()) {
            return false;
          }

          for (User *Usr : I.users()) {
            if (isa<LoadInst>(Usr)) {
              continue;
            }
            if (StoreInst *Store = dyn_cast<StoreInst>(Usr)) {
              if (Store->getOperand(1) == &I) {
                continue;
              }
            }
            return false;
          }
        }
      }

      return true;
    }

    bool isTailCall(CallInst *Call, Function &F) {
      if (Call->getCalledFunction() != &F) {
        return false;
      }

      if (Call->arg_size() != ArgumentsAllocas.size()) {
        return false;
      }

      for (Value *Alloca : ArgumentsAllocas) {
        if (Alloca == nullptr) {
          return false;
        }
      }

      Instruction *Next = Call->getNextNode();
      if (Next == nullptr) {
        return false;
      }

      if (ReturnInst *Ret = dyn_cast<ReturnInst>(Next)) {
        if (Ret->getReturnValue() == nullptr) {
          return Call->use_empty();
        }

        return Ret->getReturnValue() == Call;
      }

      if (StoreInst *Store = dyn_cast<StoreInst>(Next)) {
        if (Store->getOperand(0) != Call || Store->getOperand(1) != ReturnValueAlloca) {
          return false;
        }

        BranchInst *Branch = dyn_cast<BranchInst>(Store->getNextNode());
        if (Branch == nullptr || Branch->isConditional()) {
          return false;
        }

        return isReturnBasicBlock(Branch->getSuccessor(0));
      }

      if (BranchInst *Branch = dyn_cast<BranchInst>(Next)) {
        if (Branch->isConditional() || !Call->use_empty()) {
          return false;
        }

        return isReturnBasicBlock(Branch->getSuccessor(0));
      }

      return false;
    }

    void findTailCalls(Function &F) {
      for (BasicBlock &BB : F) {
        for (Instruction &I : BB) {
          if (CallInst *Call = dyn_cast<CallInst>(&I)) {
            if (isTailCall(Call, F)) {
              TailCalls.push_back(Call);
            }
          }
        }
      }
    }

    BasicBlock *createLoopHeader(Function &F) {
      BasicBlock *Entry = &F.front();
      Instruction *SplitPoint = nullptr;

      for (Instruction &I : *Entry) {
        if (isa<AllocaInst>(&I)) {
          continue;
        }
        if (StoreInst *Store = dyn_cast<StoreInst>(&I)) {
          if (isa<Argument>(Store->getOperand(0))) {
            continue;
          }
        }

        SplitPoint = &I;
        break;
      }

      if (SplitPoint == nullptr) {
        return nullptr;
      }

      return Entry->splitBasicBlock(SplitPoint, "tailrecurse");
    }

    void removeAllInstructions(BasicBlock &BB, Instruction *RemoveFromInstruction) {
      bool Found = false;

      InstructionsToRemove.clear();

      for (Instruction &I : BB) {
        if (&I == RemoveFromInstruction) {
          Found = true;
        }

        if (Found) {
          InstructionsToRemove.push_back(&I);
        }
      }

      for (int i = (int) InstructionsToRemove.size() - 1; i >= 0; i--) {
        InstructionsToRemove[i]->eraseFromParent();
      }
    }

    void eliminateTailCall(CallInst *Call) {
      BasicBlock *CallBasicBlock = Call->getParent();
      IRBuilder<> Builder(Call);

      for (size_t i = 0; i < Call->arg_size(); i++) {
        Builder.CreateStore(Call->getArgOperand(i), ArgumentsAllocas[i]);
      }

      removeAllInstructions(*CallBasicBlock, Call);

      Builder.SetInsertPoint(CallBasicBlock);
      Builder.CreateBr(LoopHeader);
    }

    bool runOnFunction(Function &F) override {
      VariablesMap.clear();
      ArgumentsAllocas.clear();
      TailCalls.clear();

      mapVariables(F);
      findArgumentsAllocas(F);
      findReturnValueAlloca(F);

      if (!isSafeToEliminate(F)) {
        return false;
      }

      findTailCalls(F);

      if (TailCalls.empty()) {
        return false;
      }

      LoopHeader = createLoopHeader(F);
      if (LoopHeader == nullptr) {
        return false;
      }

      for (CallInst *Call : TailCalls) {
        eliminateTailCall(Call);
      }

      errs() << "Eliminisano repnih poziva u funkciji " << F.getName() << ": "
             << TailCalls.size() << "\n";

      return true;
    }
  };
}

char OurTailCallEliminationPass::ID = 0;

static RegisterPass<OurTailCallEliminationPass>
  X("our-tail-call-elimination", "Our tail call elimination pass");

bool runOurTailCallElimination(Function &F) {
  OurTailCallEliminationPass Pass;

  return Pass.runOnFunction(F);
}
