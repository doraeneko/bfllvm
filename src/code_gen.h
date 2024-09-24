/*
bfllvm
(C) Andreas Gaiser (doraeneko@github.com), 2024
LLVM Code generation for bf compiler.
 */

#ifndef CODE_GEN_H
#define CODE_GEN_H

#include "ast.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace bfllvm
{

    class Code_Gen_Visitor : public AST_Visitor
    {
        const AST &_ast;

        // builder structures
        llvm::LLVMContext *_context;
        llvm::IRBuilder<> *_builder;
        llvm::Module *_module;
        llvm::Function *_main;

        // llvm structures
        llvm::Type *_i32_type{nullptr};
        llvm::Type *_char_type{nullptr};
        llvm::Type *_ptr_type{nullptr};
        llvm::Type *_array_type{nullptr};

        llvm::Constant *_i32_zero{nullptr};
        llvm::Constant *_i32_minus_one{nullptr};
        llvm::Constant *_i32_one{nullptr};
        llvm::Constant *_char_zero{nullptr};
        llvm::Constant *_char_one{nullptr};
        llvm::Function *_putchar;
        llvm::Function *_getchar;
        llvm::Function *_fflush;
        llvm::Value *_current_ptr{nullptr};
        llvm::GlobalVariable *_bf_array{nullptr};
        llvm::GlobalVariable *_stdout{nullptr};

        // code generation functions

        // emit code to call putchar(int)
        void output_current_value();

        // can be used for debugging
        void output_char(char number);

        void visit(const Pointer_Decrement &v) override;
        void visit(const Pointer_Increment &v) override;
        void visit(const Value_Decrement &v) override;
        void visit(const Value_Increment &v) override;
        void visit(const Put_Char &v) override;
        void visit(const Get_Char &v) override;
        void visit(const Sequence &v) override;
        void visit(const While_Loop &v) override;

        void init_structures();

    public:
        Code_Gen_Visitor(const AST &ast)
            : _ast{ast}, _context{}, _builder{}, _module{}, _main{}
        {
        }

        void generate_code();

        void write_object_file(std::string out_file);
    };

}

#endif