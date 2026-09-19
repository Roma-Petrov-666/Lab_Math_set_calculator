#pragma once

#include <vector>
#include <string>

class Set {
private:
    std::vector<int> values; // элементы в диапазоне (-30, 30)

    std::vector<int> delete_duplicate(std::vector<int> num);

public:
    Set() = default;
    Set(std::vector<int> num);

	static Set random(int n);
    static Set range(int stop);
    static Set range(int start, int stop);
    static Set range(int start, int stop, int step); 

    std::vector<int> get_values() const;
    void set_values(std::vector<int> num);
    void print() const;
};

Set operator+(const Set& lhs, const Set& rhs);   // объединение
Set operator*(const Set& lhs, const Set& rhs);   // пересечение
Set operator-(const Set& lhs, const Set& rhs);   // разность
Set operator^(const Set& lhs, const Set& rhs);   // симметрическая разность
Set operator~(const Set& s);                     // дополнение