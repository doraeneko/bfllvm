/*
bfllvm
(C) Andreas Gaiser (doraeneko@github.com), 2024
Driver of the bfllvm compiler, containing the main function.
 */

#include <iostream>
#include <sstream>
#include <string>
#include "code_gen.h"
#include "parser.h"
using namespace bfllvm;

int main(int argc, char *argv[])
{
    std::stringstream ss;
    std::string line;
    std::string out_file = "bf.bc";

    if (argc == 3 && (argv[1] == std::string("--out") || argv[1] == std::string("-o")))
    {
        out_file = argv[2];
    }
    else if (argc != 1)
    {
        std::cout << "Compiler to transform bf code to llvm bitcode.\n"
                  << "Copyright 2024, Andreas Gaiser (doraeneko@github)\n\n"
                  << "Usage example: bfllvm --out output.bc  < program.bf\n\n"
                  << "If --out / -o is not given, bf.bc is the output file.\n"
                  << "Use e.g. lli output.bc to execute the bitcode file, or\n"
                  << "use llc and gcc to compute a native executable." << std::endl;
        return 0;
    }
    while (std::getline(std::cin, line))
    {
        ss << line << "\n";
    }
    Parser p{ss};
    const auto ast = p.parse();
    if (ast == nullptr)
    {
        std::cout << "Parsing error: " << p.state() << std::endl;
        return 1;
    }
    Code_Gen_Visitor cgv(ast);
    cgv.generate_code();
    cgv.write_object_file(out_file);
    return 0;
}