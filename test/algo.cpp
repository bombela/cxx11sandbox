/*
 * range.cpp
 * Copyright © 2012 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <iostream>
#include <list>
#include <stdexcept>

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


// reduce a range though a function
template <typename F, typename R>
struct Reducer {
	typedef decltype( (*(F*)0)( (*(R*)0).front(), (*(R*)0).front()) ) return_t;

	Reducer(F f, R r): _f(f), _r(r) {}

	bool empty() const { return _r.empty(); }
	void pop_front() { _r.pop_front(); }
	return_t front() {
		return _f(_r.front());
	}

	F _f;
	R _r;
};

template <typename F, typename R>
auto reduce(F f, R r) -> decltype(f(r.front(), r.front())) {
	if (r.empty()) {
		throw std::range_error("cannot reduce an empty range!");
	}

	decltype(f(r.front(), r.front())) sum = r.front();
	r.pop_front();

	for (auto x: r) {
		sum = f(sum, x);
	}
	return sum;
}

namespace details {

	struct adder {
		template <typename A, typename B>
			auto operator()(const A& a, const B& b) -> decltype(a + b) {
				return a + b;
			}
	};

	struct multiplier {
		template <typename A, typename B>
			auto operator()(const A& a, const B& b) -> decltype(a * b) {
				return a * b;
			}
	};
}

template <typename R>
auto sum(R r) -> decltype(reduce(details::adder(), r)) {
	return reduce(details::adder(), r);
}

template <typename R>
auto product(R r) -> decltype(reduce(details::multiplier(), r)) {
	return reduce(details::multiplier(), r);
}

// filter a range through a function
template <typename F, typename R>
struct Filterer {
	typedef decltype( (*(R*)0).front() ) return_t;

	Filterer(F f, R r): _f(f), _r(r) {
		_discard();
	}

	bool empty() const { return _r.empty(); }
	void pop_front() {
		_r.pop_front();
		_discard();
	}
	return_t front() {
		return _r.front();
	}

	void _discard() {
		for (auto x: _r) {
			if (_f(x)) {
				break;
			}
		}
	}

	F _f;
	R _r;
};

template <typename F, typename R>
Filterer<F, R> filter(F f, R r) { return {f, r}; }

int main()
{
	std::cout << std::boolalpha;
	std::cout << "r1,10 ---" << std::endl;

	auto r = range(1, 10);

	std::cout << TN(r) << " " << r.size() << std::endl;
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

	std::cout << "reduce ---" << std::endl;
	std::cout << reduce([](int a, int b) { return a + b; },
			range(1, 10)) << std::endl;

	std::cout << "reduce (const ref) ---" << std::endl;
	std::cout << reduce([](const int& a, const int& b) { return a + b; },
			range(1, 10)) << std::endl;

	std::cout << "sum ---" << std::endl;
	std::cout << sum(range(1, 10)) << std::endl;

	std::cout << "product ---" << std::endl;
	std::cout << product(range(1, 10)) << std::endl;

	std::cout << "map/reduce ---" << std::endl;
	std::cout << reduce([](const int& a, const int& b) { return a + b; },
			map([](const int& v) { return v * 10; },
				range(1, 10))) << std::endl;

	std::cout << "filter ---" << std::endl;
	for (auto x: filter([](int v){ return v > 5 and v < 8; }, range(1, 10))) {
		std::cout << x << std::endl;
	}
}
