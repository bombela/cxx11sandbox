/*
 * lambda.cpp
 * Copyright © 2012 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <iostream>

#include "cxxabi.cpp"
#define TN(x) typeName<decltype(x)>()

#include <lambdaexpr.hpp>

int main()
{
	std::cout << std::boolalpha << std::endl;
	{
		std::cout << "-- test 1" << std::endl;
		int v = 42;
		auto a = ((--_1 + _2) / 3., _1)(v, 3, 5);
		std::cout << a << std::endl;
	}
	{
		std::cout << "-- test 2" << std::endl;
		int v = 42;
		auto a = (_1 = 2)(v, 3, 5);
		std::cout << a << std::endl;
		std::cout << v << std::endl;
	}
	{
		std::cout << "-- test 3" << std::endl;
		int v = 42;
		int* p = &v;
		auto a = (*_1 = 21)(p);
		std::cout << a << std::endl;
		std::cout << v << std::endl;
	}
	{
		std::cout << "-- test 4" << std::endl;
		auto a = (!_2)(42, 0, 2);
		std::cout << a << std::endl;
	}
	{
		std::cout << "-- test 5" << std::endl;
		int v = 40;
		auto a = (_1 += _2)(v, 2, 3);
		std::cout << a << std::endl;
	}
	{
		std::cout << "-- test 6" << std::endl;
		struct V { int a = 1, b = 2; } v;
		auto a = (_1 ->* &V::a)(&v, 2, 3);
		std::cout << a << std::endl;
	}
	{
		std::cout << "-- test 7" << std::endl;
		int v[] = { 1, 2, 3, 4, 5 };
		auto a = (_1[2] + _1[4] + _3)(v, 6, 7);
		std::cout << a << std::endl;
	}
}
