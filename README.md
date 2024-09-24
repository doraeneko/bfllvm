# bfllvm

A simple Brainf*ck ([BF](https://en.wikipedia.org/wiki/Brainfuck)) compiler that emits LLVM bitcode, designed for learning LLVM structures and having some fun along the way. BF is an esoteric programming language I discovered during my first university course on compilers. Its syntax and semantics are extremely minimalistic and closely resemble a Turing machine. It is Turing-complete under certain assumptions (e.g. infinite memory).

Syntax:
  - BF programs are strings consisting of ``>`` , ``<``, ``+``, ``-``, ``.``, ``,``, ``[`` and ``]``. Other characters are allowed, but have no meaning
  in the language (are comments, so to speak).
  - Additionally, ``[`` and ``]`` have to form proper nestings. E.g., ``..[+]]`` is not a correct BF program.

Semantics:
  - Memory given in form of a char array, with a single pointer variable ``p`` pointing to the "current" location in the array.
  - Commands `>` and `<` move the pointer one position right or left (``p--``, ``p++``).
  - Commands `+` and `-` increment and decrement the value ``*p``.
  - Command `.` outputs the character ``*p``.
  - Command `,` reads in a character from stdin and writes it into ``*p``.
  - A while loop can be realized by ``[A]B``, where ``A`` and ``B`` are again BF programs: if ``*p != 0``, ``A`` is executed and execution jumps back to the initial ``[``; otherwise,  execution continues with ``B``.

The obligatory "hello word" in BF (from [therenegadecoder.com](https://therenegadecoder.com/code/hello-world-in-brainfuck/)):

    >++++++++[<+++++++++>-]<.>++++[<+++++++>-]<+.+++++++..+++.>>++++++[<+++++++>-]<+
    +.------------.>++++++[<+++++++++>-]<+.<.+++.------.--------.>>>++++[<++++++++>-
    ]<+.


The compiler ``bfllvm`` contains a primitive hand-written lexer, parser, abstract syntax tree, and code generation for LLVM code for this
mini language. I used it to familiarize myself with LLVMs facilities.

## Building and Tests
You need clang or g++, cmake, pytest and a decent llvm version (18 or better):
   
    cd bfllvm
    mkdir build
    cd build
    cmake ../src
    make

Some simple end-to-end tests are located in ``src/tests``.

## Compiling BF programs
After building, you can use the executable like this:

    bfllvm --out my_file.bc < input.bf

The resulting LLVM bitfile can be used as input e.g. to ``lli``. You can also
use ``llc`` to create native assembler code. There is also a simple shell script ``execute_it.sh`` in ``tests`` to
do compile and run ``lli`` in one:

    sh execute_it.sh hello_world.bf
    Hello, World!

