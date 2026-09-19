#include <string>
#include <regex>
#include <iostream>

#include "Set.h"
#include "RPN.h"
#include "Token.h"
#include "tokenize.h"
#include "Sets.h"

#include <windows.h>

void print_help() {
    std::cout <<
    "╔══════════════════════════════════════════════════════════╗\n"
    "║            КАЛЬКУЛЯТОР МНОЖЕСТВ — СПРАВКА                ║\n"
    "╠══════════════════════════════════════════════════════════╣\n"
    "║                                                          ║\n"
    "║  ПРИСВАИВАНИЕ:                                           ║\n"
    "║    A = {1, 2, 3}          явное множество                ║\n"
    "║    B = {-5, 0, 10}        отрицательные числа            ║\n"
    "║    C = {}                 пустое множество               ║\n"
    "║                                                          ║\n"
    "║  ОПЕРАЦИИ:                                               ║\n"
    "║    A + B                  объединение                    ║\n"
    "║    A * B                  пересечение                    ║\n"
    "║    A - B   (или A \\ B)    разность                      ║\n"
    "║    A ^ B                  симметрическая разность        ║\n"
    "║    ~A      (или !A)       дополнение до универсума       ║\n"
    "║                                                          ║\n"
    "║  СКОБКИ:  ( )  или  [ ]                                  ║\n"
    "║                                                          ║\n"
    "║  ФУНКЦИИ:                                                ║\n"
    "║    random(n)              n случайных элементов          ║\n"
    "║    range(stop)            {0, 1, ..., stop-1}            ║\n"
    "║    range(start, stop)     {start, ..., stop-1}           ║\n"
    "║    range(start, stop, step)  с заданным шагом            ║\n"
    "║                                                          ║\n"
    "║  МНОЖЕСТВО ПО УСЛОВИЮ:                                   ║\n"
    "║    {x | x > 5}                                           ║\n"
    "║    {x | x in A and x < 10}                               ║\n"
    "║    {x | x % 3 = 0}        (делимость)                    ║\n"
    "║    Операторы условий: >, <, >=, <=, in, and(&), or(|)    ║\n"
    "║                                                          ║\n"
    "║  УНИВЕРСУМ:  U = {-30, -29, ..., 29, 30}                 ║\n"
    "║  Все элементы должны быть в диапазоне [-30, 30].         ║\n"
    "║                                                          ║\n"
    "║  КОМАНДЫ:                                                ║\n"
    "║    stop                   выход из программы             ║\n"
    "║                                                          ║\n"
    "║  ПРИМЕРЫ:                                                ║\n"
    "║    A = {1, 2, 3} + {3, 4, 5}                             ║\n"
    "║    B = ~A * range(0, 10)                                 ║\n"
    "║    C = {x | x in A or x > 5}                             ║\n"
    "║    D = (A + B) ^ random(5)                               ║\n"
    "║                                                          ║\n"
    "╚══════════════════════════════════════════════════════════╝\n"
    << std::endl;
}

int main() {
    print_help();
	
	SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
	
    const std::regex creature_Set(R"(^\s*([a-zA-Z]+)\s*=\s*)");
    std::smatch match;
	
    Sets sets;
	
    std::string command;
    
    while (true) {
		
        std::cout << ">> ";
        if (!std::getline(std::cin, command)) {
            break;
        }
        if (command == "stop") { break; }
        
        try {
			
            if (std::regex_search(command, match, creature_Set)) {
				
                std::string varname = match[1];
                size_t pos = command.find('=');
                if (pos != std::string::npos) {
                    command = command.substr(pos + 1);
                }
                sets.add_set(varname, infixToRPN(tokenize(command)));
            } else {
				
                auto rpn = infixToRPN(tokenize(command));
                Sets temp_sets = sets;
                temp_sets.add_set("__temp__", rpn);
                temp_sets.sets["__temp__"].print();
            }
			
            sets.print();
        } catch (const std::exception& e) {
            std::cerr << "Ошибка: " << e.what() << std::endl;
        }
    }
    return 0;
}