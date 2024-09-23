/*
bfllvm
(C) Andreas Gaiser (doraeneko@github.com), 2024
LLVM Code generation for bf compiler.
 */

#include "code_gen.h"

#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/TargetParser/Host.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Bitcode/BitcodeWriter.h"
#include <iostream>

using namespace llvm;

const uint32_t BF_ARRAY_SIZE = 60000;

namespace bfllvm
{
    void Code_Gen_Visitor::init_structures()
    {
        InitializeNativeTarget();
        InitializeNativeTargetAsmPrinter();
        _context = new LLVMContext();
        _module = new Module("bfllvm", *_context);
        _builder = new IRBuilder<>(*_context);
        _i32_type = Type::getInt32Ty(*_context);
        _char_type = Type::getInt8Ty(*_context);
        _ptr_type = _builder->getPtrTy();
        _i32_zero = ConstantInt::get(_i32_type, 0);
        _i32_one = ConstantInt::get(_i32_type, 1);
        _i32_minus_one = ConstantInt::get(_i32_type, -1, true);
        _char_zero = ConstantInt::get(_char_type, 0);
        _char_one = ConstantInt::get(_char_type, 1);

        // create required libc function declarations
        FunctionType *putchar_type = FunctionType::get(
            _i32_type,
            {_i32_type},
            false);
        FunctionType *getchar_type = FunctionType::get(
            _i32_type,
            {},
            false);
        FunctionType *fflush_type = FunctionType::get(
            _i32_type, {_ptr_type}, false);

        _putchar = Function::Create(
            putchar_type,
            Function::ExternalLinkage,
            "putchar",
            _module);
        _getchar = Function::Create(
            getchar_type,
            Function::ExternalLinkage,
            "getchar",
            _module);
        _fflush = Function::Create(
            fflush_type,
            Function::ExternalLinkage,
            "fflush",
            _module);

        // stdout global variable declaration
        _stdout = new GlobalVariable(
            *_module, _ptr_type, false, GlobalValue::ExternalLinkage, nullptr, "stdout");
        _stdout->setAlignment(Align(8));

        // we only need one main function
        FunctionType *funcType = FunctionType::get(_i32_type, false);
        _main = Function::Create(
            funcType, Function::ExternalLinkage, "main", *_module);

        // create the bf array
        _array_type = ArrayType::get(_char_type, BF_ARRAY_SIZE);
        Constant *zero_init = ConstantAggregateZero::get(_array_type);
        _bf_array = new GlobalVariable(
            *_module,
            _array_type,
            false,
            GlobalValue::PrivateLinkage,
            zero_init,
            "bf_array");
    }

    void Code_Gen_Visitor::generate_code()
    {
        init_structures();

        // allocate current_ptr, store &bf_array[0]
        BasicBlock *entry_bb = BasicBlock::Create(*_context, "entry", _main);
        _builder->SetInsertPoint(entry_bb);
        _current_ptr = _builder->CreateAlloca(_ptr_type, nullptr, "current_ptr");
        const auto array_ref = ArrayRef<Value *>{
            _i32_zero};
        _builder->CreateStore(_builder->CreateGEP(_array_type, _bf_array, array_ref), _current_ptr);

        // generate bf code
        _ast->accept(*this);

        // add an end block, always return 0 here.
        BasicBlock *end_bb = BasicBlock::Create(*_context, "end", _main);
        _builder->CreateBr(end_bb);
        _builder->SetInsertPoint(end_bb);
        _builder->CreateRet(_i32_zero);

        // perform verification checks
        verifyFunction(*_main, &errs());
        verifyModule(*_module, &errs());

        // _module->print(errs(), nullptr);
    }

    void Code_Gen_Visitor::output_current_value()
    {
        // call putchar() with *(*_current_ptr)
        Value *ptr_value = _builder->CreateLoad(_ptr_type, _current_ptr);
        Value *out_value = _builder->CreateLoad(_char_type, ptr_value);
        // extend to int
        out_value = _builder->CreateSExt(out_value, _i32_type);
        _builder->CreateCall(_putchar, out_value);
        Value *stdout_value = _builder->CreateLoad(_ptr_type, _stdout);
        _builder->CreateCall(_fflush, {stdout_value});
    }

