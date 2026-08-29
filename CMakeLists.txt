add_llvm_library( LLVMKKProject MODULE BUILDTREE_ONLY
  OurTailCallEliminationPass/OurTailCallEliminationPass.cpp
  OurBlockPlacementPass/OurBlockPlacementPass.cpp
  OurBlockPlacementPass/OurCFG.cpp
  CombinedPass.cpp

  DEPENDS
  intrinsics_gen
  PLUGIN_TOOL
  opt
  )
