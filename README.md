# LLVM Tail Call Elimination & Block Placement Passes

LLVM legacy pass plugin implementing Tail Call Elimination and Block Placement. Tail call elimination replaces self-recursive calls in tail position with a jump back to the top of the function, so the recursion runs in constant stack space. Block placement reorders the basic blocks of a function so that the most likely path is contiguous, moves cold blocks to the end and inverts conditional branches so that the hot successor becomes the fall-through. The combined pass runs tail call elimination and then block placement, in that order, since tail call elimination creates the loop that block placement lays out.

## Requirements

- LLVM 17.0.0
- Clang 17.0.0
- CMake
- GNU Make
- Graphviz

## Build

Place `KKProject` in `llvm/lib/Transforms` and add the following line to `llvm/lib/Transforms/CMakeLists.txt`:

```cmake
add_subdirectory(KKProject)
```

## Build

Place the project in `llvm/lib/Transforms` under the name `KKProject` and add the following line to `llvm/lib/Transforms/CMakeLists.txt`:

```cmake
add_subdirectory(KKProject)
```

Build the plugin from the LLVM build directory:

```bash
cmake -S ../llvm -B .
cmake --build . --target LLVMKKProject
```

Each pass has its own directory, but a single `add_llvm_library` builds them into one plugin, `lib/LLVMKKProject.so`.

## Usage

Compile a C example to LLVM IR:

```bash
./bin/clang \
  -O0 \
  -Xclang -disable-O0-optnone \
  -fno-discard-value-names \
  -emit-llvm -S \
  combined.c -o combined.ll
```

Run the combined pass:

```bash
./bin/opt \
  -load lib/LLVMKKProject.so \
  -enable-new-pm=0 \
  -our-tce-block-placement \
  combined.ll \
  -S -o combined_out.ll
```

Individual passes can be executed with `-our-tail-call-elimination` or `-our-block-placement`.

Tail call elimination prints how many calls it removed from each function. Block placement prints the block order before and after the transformation and writes a `.dot` file with the control flow graph of each function, which can be rendered with `dot -Tpng ourfactorial.dot -o ourfactorial.png`.

On LLVM releases where `opt` no longer accepts `-enable-new-pm=0`, the passes are also registered with the new pass manager:

```bash
./bin/opt \
  -load-pass-plugin lib/LLVMKKProject.so \
  -passes=our-tce-block-placement \
  combined.ll \
  -S -o combined_out.ll
```

## Examples

Examples are in `Examples`, each with the source file, the input IR and the output IR. `tail_call.c` contains a tail recursive function, a function with two tail calls, a tail recursive `void` function and a function that is not tail recursive and is therefore left untouched. `deep_recursion.c` recurses one million levels deep: without the pass the program overflows the stack, after the pass it runs in constant stack space. `block_placement.c` contains a cold block, a loop and both combined. `combined.c` is a tail recursive function whose new loop is then laid out by block placement.

Tail call elimination handles direct recursion only, and gives up when the address of a local variable escapes the function, when an `alloca` appears outside the entry block, or when a parameter is not kept in an `alloca` at all. Block placement relies on static heuristics, without profile information.
