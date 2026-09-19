#pragma once

#include <map>
#include <string>
#include <iostream>
#include <stack>
#include <vector>
#include <stdexcept>

#include "Set.h"
#include "Token.h"

struct Sets {
	std::map<std::string, Set> sets;
	
    static Set create_universe() {
        std::vector<int> values;
        for (int i = -30; i <= 30; ++i) {
            values.push_back(i);
        }
        return Set(values);
    }

    Sets() {
        sets["U"] = create_universe();
    }
	
	void add_set(const std::string& varname, const std::vector<Token>& rpn) {
		
		if (varname == "U") {
            throw std::runtime_error("Имя 'U' зарезервировано для универсума");
        }
		
		std::stack<Token> evalStack;
            for (const auto& token : rpn) {
				
                if (token.type == TokenType::T_SET) {
					
                    if (std::holds_alternative<std::string>(token.value)) {
						
						std::string varName = std::get<std::string>(token.value);
						auto it = sets.find(varName);
						if (it == sets.end()) {
							
							throw std::runtime_error("Переменная '" + varName + "' не определена");
						}
						evalStack.push(Token(TokenType::T_SET, it->second));
					} else {
						
						evalStack.push(token); // уже готовое множество
					}
                }
                else if (token.type == TokenType::T_OPERATOR) {
					
                    std::string op = std::get<std::string>(token.value);

                    if (op == "~") {
                        // Унарный оператор дополнения
                        if (evalStack.empty()) {
                            throw std::runtime_error("Недостаточно операндов для унарного оператора '~'");
                        }
                        Token top = evalStack.top();
                        evalStack.pop();
                        if (top.type != TokenType::T_SET) {
                            throw std::runtime_error("Операнд для '~' должен быть множеством");
                        }
                        Set operand = std::get<Set>(top.value);
                        Set result = ~operand;
                        evalStack.push(Token(TokenType::T_SET, result));
                    }
                    else {
                        // Бинарные операторы: +, *, -, ^
                        if (evalStack.size() < 2) {
                            throw std::runtime_error("Недостаточно операндов для бинарного оператора '" + op + "'");
                        }
                        Token right = evalStack.top();
                        evalStack.pop();
                        Token left = evalStack.top();
                        evalStack.pop();
                        if (left.type != TokenType::T_SET || right.type != TokenType::T_SET) {
                            throw std::runtime_error("Операнды для бинарного оператора должны быть множествами");
                        }
                        Set lhs = std::get<Set>(left.value);
                        Set rhs = std::get<Set>(right.value);
                        Set result;

                        if (op == "+") {
                            result = lhs + rhs;
                        }
                        else if (op == "*") {
                            result = lhs * rhs;
                        }
                        else if (op == "-") {
                            result = lhs - rhs;
                        }
                        else if (op == "^") {
                            result = lhs ^ rhs;
                        }
                        else {
                            throw std::runtime_error("Неизвестный бинарный оператор: " + op);
                        }

                        evalStack.push(Token(TokenType::T_SET, result));
                    }
                }
                else {
                    throw std::runtime_error("Недопустимый токен в RPN");
                }
            }

            if (evalStack.size() != 1) {
                throw std::runtime_error("Некорректное выражение: в стеке осталось " +
                                         std::to_string(evalStack.size()) + " элементов");
            }

            Token resultToken = evalStack.top();
            if (resultToken.type != TokenType::T_SET) {
                throw std::runtime_error("Результат вычисления не является множеством");
            }

            Set finalSet = std::get<Set>(resultToken.value);
            sets.insert_or_assign(varname, finalSet);
	}
	
	void print() {
		std::cout << "Инициализированные множества: " << std::endl;
		for (const auto& item : sets) {
			if (item.first == "U") continue;
			std::cout << item.first << " = ";
			item.second.print();
		}
	}
};