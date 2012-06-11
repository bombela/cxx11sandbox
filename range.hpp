/*
 * range.hpp
 * Copyright © 2012 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef RANGE_H
#define RANGE_H

#include "tools.hpp"
#include "tuple.hpp"

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

#endif /* RANGE_H */
