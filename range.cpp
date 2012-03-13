/*
 * range.cpp
 * Copyright © 2012 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <iostream>
#include <list>

#include "tools.hpp"
#include "tuple.hpp"

#include "cxxabi.cpp"
#define TN(x) typeName<decltype(x)>()

// make a range referencing a little C style array.
template <typename T>
struct ArrayRange {
	ArrayRange(T* a, T* e): _b(a), _e(e) {}
	ArrayRange(T* a, size_t s): _b(a), _e(a + s - 1) {}

	template <typename U>
		ArrayRange(ArrayRange<U> o): _b(o._b), _e(o._e) {}

	bool empty() const { return _b > _e; }
	void pop_front() { ++_b; }
	T& front() const { return *_b; }
	void pop_back() { --_e; }
	T& back() const { return *_e; }

	T& at(size_t idx) { return _b[idx]; }

	size_t length() const { return _e - _b; };

	T* _b;
	T* _e;
};

template <typename T>
ArrayRange<typename array_info<T>::type> arange(T& a) {
	return ArrayRange<typename array_info<T>::type>(a, array_info<T>::value);
};

// constify a range (since its reference by default, you might
// want to force the constness if you are a serious programmer ;)
template <typename R>
struct Constifier {
	Constifier(R r): _r(r) {}

	template <typename U>
		Constifier(Constifier<U> o): _r(o._r) {}

	bool empty() const { return _r.empty(); }
	void pop_front() { _r.pop_front(); }
	typename constit<typename range_info<R>::type>::type front() {
		return _r.front();
	}
	void pop_back() { _r.pop_back(); }
	typename constit<typename range_info<R>::type>::type back() {
		return _r.back();
	}

	R _r;
};

template <typename R>
Constifier<R> constify(R r) { return r; }

// seriously, do I need to explain what it does?
template <typename R>
struct Reverser {
	Reverser(R r): _r(r) {}

	template <typename U>
		Reverser(Reverser<U> o): _r(o._r) {}

	bool empty() const { return _r.empty(); }
	void pop_front() { _r.pop_back(); }
	auto front() -> typename range_info<R>::type {
		return _r.back();
	}
	void pop_back() { _r.pop_front(); }
	auto back() -> typename range_info<R>::type {
		return _r.front();
	}

	R _r;
};

template <typename R>
Reverser<R> reverse(R r) { return r; }

// produce a range of tuple(idx, value)
// inside a for(:) loop, it might be kind of cool.
template <typename R>
struct Enumerator {
	typedef tuple<size_t, typename range_info<R>::type> tuple_t;

	Enumerator(R r): _r(r), _idx(0) {}

	bool empty() const { return _r.empty(); }
	void pop_front() { _r.pop_front(); ++_idx; }
	tuple_t front() {
		return tuple_t(_idx, _r.front());
	}

	R _r;
	size_t _idx;
};

template <typename R>
Enumerator<R> enumerate(R r) { return r; }

// the best part, zip 1 to N ranges!
// extract an item from every ranges and return them as a tuple. Every
// operations are settled down at compile time <3
template <typename... Ranges>
struct Zipper {
	tuple<Ranges...> ranges;

	Zipper(Ranges... rs): ranges(rs...) {}

	// should be inline instantiated in empty(),
	// but gcc complain.
	struct empty_reduce {
		template <typename T2>
			bool operator()(bool a, const T2& range) {
				return a or range.empty();
			}
	};

	bool empty() const {
		return tuple_reduce(empty_reduce(), this->ranges, false);
	}

	struct front_map {
		template <typename T>
			auto operator()(T range) -> decltype(range.front()) {
				return range.front();
			}
	};

	auto front() -> tuple<typename range_info<Ranges>::type...> {
		return tuple<typename range_info<Ranges>::type...>(
				tuple_map_tag, front_map(), this->ranges);
	}

	struct pop_front_foreach {
		template <typename T>
			void operator()(T& range) {
				range.pop_front();
			}
	};

	void pop_front() {
		tuple_foreach(this->ranges, pop_front_foreach());
	}
};

template <typename... Ranges>
Zipper<Ranges...> zip(Ranges... rs) { return Zipper<Ranges...>(rs...); }

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

	std::cout << "= ---" << std::endl;
	return 0;
}
