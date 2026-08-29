#ifndef LLVM_OURTAILCALLELIMINATIONPASS_H
#define LLVM_OURTAILCALLELIMINATIONPASS_H

#include "llvm/IR/Function.h"

using namespace llvm;

bool runOurTailCallElimination(Function &F);

#endif
