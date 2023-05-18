//
// Created by Даник 💪 on 06.05.2023.
//

#include "Lexer.h"
#include "../structures/Errors.h"
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
      Token token = ParseToken();
      if (token.value == "IS") {
        return ParseIsStatement();
      } else {
        return token;
      }
    }

    if (character == '\'') {
      return ParseString();
    }
    switch (character) {
      case '=': return SaveCharAndGoNext(TOKEN_EQUALS);
      case '<': {
        NextChar();
        if (character == '=') {
          NextChar();
          return {TOKEN_SMALLER_EQUALS, "<="};
        } else if (isspace(character)) {
          return {TOKEN_SMALLER, "<"};
        }
      }
      case '!': {
        NextChar();
        if (character == '=') {
          return SaveCharAndGoNext(TOKEN_NOT_EQUALS);
        }
      }
      case '(': return SaveCharAndGoNext(TOKEN_LBRACE);
      case ')': return SaveCharAndGoNext(TOKEN_RBRACE);
      case ';': return SaveCharAndGoNext(TOKEN_SEMI);
      case ',': return SaveCharAndGoNext(TOKEN_COMMA);
      case '>': {
        NextChar();
        if (character == '=') {
          NextChar();
          return {TOKEN_BIGGER_EQUALS, ">="};
        } else if (isspace(character)) {
          return {TOKEN_BIGGER, ">"};
        }
      }
    }
    return SaveCharAndGoNext(TOKEN_UNKNOWN);
  }
  return {TOKEN_END, ""};
}

Token Lexer::ParseToken() {
  std::string token;
  while (isalnum(character) || character == '_' || character == '.') {
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
Token Lexer::ParseIsStatement() {
  SkipWhitespace();
  Token token = ParseToken();

  if (character == '\0') throw SQLError(SYNTAX_ERROR);
  if (token.value == "NULL") {
    return {TOKEN_IS_NULL, "IS NULL"};
  } else if (token.value == "NOT"){
    SkipWhitespace();
    token = ParseToken();
    if (token.value == "NULL") {
      return {TOKEN_IS_NOT_NULL, "IS NOT NULL"};
    }
  }
  throw SQLError(SYNTAX_ERROR);
}
