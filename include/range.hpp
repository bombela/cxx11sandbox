/*
 * range.hpp
 * Copyright © 2012 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef RANGE_H
#define RANGE_H

#include <stdexcept>

#include "tools.hpp"
#include "tuple.hpp"

// a simple number range
template <typename T>
struct NumberRange {
	constexpr NumberRange(T begin, T end, T step): _b(begin), _e(end),
		_s(begin <= end or step < 0 ? step : -step) {}
	constexpr NumberRange(NumberRange&& r) = default;
	NumberRange(const NumberRange&) = default;

	NumberRange& operator=(NumberRange from) = delete;
	NumberRange& operator=(NumberRange&& from) = delete;

	bool empty() const {
		if (_s > 0)
			return _b >= _e;
		return _b <= _e;
	}

	void pop_front() { _b += _s; }
	T front() const { return _b; }

	void pop_back() { _e += _s; }
	T back() const { return _e; }

	size_t size() const {
		const T fract_size = (_e - _b) / _s;
		return (fract_size - size_t(fract_size)) > 0 ?
			 fract_size + 1 : fract_size;
	};

	T operator[](size_t idx) {
		const T v = _b + idx * _s;
		if ((_s > 0) ? (v >= _e) : (v <= _e)) {
			throw std::overflow_error("out of bound");
		}
		return v;
	}

	T _b, _e;
	T const _s;
};

template <typename S = int, typename B, typename E>
constexpr auto range(B begin, E end, S step = 1)
	-> NumberRange<decltype(begin + end + step)> {
	return {begin, end, step};
}

template <typename E>
constexpr auto range(E end)
	-> decltype(range(0, end)) {
	return range(0, end);
}

// make a range referencing a little C style array.
template <typename T>
struct ArrayRange {
	ArrayRange(T* a, T* e): _b(a), _e(e) {}
	ArrayRange(T* a, size_t s): _b(a), _e(a + s - 1) {}

	bool empty() const { return _b > _e; }
	void pop_front() { ++_b; }
	T& front() const { return *_b; }
	void pop_back() { --_e; }
	T& back() const { return *_e; }

	T& operator[](size_t idx) { return _b[idx]; }

	size_t size() const { return _e - _b; };

	T* _b;
	T* _e;
};

template <typename T>
ArrayRange<typename array_info<T>::type> arange(T& a) {
	return {a, array_info<T>::size};
};

// constify a range (add a const qualifier to front & back)
template <typename R>
struct Constifier: public R {
	Constifier(R r): R(r) {}

	typename constit<typename range_info<R>::type>::type front() {
		return R::front();
	}
	typename constit<typename range_info<R>::type>::type back() {
		return R::back();
	}
	typename constit<typename range_info<R>::type>::type operator[](size_t idx) {
		return R::operator[](idx);
	}
};

template <typename R>
Constifier<R> constify(R r) { return r; }

// seriously, do I need to explain what it does?
template <typename R>
struct Reverser: public R {
	Reverser(R r): R(r) {}

	void pop_front() { R::pop_back(); }
	auto front() -> typename range_info<R>::type {
		return R::back();
	}
	void pop_back() { R::pop_front(); }
	auto back() -> typename range_info<R>::type {
		return R::front();
	}
	auto operator[](size_t idx) -> typename range_info<R>::type {
		return R::operator[](R::size() - 1 - idx);
	}
};

template <typename R>
Reverser<R> reverse(R r) { return r; }

// produce a range of tuple(idx, value)
// inside a for(:) loop, it might be kind of useful.
template <typename R>
struct Enumerator: public R {
	typedef tuple<size_t, typename range_info<R>::type> tuple_t;

	Enumerator(R r, size_t start): R(r), _idx(start) {}

	void pop_front() { R::pop_front(); ++_idx; }
	tuple_t front() {
		return tuple_t(_idx, R::front());
	}
	void pop_back() { R::pop_back(); }
	tuple_t back() {
		return tuple_t(R::size() - _idx, R::front());
	}

	tuple_t operator[](size_t idx) {
		return R::operator[](R::size() - 1 - idx);
	}

	size_t _idx;
};

template <typename R>
Enumerator<R> enumerate(R r, size_t start=0) { return {r, start}; }

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

	// should be inline instantiated in front(),
	// but gcc complain.
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

	// should be inline instantiated in pop_front(),
	// but gcc complain.
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


template <typename FV, typename... Ranges>
struct LongZipper {
	typedef FV const& fillval_t;
	// because of the C++11 reference collapsing rules, and
	// the fact that const cannot be applied to a reference:
	// T& => T&, T&& => const T&, const T& => const T&
	fillval_t fillval;
	tuple<Ranges...> ranges;

	template <typename T>
		struct range_type {
			// the constness of fillval is reported to the type returned by the
			// range.
			typedef typename range_info<T>::type _range_type;
			typedef typename
				std::conditional<
					// if
					std::is_const<
						typename std::remove_reference<fillval_t>::type
					>::value and std::is_reference<_range_type>::value,
					// then
					typename std::remove_reference<_range_type>::type const&,
					// else
					typename range_info<T>::type
				>::type type;
		};

	typedef tuple<typename range_type<Ranges>::type...> return_type;

	LongZipper(FV&& fillval, Ranges... rs):
		fillval(std::forward<FV>(fillval)), ranges(rs...) {}

	struct empty_reduce {
		template <typename T2>
			bool operator()(bool a, const T2& range) {
				return a and range.empty();
			}
	};

	bool empty() const {
		return tuple_reduce(empty_reduce(), this->ranges, true);
	}

	struct front_map {
		FV const& fillval;
		template <typename T>
			auto operator()(T range)
			-> typename range_type<T>::type {
				return range.empty() ? fillval : range.front();
			}
	};

	return_type front() {
		return return_type(tuple_map_tag, front_map{fillval}, ranges);
	}

	struct pop_front_foreach {
		template <typename T>
			void operator()(T& range) {
				if (not range.empty()) {
					range.pop_front();
				}
			}
	};

	void pop_front() {
		tuple_foreach(this->ranges, pop_front_foreach());
	}
};

//template <typename FV, typename... Ranges>
//LongZipper<FV, Ranges...> longzip(const FV& fillval, Ranges... rs) {
//    return LongZipper<FV, Ranges...>(fillval, rs...);
//}

template <typename FV, typename... Ranges>
LongZipper<FV, Ranges...> longzip(FV&& fillval, Ranges... rs) {
	return LongZipper<FV, Ranges...>(std::forward<FV>(fillval), rs...);
}


// map a function to a range
template <typename F, typename R>
struct Mapper {
	typedef decltype( (*(F*)0)((*(R*)0).front()) ) return_t;

	Mapper(F f, R r): _f(f), _r(r) {}

	bool empty() const { return _r.empty(); }
	void pop_front() { _r.pop_front(); }
	return_t front() {
		return _f(_r.front());
	}

	F _f;
	R _r;
};

template <typename F, typename R>
Mapper<F, R> map(F f, R r) { return {f, r}; }

#endif /* RANGE_H */
