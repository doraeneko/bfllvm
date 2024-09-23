/*
bfllvm
(C) Andreas Gaiser (doraeneko@github.com), 2024
Lexer for bfllvm.
*/

#include "lexer.h"
#include <string>
#include <iostream>
#include <sstream>

namespace bfllvm
{
    Token Lexer::get_next()
    {
        if (!_stream.eof())
        {
            const auto next = this->peek();
            _stream.get();
            return next;
        }
        return Token::END;
    }

    Token Lexer::peek()
    {
        if (!_stream.eof())
        {
            const auto next = _stream.peek();
            switch (static_cast<char>(next))
            {
            case '<':
                return Token::PTR_DEC;
            case '>':
                return Token::PTR_INC;
            case '+':
                return Token::VAL_INC;
            case '-':
                return Token::VAL_DEC;
            case '.':
                return Token::PUT_CHAR;
            case ',':
                return Token::GET_CHAR;
            case '[':
                return Token::WHILE_START;
            case ']':
                return Token::WHILE_END;
            default:
                return Token::OTHER;
            }
            return Token::OTHER;
        }
        return Token::END;
    }

}
