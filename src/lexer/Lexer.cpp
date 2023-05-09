//
// Created by Даник 💪 on 06.05.2023.
//

#include "Lexer.h"
Lexer::Lexer(std::string contents) : i(0), contents(contents) {
  character = contents[i];
}

void Lexer::NextChar() {
  if (character != '\0' && i < contents.size()) {
    i++;
    character = contents[i];
  }
}
void Lexer::SkipWhitespace() {
  while (isspace(character)) {
    NextChar();
  }
}
Token Lexer::GetNextToken() {
  while (character != '\0' && i < contents.size()) {
    SkipWhitespace();
    if (character == '\0') break;

    if (isalnum(character) || character == '_') {
      return ParseToken();
    }

    if (character == '\'') {
      return ParseString();
    }
    switch (character) {
      case '=': return SaveCharAndGoNext(TOKEN_EQUALS);
      case '(': return SaveCharAndGoNext(TOKEN_LBRACE);
      case ')': return SaveCharAndGoNext(TOKEN_RBRACE);
      case ';': return SaveCharAndGoNext(TOKEN_SEMI);
      case ',': return SaveCharAndGoNext(TOKEN_COMMA);
    }
    return SaveCharAndGoNext(TOKEN_UNKNOWN);
  }
  return {TOKEN_END, ""};
//  throw std::runtime_error("Undefined character");
}

Token Lexer::ParseToken() {
  std::string token;
  while (isalnum(character) || character == '_') {
    token += character;
    NextChar();
  }
  return {TOKEN_KEYWORD, token};
}
Token Lexer::ParseString() {
  std::string token;

  NextChar();
  while (character != '\'') {
    token += character;
    NextChar();
  }
  NextChar();

  return {TOKEN_STRING, token};
}
Token Lexer::SaveCharAndGoNext(TokenTypes token_type) {
  char character_ = character;
  NextChar();
  return {token_type, std::string(1, character_)};
}
