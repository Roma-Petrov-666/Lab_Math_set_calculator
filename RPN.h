#pragma once

#include <string>
#include <vector>

#include "Token.h"

int getPrecedence(const std::string& op);

bool isLeftAssociative(const std::string& op);

std::vector<Token> infixToRPN(const std::vector<Token>& tokens);