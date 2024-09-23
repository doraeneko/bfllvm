/*
bfllvm
(C) Andreas Gaiser, 2024
Almost trivial AST for bf compiler.
 */

#ifndef AST_H
#define AST_H

#include <cstdint>
#include <vector>
#include <memory>
#include <string>

namespace bfllvm
{

    class AST_Element;
    class Sequence;
    class While_Loop;
    class Pointer_Decrement;
    class Pointer_Increment;
    class Value_Decrement;
    class Value_Increment;
    class Put_Char;
    class Get_Char;

    class AST_Visitor
    {
    public:
        virtual void visit(const Sequence &v) {}
        virtual void visit(const While_Loop &v) {}
        virtual void visit(const Pointer_Decrement &v) {}
        virtual void visit(const Pointer_Increment &v) {}
        virtual void visit(const Value_Decrement &v) {}
        virtual void visit(const Value_Increment &v) {}
        virtual void visit(const Put_Char &v) {}
        virtual void visit(const Get_Char &v) {}
    };

    class AST_Element
    {

    public:
        virtual std::string print(std::uint32_t indentation = 0) const = 0;

        virtual ~AST_Element() = default;

        virtual void accept(AST_Visitor &visitor) = 0;
    };

    typedef std::shared_ptr<AST_Element> AST;

    // Sequence of AST elements
    class Sequence : public AST_Element
    {
    protected:
        std::vector<AST> _inner;

    public:
        void push_back(const AST &element)
        {
            _inner.push_back(element);
        }

        void push_back(AST &&element)
        {
            _inner.push_back(element);
        }

        const std::vector<AST> &get_inner() const
        {
            return _inner;
        }

        std::uint32_t size() const
        {
            return _inner.size();
        }

        AST get(const std::uint32_t index) const
        {
            return _inner[index];
        }

        std::string print(std::uint32_t indentation) const override
        {
            std::string indent(indentation, ' ');
            std::string result = indent + "{\n";

            for (const auto &element : _inner)
            {
                result += element->print(indentation + 4) + "\n";
            }
            result += indent + "}";
            return result;
        };

        void accept(AST_Visitor &visitor) override
        {
            visitor.visit(*this);
        };
    };

    class Instruction : public AST_Element
    {
    protected:
        virtual std::string get_type() const
        {
            return "instr";
        }

        std::string print(std::uint32_t indentation) const override
        {
            std::string indent(indentation, ' ');
            return indent + "<" + get_type() + ">";
        };
    };

    class Pointer_Increment : public Instruction
    {
        std::string get_type() const override
        {
            return "ptr++";
        }

        void accept(AST_Visitor &visitor) override
        {
            visitor.visit(*this);
        };
    };

    class Pointer_Decrement : public Instruction
    {
        std::string get_type() const override
        {
            return "ptr--";
        }

        void accept(AST_Visitor &visitor) override
        {
            visitor.visit(*this);
        };
    };

    class Value_Increment : public Instruction
    {
        std::string get_type() const override
        {
            return "*ptr++";
        }

        void accept(AST_Visitor &visitor) override
        {
            visitor.visit(*this);
        };
    };

    class Value_Decrement : public Instruction
    {
        std::string get_type() const override
        {
            return "*ptr--";
        }

        void accept(AST_Visitor &visitor) override
        {
            visitor.visit(*this);
        };
    };

    class Put_Char : public Instruction
    {
        std::string get_type() const override
        {
            return "putchar";
        }

        void accept(AST_Visitor &visitor) override
        {
            visitor.visit(*this);
        };
    };

    class Get_Char : public Instruction
    {
        std::string get_type() const override
        {
            return "getchar";
        }

        void accept(AST_Visitor &visitor) override
        {
            visitor.visit(*this);
        };
    };

    class While_Loop : public Sequence
    {

    public:
        While_Loop(const Sequence &seq) : Sequence()
        {
            _inner = seq.get_inner();
        }

        std::string print(std::uint32_t indentation) const override
        {
            std::string indent(indentation, ' ');
            return indent + "while(*ptr!=0)" + Sequence::print(indentation);
        }

        void accept(AST_Visitor &visitor) override
        {
            visitor.visit(*this);
        };
    };

}

#endif