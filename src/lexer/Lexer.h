//
// Created by Даник 💪 on 06.05.2023.
//

#ifndef LABWORK_12_KATSUSHOOTER_SRC_LEXER_LEXER_H_
#define LABWORK_12_KATSUSHOOTER_SRC_LEXER_LEXER_H_

#include <string>
#include "Token.h"

class Lexer {
 public:
  explicit Lexer(std::string contents);
  Token GetNextToken();
 public:

 private:
  char character;
  unsigned int i;
  std::string contents;

  void NextChar();
  void SkipWhitespace();
  Token ParseToken();
  Token ParseString();
  Token SaveCharAndGoNext(TokenTypes token_type);
  Token ParseIsStatement();
};

#endif //LABWORK_12_KATSUSHOOTER_SRC_LEXER_LEXER_H_
