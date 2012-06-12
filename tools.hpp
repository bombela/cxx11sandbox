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


// yes I know, macro is ugly and bad.
// but, good luck doing the same black magic without it.

#define DEF_IS_EXPR(__name, __expr, __and) \
struct __name { \
	typedef char yes; \
	struct no { char _[2]; }; \
\
	template <size_t> struct swallow {}; \
\
	template <typename U> \
	static yes check(U*, swallow<(( sizeof __expr ))>* = 0); \
	static no check(...); \
\
	static const bool value = \
		((sizeof check((T*)0) == sizeof (yes)) and (__and)); \
};

template <typename T>
DEF_IS_EXPR(is_forward_range, (
		sizeof bool((*(U*)0).empty()),
		sizeof (*(U*)0).front(),
		sizeof (*(U*)0).pop_front()
		), true)

template <typename T>
DEF_IS_EXPR(is_bidirectional_range, (
		sizeof (*(U*)0).back(),
		sizeof (*(U*)0).pop_back()
		), is_forward_range<T>::value)

template <typename T>
DEF_IS_EXPR(is_random_range, (
		sizeof (*(U*)0).at(42)
		), is_forward_range<T>::value)

template <typename T>
DEF_IS_EXPR(is_finite_range, (
		sizeof size_t((*(U*)0).length())
		), is_forward_range<T>::value)

template <typename R>
struct range_info {
	typedef decltype(((R*)0)->front()) type;
};

// its not a real iterator, just something that
// make possible the for(:) statement on ranges.
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
typename std::enable_if<
	is_forward_range<R>::value,
	range_iterator<R>
>::type begin(R& r) {
	return range_iterator<R>(r);
}

template <typename R>
typename std::enable_if<
	is_forward_range<R>::value,
	range_iterator<R>
>::type end(R& r) {
	return range_iterator<R>(r);
}

#endif /* TOOLS_H */
