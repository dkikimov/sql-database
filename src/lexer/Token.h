//
// Created by Даник 💪 on 06.05.2023.
//

#ifndef LABWORK_12_KATSUSHOOTER_SRC_LEXER_TOKEN_H_
#define LABWORK_12_KATSUSHOOTER_SRC_LEXER_TOKEN_H_

#include <string>

enum TokenTypes{
  TOKEN_STRING,
  TOKEN_RBRACE,
  TOKEN_LBRACE,
  TOKEN_SEMI,
  TOKEN_KEYWORD,
  TOKEN_COMMA,
  TOKEN_EQUALS,
  TOKEN_END,
  TOKEN_UNKNOWN
};

struct Token {
  Token(int type, std::string value);

  TokenTypes type;
  std::string value;
};

#endif //LABWORK_12_KATSUSHOOTER_SRC_LEXER_TOKEN_H_