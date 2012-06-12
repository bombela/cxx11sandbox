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


template <typename R>
bool any(R r) {
	for (auto x: r) {
		if (bool(x)) {
			return true;
		}
	}
	return false;
}

template <typename R>
bool all(R r) {
	for (auto x: r) {
		if (not bool(x)) {
			return false;
		}
	}
	return true;
}


int main()
{
	std::cout << std::boolalpha;
	std::cout << "r1,10 ---" << std::endl;

	auto r = range(1, 10);

	std::cout << TN(r) << " " << r.length() << std::endl;
	for (auto x: r) {
		std::cout << TN(x) << " " << x << std::endl;
	}

	std::cout << "map ---" << std::endl;
	for (auto x: map([](int x) { return x / 2; }, range(10))) {
		std::cout << x << std::endl;
	}

	std::cout << "any1 ---" << std::endl;
	std::cout << any(range(0)) << std::endl;

	std::cout << "any2 ---" << std::endl;
	std::cout << any(range(10)) << std::endl;

	std::cout << "any3 ---" << std::endl;
	std::cout << any(map([](int x) { return x < 9; }, range(9))) << std::endl;

	std::cout << "any4 ---" << std::endl;
	std::cout << any(map([](int x) { return x < 9; }, range(9, 100))) << std::endl;

	std::cout << "all1 ---" << std::endl;
	std::cout << all(range(0)) << std::endl;

	std::cout << "all2 ---" << std::endl;
	std::cout << all(range(10)) << std::endl;

	std::cout << "all3 ---" << std::endl;
	std::cout << all(map([](int x) { return x < 9; }, range(9))) << std::endl;

	std::cout << "all4 ---" << std::endl;
	std::cout << all(map([](int x) { return x < 9; }, range(9, 10))) << std::endl;
}
