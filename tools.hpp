/*
 * tools.hpp
 * Copyright © 2012 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef TOOLS_H
#define TOOLS_H

template <typename A>
struct array_info;

template <typename A, size_t S>
struct array_info<A[S]> {
	static const size_t value = S;
	typedef A type;
};

template <typename T>
struct constit {
	typedef const T type;
};

template <typename T>
struct constit<T&> {
	typedef const T& type;
};

template <typename T>
struct constit<T*> {
	typedef const T* type;
};

#define DEF_IS_EXPR(__name, __expr) \
struct __name { \
	typedef char yes; \
	struct no { char _[2]; }; \
\
	template <size_t> struct swallow {}; \
\
	template <typename U> \
	static yes check(U*, swallow<sizeof (( __expr ))>* = 0); \
	static no check(...); \
\
	static const bool value = sizeof check((T*)0) == sizeof (yes); \
};

template <typename T>
DEF_IS_EXPR(is_forward_range, (*(T*)0).front() )

template <typename R>
struct range_info {
	typedef decltype(((R*)0)->front()) type;
};

template <bool C, typename T=void>
struct enable_if {};

template <typename T>
struct enable_if<true, T> { typedef T type; };

template <typename R>
struct range_iterator {
	R& r;
	range_iterator(R& r): r(r) {}

	range_iterator& operator++() {
		r.pop_front();
		return *this;
	}

	bool operator==(const range_iterator&) const { return r.empty(); }
	bool operator!=(const range_iterator&) const { return not r.empty(); }
	typename range_info<R>::type operator*() {
		return r.front();
	}
};

template <typename R>
typename enable_if<
	is_forward_range<R>::value,
	range_iterator<R>
>::type begin(R& r) {
	return range_iterator<R>(r);
}

template <typename R>
typename enable_if<
	is_forward_range<R>::value,
	range_iterator<R>
>::type end(R& r) {
	return range_iterator<R>(r);
}

#endif /* TOOLS_H */
