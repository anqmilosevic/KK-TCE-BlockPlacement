#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

#include "OurBlockPlacementPass.h"
#include "OurCFG.h"

#include <unordered_map>
#include <vector>

using namespace llvm;

namespace {
  struct OurBlockPlacementPass : public FunctionPass {

    OurCFG *CFG;

    std::unordered_map<BasicBlock *, bool> Placed;

    std::vector<BasicBlock *> NewOrder;

    static char ID;
    OurBlockPlacementPass() : FunctionPass(ID) {}

    bool isColdBasicBlock(BasicBlock *BB) {
      if (isa<UnreachableInst>(BB->getTerminator())) {
        return true;
      }

      for (Instruction &I : *BB) {
        if (CallInst *Call = dyn_cast<CallInst>(&I)) {
          Function *Callee = Call->getCalledFunction();
          if (Callee == nullptr) {
            continue;
          }
          if (Callee->getName() == "exit" || Callee->getName() == "abort") {
            return true;
          }
        }
      }

      return false;
    }

    bool isLoopEdge(BasicBlock *From, BasicBlock *To) {
      return CFG->isReachable(To, From);
    }

    int getEdgeWeight(BasicBlock *From, BasicBlock *To) {
      if (isColdBasicBlock(To)) {
        return 5;
      }

      if (From->getTerminator()->getNumSuccessors() == 1) {
        return 100;
      }

      if (isLoopEdge(From, To)) {
        return 90;
      }

      return 50;
    }

    int getBasicBlockWeight(BasicBlock *BB) {
      if (isColdBasicBlock(BB)) {
        return 0;
      }

      int Weight = 1;

      for (BasicBlock *Predecessor : CFG->getPredecessors(BB)) {
        if (!Placed[Predecessor]) {
          continue;
        }

        int PredecessorWeight = getEdgeWeight(Predecessor, BB);
        if (PredecessorWeight > Weight) {
          Weight = PredecessorWeight;
        }
      }

      return Weight;
    }

    BasicBlock *findBestSuccessor(BasicBlock *BB) {
      BasicBlock *BestSuccessor = nullptr;
      int BestWeight = -1;

      for (BasicBlock *Successor : CFG->getSuccessors(BB)) {
        if (Placed[Successor]) {
          continue;
        }

        int Weight = getEdgeWeight(BB, Successor);
        if (Weight > BestWeight) {
          BestWeight = Weight;
          BestSuccessor = Successor;
        }
      }

      return BestSuccessor;
    }

    BasicBlock *findNextBasicBlock(Function &F) {
      BasicBlock *BestBasicBlock = nullptr;
      int BestWeight = -1;

      for (BasicBlock &BB : F) {
        if (Placed[&BB]) {
          continue;
        }

        int Weight = getBasicBlockWeight(&BB);
        if (Weight > BestWeight) {
          BestWeight = Weight;
          BestBasicBlock = &BB;
        }
      }

      return BestBasicBlock;
    }

    void createNewOrder(Function &F) {
      NewOrder.clear();
      Placed.clear();

      BasicBlock *Current = &F.front();

      while (Current != nullptr) {
        NewOrder.push_back(Current);
        Placed[Current] = true;

        BasicBlock *Next = findBestSuccessor(Current);
        if (Next == nullptr) {
          Next = findNextBasicBlock(F);
        }

        Current = Next;
      }
    }

    bool reorderBasicBlocks() {
      bool Changed = false;
      BasicBlock *Previous = NewOrder.front();

      for (size_t i = 1; i < NewOrder.size(); i++) {
        if (NewOrder[i]->getPrevNode() != Previous) {
          NewOrder[i]->moveAfter(Previous);
          Changed = true;
        }

        Previous = NewOrder[i];
      }

      return Changed;
    }

    bool invertConditionalBranches() {
      bool Changed = false;

      for (size_t i = 0; i + 1 < NewOrder.size(); i++) {
        BasicBlock *Current = NewOrder[i];
        BasicBlock *Next = NewOrder[i + 1];

        BranchInst *Branch = dyn_cast<BranchInst>(Current->getTerminator());
        if (Branch == nullptr || !Branch->isConditional()) {
          continue;
        }

        if (Branch->getSuccessor(0) != Next) {
          continue;
        }

        ICmpInst *Compare = dyn_cast<ICmpInst>(Branch->getCondition());
        if (Compare == nullptr || !Compare->hasOneUse()) {
          continue;
        }

        Compare->setPredicate(Compare->getInversePredicate());
        Branch->swapSuccessors();
        Changed = true;
      }

      return Changed;
    }

    void printBasicBlockOrder(Function &F, const char *Message) {
      errs() << Message << " (" << F.getName() << "): ";

      for (BasicBlock &BB : F) {
        BB.printAsOperand(errs(), false);
        errs() << " ";
      }

      errs() << "\n";
    }

    bool runOnFunction(Function &F) override {
      if (F.size() < 2) {
        return false;
      }

      CFG = new OurCFG(F);
      CFG->DumpToFile();

      printBasicBlockOrder(F, "Redosled pre ");

      createNewOrder(F);

      bool Changed = reorderBasicBlocks();
      Changed = invertConditionalBranches() || Changed;

      printBasicBlockOrder(F, "Redosled posle");

      delete CFG;

      return Changed;
    }
  };
}

char OurBlockPlacementPass::ID = 0;

static RegisterPass<OurBlockPlacementPass>
  Y("our-block-placement", "Our block placement pass");

bool runOurBlockPlacement(Function &F) {
  OurBlockPlacementPass Pass;

  return Pass.runOnFunction(F);
}
