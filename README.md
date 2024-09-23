# bfllvm
A simple [BF](https://en.wikipedia.org/wiki/Brainfuck) compiler emitting LLVM bitcode, for learning LLVM structures and some fun.
## Build
Building is very simple, you need clang or g++, cmake and a decent llvm version (18 or better):
   
    cd bfllvm
    mkdir build
    cd build
    cmake ../src
    make

After building, you can use the executable like this:

    bfllvm --out my_file.bc < input.bf

The resulting LLVM bitfile can then be used as input e.g. to ``lli``. There is also a the simple shell script ``execute_it.sh`` in ``test`` to
do compile and run ``lli`` in one:

    sh execute_it.sh hw.bf
    >>>Hello World!

