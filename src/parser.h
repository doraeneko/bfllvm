/*
bfllvm
(C) Andreas Gaiser (doraeneko@github.com), 2024
Primitive RD parser for bfllvm.
*/

#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"

namespace bfllvm {

class Parser {
  Lexer _lexer;
  std::string _state;
  // parse a sequence of commands. If inner_loop is true,
  // a "]" is expected to be eventually read as sequence terminator.
  std::shared_ptr<Sequence> parse_sequence(const bool inner_loop);

public:
  Parser(std::istream &stream) : _lexer(stream), _state{"ok"} {}

  // Try to parse the program given by the initially provided stream.
  // If parsing is successful, an AST value != nullptr is returned;
  // otherwise nulllptr is returned and an error can be read from the status
  AST parse();

  inline std::string state() { return _state; }

  virtual ~Parser() = default;
};

} // namespace bfllvm

#endif