    void Code_Gen_Visitor::output_char(char c)
    {
        _builder->CreateCall(_putchar, ConstantInt::get(_i32_type, c));
        Value *stdout_value = _builder->CreateLoad(_ptr_type, _stdout);
        _builder->CreateCall(_fflush, {stdout_value});
    }

    void Code_Gen_Visitor::visit(const Pointer_Increment &v)
    {
        // increment (*_current_ptr)
        Value *ptr = _builder->CreateLoad(_ptr_type, _current_ptr);
        Value *inc_ptr = _builder->CreateGEP(_char_type, ptr, _i32_one);
        _builder->CreateStore(inc_ptr, _current_ptr);
    }

    void Code_Gen_Visitor::visit(const Pointer_Decrement &v)
    {
        // decrement (*_current_ptr)
        Value *ptr = _builder->CreateLoad(_ptr_type, _current_ptr);
        Value *dec_ptr = _builder->CreateGEP(_char_type, ptr, _i32_minus_one);
        _builder->CreateStore(dec_ptr, _current_ptr);
    }

    void Code_Gen_Visitor::visit(const Value_Increment &v)
    {
        // increment *(*_current_ptr)
        Value *ptr = _builder->CreateLoad(_ptr_type, _current_ptr);
        Value *old_value = _builder->CreateLoad(_char_type, ptr);
        Value *new_value = _builder->CreateAdd(old_value, _char_one);
        _builder->CreateStore(new_value, ptr);
    }

    void Code_Gen_Visitor::visit(const Value_Decrement &v)
    {
        // decrement *(*_current_ptr)
        Value *ptr = _builder->CreateLoad(_ptr_type, _current_ptr);
        Value *old_value = _builder->CreateLoad(_char_type, ptr);
        Value *new_value = _builder->CreateSub(old_value, _char_one);
        _builder->CreateStore(new_value, ptr);
    }

    void Code_Gen_Visitor::visit(const Put_Char &v)
    {
        output_current_value();
    }

    void Code_Gen_Visitor::visit(const Get_Char &v)
    {
        // call getchar() and store the return value in *(*_current_ptr)
        Value *in_value = _builder->CreateCall(_getchar);
        in_value = _builder->CreateTrunc(in_value, _char_type);
        Value *ptr_value = _builder->CreateLoad(_ptr_type, _current_ptr);
        _builder->CreateStore(in_value, ptr_value);
    }

    void Code_Gen_Visitor::visit(const Sequence &v)
    {
        uint32_t length = v.size();
        for (uint32_t i = 0; i < length; ++i)
        {
            v.get(i)->accept(*this);
        }
    }

    void Code_Gen_Visitor::visit(const While_Loop &v)
    {
        // create a block computing the condition *(*_current_ptr) != 0
        BasicBlock *cond_bb = BasicBlock::Create(*_context, "condition", _main);
        _builder->CreateBr(cond_bb);
        _builder->SetInsertPoint(cond_bb);
        // create also a block for start of the loop, and one for code after the loop
        BasicBlock *loop_body_start_bb =
            BasicBlock::Create(*_context, "loop_body_start", _main);
        BasicBlock *after_loop_bb = BasicBlock::Create(*_context, "after_loop", _main);

        // code for condition
        Value *ptr_value = _builder->CreateLoad(_ptr_type, _current_ptr);
        Value *deref_value = _builder->CreateLoad(_char_type, ptr_value);
        Value *comparison = _builder->CreateICmpNE(deref_value, _char_zero, "cmp");
        _builder->CreateCondBr(comparison, loop_body_start_bb, after_loop_bb);

        // code for loop body
        _builder->SetInsertPoint(loop_body_start_bb);
        // iterate over body
        uint32_t length = v.size();
        for (uint32_t i = 0; i < length; ++i)
        {
            v.get(i)->accept(*this);
        }

        // create another block for jumping back to the condition
        BasicBlock *loop_jump_back_bb = BasicBlock::Create(*_context, "loop_back", _main);
        _builder->CreateBr(loop_jump_back_bb);
        _builder->SetInsertPoint(loop_jump_back_bb);
        _builder->CreateBr(cond_bb);

        // continue code generation with after loop block
        _builder->SetInsertPoint(after_loop_bb);
    }

    void Code_Gen_Visitor::write_object_file(std::string out_file)
    {
        std::error_code EC;
        raw_fd_ostream OS(out_file, EC, sys::fs::FA_Write);
        WriteBitcodeToFile(*_module, OS);
        OS.flush();
    }
}
