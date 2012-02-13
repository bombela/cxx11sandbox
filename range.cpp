
#include <iostream>
#include <list>

#include "tools.hpp"
#include "tuple.hpp"

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

int main()
{
	std::cout << std::boolalpha << std::endl;

	int a[] = { 1, 2, 3, 4, 5 };
	auto b = arange(a);
	std::cout << is_forward_range<decltype(b)>::value << std::endl;
	std::cout << is_bidirectional_range<decltype(b)>::value << std::endl;
	std::cout << is_random_range<decltype(b)>::value << std::endl;
	std::cout << is_finite_range<decltype(b)>::value << std::endl;

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

	std::cout << "e ---" << std::endl;
	for (auto e: enumerate(b)) {
		std::cout << e << std::endl;
	}
	return 0;
}
