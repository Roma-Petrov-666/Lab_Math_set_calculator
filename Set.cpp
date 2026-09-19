#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>

#include "Set.h"

std::vector<int> Set::delete_duplicate(std::vector<int> num) {
    num.erase(std::unique(num.begin(), num.end()), num.end());
    return num;
}

Set::Set(std::vector<int> num) {
    std::sort(num.begin(), num.end());
    num = delete_duplicate(num);

    if (num.size() > 61) {
        throw std::invalid_argument("Set: Каждый элемент должен быть уникальным (max 61)");
    }
    for (const auto& item : num) {
        if (item > 30 || item < -30) {
            throw std::invalid_argument("Set: Каждый элемент должен быть в универсуме");
        }
    }

    values = num;
}

Set Set::random(int n) {
    if (n < 0 || n > 61) {
        throw std::invalid_argument("random(n): n должно быть в диапазоне [0, 61]");
    }
    std::vector<int> values;
    while ((int)values.size() < n) {
        int tmp = (std::rand() % 61) - 30;
        if (std::find(values.begin(), values.end(), tmp) == values.end()) {
            values.push_back(tmp);
        }
    }
    return Set(values);
}

Set Set::range(int stop) {
    return range(0, stop, 1);
}

Set Set::range(int start, int stop) {
    return range(start, stop, 1);
}

Set Set::range(int start, int stop, int step) {
    if (step == 0) {
        throw std::invalid_argument("range(): шаг не может быть равен 0");
    }
    std::vector<int> values;
    if (step > 0) {
        for (int i = start; i < stop; i += step) {
            values.push_back(i);
        }
    } else {
        for (int i = start; i > stop; i += step) {
            values.push_back(i);
        }
    }
    return Set(values);
}

std::vector<int> Set::get_values() const {
    return values;
}

void Set::set_values(std::vector<int> num) {
    std::sort(num.begin(), num.end());
    num = delete_duplicate(num);

    if (num.size() > 61) {
        throw std::invalid_argument("Set: Каждый элемент должен быть уникальным (max 61)");
    }
    for (const auto& item : num) {
        if (item > 30 || item < -30) {
            throw std::invalid_argument("Set: Каждый элемент должен быть в универсуме");
        }
    }

    values = num;
}

void Set::print() const {
    std::cout << "{ ";
    if (!values.empty()) {
        std::cout << values[0];
        for (size_t i = 1; i < values.size(); ++i) {
            std::cout << ", " << values[i];
        }
    }
    std::cout << " }" << std::endl;
}


// Объединение (+)
Set operator+(const Set& lhs, const Set& rhs) {
    std::vector<int> res = lhs.get_values();
    std::vector<int> rvs = rhs.get_values();
    res.insert(res.end(), rvs.begin(), rvs.end());
    return Set(res);
}

// Пересечение (*)
Set operator*(const Set& lhs, const Set& rhs) {
    std::vector<int> res;
    std::vector<int> lvs = lhs.get_values();
    std::vector<int> rvs = rhs.get_values();
    for (const auto& item : lvs) {
        if (std::binary_search(rvs.begin(), rvs.end(), item)) {
            res.push_back(item);
        }
    }
    return Set(res);
}

// Разность (/)
Set operator-(const Set& lhs, const Set& rhs) {
    std::vector<int> res;
    std::vector<int> lvs = lhs.get_values();
    std::vector<int> rvs = rhs.get_values();
    for (const auto& item : lvs) {
        if (!std::binary_search(rvs.begin(), rvs.end(), item)) {
            res.push_back(item);
        }
    }
    return Set(res);
}

// Симметрическая разность(^)
Set operator^(const Set& lhs, const Set& rhs) {
    return (lhs - rhs) + (rhs - lhs);
}

// Дополнение до универсума (-30..30)
Set operator~(const Set& s) {
    std::vector<int> res;
    std::vector<int> lvs = s.get_values();
    for (int i = -30; i < 31; ++i) {
        if (!std::binary_search(lvs.begin(), lvs.end(), i)) {
            res.push_back(i);
        }
    }
    return Set(res);
}