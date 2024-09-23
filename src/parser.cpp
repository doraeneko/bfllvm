/*
bfllvm
(C) Andreas Gaiser (doraeneko@github.com), 2024
Primitive RD parser for bfllvm.
*/

#include "parser.h"

namespace bfllvm
{

    AST Parser::parse()
    {
        return parse_sequence(false);
    }

    std::shared_ptr<Sequence> Parser::parse_sequence(const bool inner_loop)
    {
        auto result = std::make_shared<Sequence>();

        Token token;
        while ((token = _lexer.get_next()) != Token::END)
        {
            switch (token)
            {
            case Token::PTR_INC:
            {
                result->push_back(std::make_shared<Pointer_Increment>());
                break;
            }
            case Token::PTR_DEC:
            {
                result->push_back(std::make_shared<Pointer_Decrement>());
                break;
            }
            case Token::VAL_INC:
            {
                result->push_back(std::make_shared<Value_Increment>());
                break;
            }
            case Token::VAL_DEC:
            {
                result->push_back(std::make_shared<Value_Decrement>());
                break;
            }
            case Token::PUT_CHAR:
            {
                result->push_back(std::make_shared<Put_Char>());
                break;
            }
            case Token::GET_CHAR:
            {
                result->push_back(std::make_shared<Get_Char>());
                break;
            }
            case Token::WHILE_START:
            {
                // start of while already consumed, parse the inner part.
                auto inner_seq = parse_sequence(true);
                if (inner_seq == nullptr)
                {
                    // error, return; _state has been already set.
                    return nullptr;
                }
                auto while_loop = std::make_shared<While_Loop>(*inner_seq);
                result->push_back(while_loop);
                break;
            }
            case Token::WHILE_END:
            {
                if (inner_loop)
                {
                    return result;
                }
                else
                {
                    _state = "Expected an opening '['";
                    return nullptr;
                }
            }
            case Token::OTHER:
            {
                // just ignore
                break;
            }
            default:
            {
                _state = "unhandled parsing error detected.";
                return nullptr;
            }
            }
        }
        if (token == Token::END && inner_loop)
        {
            _state = "Expected a closing ']'";
            return nullptr;
        }
        return result;
    }
}
