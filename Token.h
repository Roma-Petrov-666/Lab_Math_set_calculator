#pragma once

#include <string>
#include <variant>

#include "Set.h"

enum class TokenType {
    T_SET,        // множество
    T_OPERATOR,   // Оператор (+, -, *, ^, ~)
    T_LPAREN,     // Левая скобка
    T_RPAREN,     // Правая скобка
};

struct Token {
    TokenType type;
    std::variant<Set, std::string> value;

    Token(TokenType t, const std::string& s) : type(t), value(s) {}
    Token(TokenType t, const Set& s) : type(t), value(s) {}
};