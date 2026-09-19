#include <regex>
#include <string>
#include <vector>
#include <stdexcept>
#include <map>

#include "tokenize.h"
#include "Token.h"
#include "Set.h"

std::string normalize(const std::string& op) {
	
    switch(op[0]) {
        case '-' :
        case '\\' : return "-";
        case '~' :
        case '!' : return "~";
        case '(' :
        case '[' : return "(";
        case ')' :
        case ']' : return ")";
    }
	
    return op;
}

std::vector<Token> tokenize(const std::string& str) {
	
    std::vector<Token> tokens;
	
    static const std::regex token_regex(
        R"((\{\s*(?:-?\d+(?:\s*,\s*-?\d+)*)?\s*\}))"					// 1 - множество в явном виде
        R"(|random\s*\(\s*(\d+)\s*\))"									// 2 - множество заполненное случайными элементами
        R"(|range\s*\(\s*((?:-?\d+\s*(?:,\s*-?\d+\s*){0,2})?)\s*\))"	// 3 - множество как диапазон
        R"(|([a-zA-Z]+))"												// 4 - множество как переменная
        R"(|\{\s*([a-zA-Z]+)\s*\|(.*?)\})"								// 5,6 - множество как условие
        R"(|([+*\\^~!-]))"                          					// 7 - операторы
        R"(|(\(|\[))"                               					// 8 - левая скобка
        R"(|(\)|\]))"                               					// 9 - правая скобка
    );

    auto begin = std::sregex_iterator(str.begin(), str.end(), token_regex);
    auto end = std::sregex_iterator();
    
    size_t lastPos = 0; // Позиция конца последнего обработанного токена

    for (auto it = begin; it != end; ++it) {
		std::smatch match = *it;

		if (match.position() > lastPos) {
			std::string gap = str.substr(lastPos, match.position() - lastPos);

			if (gap.find_first_not_of(" \t\n\r\f\v") != std::string::npos) {
				throw std::runtime_error("Неизвестный синтаксис: \"" + gap + "\"");
			}
		}
		
		lastPos = match.position() + match.length();

        std::string expression = match.str();
        
        // Определяем, какая группа совпала
        if (match[1].matched) { // явное множество
            std::vector<int> numbers;
            if (!expression.empty()) {
                static const std::regex numberPattern(R"(-?\d+)");
                auto begin_ = std::sregex_iterator(expression.begin(), expression.end(), numberPattern);
                auto end_   = std::sregex_iterator();
                for (auto it_ = begin_; it_ != end_; ++it_) {
                    numbers.push_back(std::stoi(it_->str()));
                }
            }
            tokens.push_back(Token{TokenType::T_SET, Set(numbers)});
        }
        else if (match[2].matched) { // random
            tokens.push_back(Token{TokenType::T_SET, Set::random(std::stoi(match[2].str()))});
        }
        else if (match[3].matched) { // range(...)
            std::string argsStr = match[3].str();
            if (argsStr.empty()) {
                throw std::invalid_argument("range(): требуется от 1 до 3 аргументов");
            }
            std::vector<int> args;
            static const std::regex numberPattern(R"(-?\d+)");
            auto begin_ = std::sregex_iterator(argsStr.begin(), argsStr.end(), numberPattern);
            auto end_   = std::sregex_iterator();
            for (auto it_ = begin_; it_ != end_; ++it_) {
                args.push_back(std::stoi(it_->str()));
            }
            if (args.size() == 1) {
                tokens.push_back(Token{TokenType::T_SET, Set::range(args[0])});
            } else if (args.size() == 2) {
                tokens.push_back(Token{TokenType::T_SET, Set::range(args[0], args[1])});
            } else if (args.size() == 3) {
                tokens.push_back(Token{TokenType::T_SET, Set::range(args[0], args[1], args[2])});
            } else {
                throw std::invalid_argument("range(): требуется от 1 до 3 аргументов");
            }
        }
        else if (match[4].matched) { // переменная
            tokens.push_back(Token{TokenType::T_SET, expression});
        }
        else if (match[5].matched) { // условие
            std::vector<Token> tmp = condition_to_expression(match[6].str(), match[5].str());
            tokens.insert(tokens.end(), tmp.begin(), tmp.end());
        }
        else if (match[7].matched) { // операторы
            tokens.push_back(Token{TokenType::T_OPERATOR, normalize(expression)});
        }
        else if (match[8].matched) { // левая скобка
            tokens.push_back(Token{TokenType::T_LPAREN, normalize(expression)});
        }
        else if (match[9].matched) { // правая скобка
            tokens.push_back(Token{TokenType::T_RPAREN, normalize(expression)});
        }
    }

    // Проверка "хвоста" строки после последнего токена
    if (lastPos < str.length()) {
		std::string tail = str.substr(lastPos);

		if (tail.find_first_not_of(" \t\n\r\f\v") != std::string::npos) {
			throw std::runtime_error("Неизвестный синтаксис: \"" + tail + "\"");
		}
	}
	
	return tokens;
}

std::vector<Token> condition_to_expression(const std::string& str, const std::string& varname) {
    std::string result = str;

    std::vector<std::pair<std::string, std::string>> pattern {
        std::pair<std::string, std::string>(R"(\b)" + varname + R"(\b\s*in\s*)", R"()"),
        std::pair<std::string, std::string>(R"(\band\b|&)", R"(*)"),
        std::pair<std::string, std::string>(R"(\bor\b|\|)", R"(+)"),
        std::pair<std::string, std::string>(R"(\b)" + varname + R"(\b\s*%\s*(-?\d+)\s*=\s*(-?\d+))", R"((range($2, 31, $1) + range($2, -31, -$1)))"),
        std::pair<std::string, std::string>(R"(\b)" + varname + R"(\b\s*>=\s*(-?\d+))", R"((range($1, 31)))"),
        std::pair<std::string, std::string>(R"(\b)" + varname + R"(\b\s*<=\s*(-?\d+))", R"((range(-30, $1) + {$1}))"),
        std::pair<std::string, std::string>(R"(\b)" + varname + R"(\b\s*>\s*(-?\d+))", R"((range($1, 31) - {$1}))"),
        std::pair<std::string, std::string>(R"(\b)" + varname + R"(\b\s*<\s*(-?\d+))", R"((range(-30, $1)))")
    };
    
    for (const auto& item : pattern) {
        result = std::regex_replace(result, std::regex(item.first), item.second);
    }
    return tokenize(result);
}