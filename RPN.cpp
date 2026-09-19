#include <string>
#include <stack>
#include <stdexcept>
#include <vector>

#include "RPN.h"
#include "Token.h"

// Функция получения приоритета оператора
int getPrecedence(const std::string& op) {
    if (op == "~") {
        return 2;
    }
    return 1;
}

// Функция проверки ассоциативности 
bool isLeftAssociative(const std::string& op) {
    return op != "~";
}

std::vector<Token> infixToRPN(const std::vector<Token>& tokens) {
    std::vector<Token> output;
    std::stack<Token> opStack;

    for (const auto& token : tokens) {
        if (token.type == TokenType::T_SET) {
            output.push_back(token);
        }
        else if (token.type == TokenType::T_LPAREN) {

            opStack.push(token);
        }
        else if (token.type == TokenType::T_RPAREN) {

            while (!opStack.empty() && opStack.top().type != TokenType::T_LPAREN) {
                output.push_back(opStack.top());
                opStack.pop();
            }
            
            if (!opStack.empty() && opStack.top().type == TokenType::T_LPAREN) {
                opStack.pop();
            } else {
                throw std::runtime_error("Ошибка: несоответствие скобок (лишняя правая скобка)");
            }
        }
        else if (token.type == TokenType::T_OPERATOR) {

            while (!opStack.empty() && opStack.top().type == TokenType::T_OPERATOR) {
                int precCurrent = getPrecedence(std::get<std::string>(token.value));
                int precTop = getPrecedence(std::get<std::string>(opStack.top().value));

                bool shouldPop = false;
                if (isLeftAssociative(std::get<std::string>(token.value)) && precCurrent <= precTop) {
                    shouldPop = true;
                } else if (!isLeftAssociative(std::get<std::string>(token.value)) && precCurrent < precTop) {
                    shouldPop = true;
                }

                if (shouldPop) {
                    output.push_back(opStack.top());
                    opStack.pop();
                } else {
                    break;
                }
            }

            opStack.push(token);
        }
    }

    while (!opStack.empty()) {
        if (opStack.top().type == TokenType::T_LPAREN || opStack.top().type == TokenType::T_RPAREN) {
            throw std::runtime_error("Ошибка: несоответствие скобок (забытая левая скобка)");
        }
        output.push_back(opStack.top());
        opStack.pop();
    }

    return output;
}

