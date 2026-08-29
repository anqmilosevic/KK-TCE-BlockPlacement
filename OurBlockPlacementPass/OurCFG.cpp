#include "OurCFG.h"

OurCFG::OurCFG(Function &F)
{
  FunctionName = F.getName().str();
  CreateCFG(F);
  ComputeReachableBlocks(F);
}

void OurCFG::CreateCFG(Function &F)
{
  for (BasicBlock &BB : F) {
    AdjacencyList[&BB] = {};
    PredecessorsList[&BB] = {};
  }

  for (BasicBlock &BB : F) {
    for (BasicBlock *Successor : successors(&BB)) {
      AdjacencyList[&BB].push_back(Successor);
      PredecessorsList[Successor].push_back(&BB);
    }
  }
}

void OurCFG::DFS(BasicBlock *CurrentBlock)
{
  Visited.insert(CurrentBlock);

  for (BasicBlock *Successor : AdjacencyList[CurrentBlock]) {
    if (Visited.find(Successor) == Visited.end()) {
      DFS(Successor);
    }
  }
}

void OurCFG::resetVisited()
{
  Visited.clear();
}

bool OurCFG::isReachable(BasicBlock *BB)
{
  return Visited.find(BB) != Visited.end();
}

void OurCFG::ComputeReachableBlocks(Function &F)
{
  for (BasicBlock &BB : F) {
    resetVisited();
    DFS(&BB);
    ReachableBlocks[&BB] = Visited;
  }

  resetVisited();
}

bool OurCFG::isReachable(BasicBlock *From, BasicBlock *To)
{
  return ReachableBlocks[From].find(To) != ReachableBlocks[From].end();
}

std::vector<BasicBlock *> &OurCFG::getSuccessors(BasicBlock *BB)
{
  return AdjacencyList[BB];
}

std::vector<BasicBlock *> &OurCFG::getPredecessors(BasicBlock *BB)
{
  return PredecessorsList[BB];
}

void OurCFG::DumpToFile()
{
  std::error_code error;
  raw_fd_ostream File("our" + FunctionName + ".dot", error);

  File << "digraph \"CFG for '" << FunctionName << "' function\" {\n";
  File << "\tlabel=\"CFG for '" << FunctionName << "' function\";\n\n";

  for (const auto &p : AdjacencyList) {
    DumpBasicBlock(p.first, File);
  }

  File << "}\n";
}

void OurCFG::DumpBasicBlock(BasicBlock *BB, raw_fd_ostream &File)
{
  bool MultipleSuccessors = AdjacencyList[BB].size() > 1;

  File << "\tNode" << BB << " [shape=record,color=\"#b70d28ff\", style=filled, "
       << "fillcolor=\"#b70d2870\",label=\"{";

  for (Instruction &I : *BB) {
    if (!I.isTerminator()) {
      File << I << "\\l  ";
    }
    else if (MultipleSuccessors) {
      File << I << "\\l|{<s0>T|<s1>F}}\"];\n";
    }
    else {
      File << I << "\\l}\"];\n";
    }
  }

  int index = 0;

  for (BasicBlock *Successor : AdjacencyList[BB]) {
    if (MultipleSuccessors) {
      File << "\tNode" << BB << ":s" << index++ << " -> Node" << Successor << ";\n";
    }
    else {
      File << "\tNode" << BB << " -> Node" << Successor << ";\n";
    }
  }
}
