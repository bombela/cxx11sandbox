/*
 * lambda.cpp
 * Copyright © 2012 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <iostream>

#include "cxxabi.cpp"
#define TN(x) typeName<decltype(x)>()

#include <lambdaexpr.hpp>

struct V {
	int _v;
	V(int v): _v(v) {
		std::cout << "V(" << _v << ") [" << this << "]" << std::endl;
	}
	V(int v, int): _v(v) {
		std::cout << "ret V(" << _v << ") [" << this << "]" << std::endl;
	}
	friend V operator+(const V& a, const V& b) {
		auto r = a._v + b._v;
		std::cout << "V+(" << a._v << ", " << b._v << ") => " << r
			<< " [" << &a << ", " << &b << "]" <<  std::endl;
		return V(r, 42);
	}
	friend V&& operator+(V&& a, V&& b) {
		auto r = a._v + b._v;
		std::cout << "V+(rval(" << a._v << "), rval(" << b._v << ")) => " << r
			<< " [" << &a << ", " << &b << "]" <<  std::endl;
		a._v = r;
		return std::move(a);
	}
	friend V&& operator+(V&& a, const V& b) {
		auto r = a._v + b._v;
		std::cout << "V+(rvalue(" << a._v << "), " << b._v << ") => " << r
			<< " [" << &a << ", " << &b << "]" <<  std::endl;
		a._v = r;
		return std::move(a);
	}
	friend V&& operator+(const V& a, V&& b) {
		auto r = a._v + b._v;
		std::cout << "V+(" << a._v << ", rvalue(" << b._v << ")) => " << r
			<< " [" << &a << ", " << &b << "]" <<  std::endl;
		b._v = r;
		return std::move(b);
	}
	V(const V& b): _v(b._v) {
		std::cout << "V copy(" << _v << ") [" << this <<  ", " << &b <<
			"]" << std::endl;
	}
	V(V&& b): _v(std::move(b._v)) {
		std::cout << "V move(" << _v << ") [" << this << ", " << &b <<
			"]" << std::endl;
	}
	V& operator=(const V& b) {
		std::cout << "V(" << _v << ") = V(" << b._v << ") [" << this <<
			", " << &b << "]" << std::endl;
		_v = b._v;
		return *this;
	}
	V& operator=(V&& b) {
		std::cout << "V(" << _v << ") = move(V(" << b._v << ")) [" << this <<
			", " << &b << "]" << std::endl;
		_v = std::move(b._v);
		return *this;
	}
	V& operator++() {
		auto b = _v++;
		std::cout << "++V(" << b << ") => " << _v << " [" << this <<
			"]" << std::endl;
		return *this;
	}
	V& operator--() {
		auto b = _v--;
		std::cout << "--V(" << b << ") => " << _v << " [" << this <<
			"]" << std::endl;
		return *this;
	}
};


int main()
{
	std::cout << std::boolalpha << std::endl;

	{
		std::cout << "-- test 0" << std::endl;
		auto c = (_1 + _2 + _1);
		std::cout << decltype(c)::arg_cnt<0>::value << std::endl;
		std::cout << decltype(c)::arg_cnt<1>::value << std::endl;
		std::cout << decltype(c)::arg_cnt<2>::value << std::endl;
	}
	{
		std::cout << "-- test 1" << std::endl;
		int v = 42;
		auto a = (_1)(v, 3, 5);
		std::cout << a << std::endl;
	}
	{
		std::cout << "-- test 2" << std::endl;
		int v = 42;
		auto a = ((--_1 + _2) / 3.)(v, 3, 5);
		std::cout << a << std::endl;
		std::cout << v << std::endl;
	}
	{
		std::cout << "-- test 3" << std::endl;
		int v = 42;
		int* p = &v;
		auto a = (*_1 = 2)(p);
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
	{
		std::cout << "-- test 8" << std::endl;
		auto&& a = (_1 + _1)(V{42}, V{2});
		std::cout << a._v << " [" << &a << "]" << std::endl;
	}
	{
		std::cout << "-- test 9" << std::endl;
		auto&& a = (_1 + _2)(V{42}, V{2});
		std::cout << a._v << " [" << &a << "]" << std::endl;
	}
	{
		std::cout << "-- test 10" << std::endl;
		auto&& a = (_1 + _2 + _1)(V{42}, V{2});
		std::cout << a._v << " [" << &a << "]" << std::endl;
	}
	{
		std::cout << "-- test 11" << std::endl;
		V v(2);
		auto&& a = (--_2 + ++_2)(V{42}, v);
		std::cout << a._v << " [" << &a << "]" << std::endl;
	}
	{
		std::cout << "-- test 12" << std::endl;
		const V v(2);
		auto&& a = (_2 + _2)(V{42}, v);
		std::cout << a._v << " [" << &a << "]" << std::endl;
	}
}
