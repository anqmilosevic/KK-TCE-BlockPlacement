#ifndef LLVM_PROJECT_OURCFG_H
#define LLVM_PROJECT_OURCFG_H

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace llvm;

class OurCFG {
private:
  std::string FunctionName;

  std::unordered_map<BasicBlock *, std::vector<BasicBlock *>> AdjacencyList;
  std::unordered_map<BasicBlock *, std::vector<BasicBlock *>> PredecessorsList;

  std::unordered_set<BasicBlock *> Visited;

  std::unordered_map<BasicBlock *, std::unordered_set<BasicBlock *>> ReachableBlocks;

  void CreateCFG(Function &F);
  void ComputeReachableBlocks(Function &F);
  void DumpBasicBlock(BasicBlock *BB, raw_fd_ostream &File);

public:
  OurCFG(Function &F);
  ~OurCFG() {}

  void DFS(BasicBlock *CurrentBlock);
  void resetVisited();
  bool isReachable(BasicBlock *BB);
  bool isReachable(BasicBlock *From, BasicBlock *To);

  std::vector<BasicBlock *> &getSuccessors(BasicBlock *BB);
  std::vector<BasicBlock *> &getPredecessors(BasicBlock *BB);

  void DumpToFile();
};

#endif
