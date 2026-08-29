#include "OurBlockPlacementPass/OurBlockPlacementPass.h"
#include "OurTailCallEliminationPass/OurTailCallEliminationPass.h"

#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

namespace {

  struct OurCombinedPass : public FunctionPass {
    static char ID;
    OurCombinedPass() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
      bool Changed = runOurTailCallElimination(F);

      if (runOurBlockPlacement(F)) {
        Changed = true;
      }

      return Changed;
    }
  };

}

char OurCombinedPass::ID = 0;

static RegisterPass<OurCombinedPass>
  Z("our-tce-block-placement", "Our combined tail call elimination and block placement pass");

namespace {

  struct OurTailCallEliminationNewPM
    : public PassInfoMixin<OurTailCallEliminationNewPM> {
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
      if (!runOurTailCallElimination(F)) {
        return PreservedAnalyses::all();
      }

      return PreservedAnalyses::none();
    }
  };

  struct OurBlockPlacementNewPM
    : public PassInfoMixin<OurBlockPlacementNewPM> {
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
      if (!runOurBlockPlacement(F)) {
        return PreservedAnalyses::all();
      }

      return PreservedAnalyses::none();
    }
  };

  struct OurCombinedNewPM : public PassInfoMixin<OurCombinedNewPM> {
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
      bool Changed = runOurTailCallElimination(F);

      if (runOurBlockPlacement(F)) {
        Changed = true;
      }

      if (!Changed) {
        return PreservedAnalyses::all();
      }

      return PreservedAnalyses::none();
    }
  };

}

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION,
    "KKProject",
    LLVM_VERSION_STRING,
    [](PassBuilder &Builder) {
      Builder.registerPipelineParsingCallback(
        [](StringRef Name, FunctionPassManager &FPM,
           ArrayRef<PassBuilder::PipelineElement>) {
          if (Name == "our-tail-call-elimination") {
            FPM.addPass(OurTailCallEliminationNewPM());
            return true;
          }

          if (Name == "our-block-placement") {
            FPM.addPass(OurBlockPlacementNewPM());
            return true;
          }

          if (Name == "our-tce-block-placement") {
            FPM.addPass(OurCombinedNewPM());
            return true;
          }

          return false;
        });
    }
  };
}
