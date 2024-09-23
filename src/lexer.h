/*
bfllvm
(C) Andreas Gaiser (doraeneko@github.com), 2024
Lexer for bfllvm.
*/

#ifndef LEXER_H
#define LEXER_H

#include <cstdint>
#include <istream>

namespace bfllvm
{

    enum class Token : std::uint32_t
    {
        PTR_INC = 0,
        PTR_DEC,
        VAL_INC,
        VAL_DEC,
        PUT_CHAR,
        GET_CHAR,
        WHILE_START,
        WHILE_END,
        OTHER,
        END
    };

    class Lexer
    {
        std::istream &_stream;

    public:
        Lexer(std::istream &stream)
            : _stream(stream)
        {
        }

        // remove next token from stream and return it
        Token get_next();
        // just peek the next token
        Token peek();

        virtual ~Lexer() = default;
    };

}

#endif