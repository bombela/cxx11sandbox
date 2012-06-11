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

// stride
// one day, implement stride algo.

// a little verbose class, just to check move semantic application.
struct V {
	int id;
	V(int id): id(id) {
		log() << " ctor" << std::endl;
	}
	~V() {
		log() << " dtor" << std::endl;
	}
	V(const V& from): id(from.id) {
		log() << " copy " << from << std::endl;
	}
	V(V&& from): id(from.id) {
		from.id = -1;
		log() << " move " << from << std::endl;
	}
	V& operator=(V from) {
		id = from.id;
		log() << " assigned " << from << std::endl;
		return *this;
	}
	V& operator=(V&& from) {
		std::swap(id, from.id);
		log() << " move assigned " << from << std::endl;
		return *this;
	}

	std::ostream& log() const {
		return std::cout << "# " << *this;
	}

	friend std::ostream& operator<<(std::ostream& os, const V& v) {
		os << "V(" << v.id << ")";
		return os;
	}
};


	struct {
		template <typename T>
			T operator()(T v) { return v * 2; }
	} op;

int main()
{
	std::cout << std::boolalpha << std::endl;

	int a[] = { 1, 2, 3, 4, 5 };
	auto b = arange(a);
	static_assert(is_forward_range<decltype(b)>::value,
			"is_forward_range");
	static_assert(is_bidirectional_range<decltype(b)>::value,
			"is_bidirectional_range");
	static_assert(is_random_range<decltype(b)>::value,
			"is_random_range");
	static_assert(is_finite_range<decltype(b)>::value,
			"is_finite_range");

	std::cout << "---" << std::endl;
	while (not b.empty()) {
		std::cout << b.front() << std::endl;
		b.pop_front();
	}

	std::cout << "---" << std::endl;
	for (auto e: arange(a)) {
		std::cout << e << std::endl;
	}
	std::cout << "---" << std::endl;
	for (auto e: reverse(arange(a))) {
		std::cout << e << std::endl;
	}
	std::cout << "---" << std::endl;
	tuple<int, double, const char*> t(42, 21.1, "super tuple");

	for (auto e: t.all()) {
		std::cout << e << std::endl;
	}
	std::cout << t << std::endl;

	std::cout << "n ---" << std::endl;

	for (auto e: make_tuple(2, 'a', 22.3).all()) {
		std::cout << e << std::endl;
	}

	V aa[] = { 1, 2, 3, 4 };
	std::cout << "e ---" << std::endl;
	for (auto e: enumerate(arange(aa))) {
		std::cout << e << " /" << get<1>(e) << std::endl;
		get<1>(e).id += 10;
	}
	std::cout << "e2 ---" << std::endl;
	for (auto e: enumerate(arange(aa))) {
		std::cout << e << " /" << get<1>(e) << std::endl;
	}

	std::cout << "z ---" << std::endl;
	auto r = zip(arange(a), reverse(arange(a)));
	std::cout << r.empty() << std::endl;
	std::cout << r.front() << std::endl;
	r.pop_front();

	for (auto e: r) {
		std::cout << e << std::endl;
	}

	int a2[] = { 11, 12, 13, 14, 15, 16, 17 };
	std::cout << "z2 ---" << std::endl;
	for (auto e: enumerate(zip(arange(a), arange(a2)))) {
		std::cout << e << std::endl;
	}

	std::cout << "# ---" << std::endl;
	const int v = 3;
	auto t2 = make_tuple(v, make_tuple(2, 2.2));
	std::cout << t2 << " " << TN(t2) << std::endl;
	std::cout << static_cast<tuple<tuple<int, double>>&>(t2) << std::endl;
	std::cout << "/ ---" << std::endl;
	auto t3 = tuple_map(op, make_tuple(2, 3.3));
	std::cout << t3 << " " << TN(t3) << std::endl;
	std::cout << "= ---" << std::endl;
	return 0;
}
