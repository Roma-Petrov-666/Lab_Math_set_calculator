#pragma once

#include <string>
#include <vector>

#include "Token.h"

std::vector<Token> tokenize(const std::string& str);

std::vector<Token> condition_to_expression(const std::string& str, const std::string& varname);