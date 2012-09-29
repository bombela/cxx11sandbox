/*
 * range.cpp
 * Copyright © 2012 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <iostream>
#include <list>

#include "tools.hpp"
#include "tuple.hpp"
#include "range.hpp"

#include "cxxabi.cpp"
#define TN(x) typeName<decltype(x)>()

template <typename T>
void test(T b, T e, T s) {
	std::cout << "-> range(" << b << ", " << e << ", " << s << ")" << std::endl;
	{
		std::cout << "------- range" << std::endl;
		for (auto x: range(b, e, s)) {
			std::cout << x << " | ";
		}
		std::cout << std::endl;
	}
	{
		std::cout << "------- range[]" << std::endl;
		auto r = range(b, e, s);
		for (auto i: range(0, r.size())) {
			std::cout << i << ": " << r[i] << " | ";
		}
		std::cout << std::endl;
	}
	{
		std::cout << "------- neg range" << std::endl;
		for (auto x: range(-b, -e, s)) {
			std::cout << x << " | ";
		}
		std::cout << std::endl;
	}
	{
		std::cout << "------- neg range[]" << std::endl;
		auto r = range(-b, -e, s);
		for (auto i: range(0, r.size())) {
			std::cout << i << ": " << r[i] << " | ";
		}
		std::cout << std::endl;
	}
	{
		std::cout << "------- range - to +, step" << std::endl;
		for (auto x: range(-b, +e, s)) {
			std::cout << x << " | ";
		}
		std::cout << std::endl;
	}
	{
		std::cout << "------- range + to -, step" << std::endl;
		for (auto x: range(+b, -e, s)) {
			std::cout << x << " | ";
		}
		std::cout << std::endl;
	}
	{
		std::cout << "------- range - to +, -step" << std::endl;
		for (auto x: range(-b, +e, -s)) {
			std::cout << x << " | ";
		}
		std::cout << std::endl;
	}
	{
		std::cout << "------- range + to -, -step" << std::endl;
		for (auto x: range(+b, -e, -s)) {
			std::cout << x << " | ";
		}
		std::cout << std::endl;
	}
}

int main()
{
	std::cout << std::boolalpha;
	std::cout << "r1,10 ---" << std::endl;

	test(1, 4, 1);
	test(1.0, 2.0, 0.3);
}
