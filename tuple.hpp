/*
 * tuple.hpp
 * Copyright © 2012 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef TUPLE_H
#define TUPLE_H

#include "variant.hpp"

template <size_t I, typename T, typename... Ts>
struct getter;

template <typename... Ts>
struct tuple {};

template <typename T, typename... Ts>
class tuple<T, Ts...>: private tuple<Ts...> {
	T value;

	template <size_t, typename, typename...>
	friend struct getter;

	public:
		tuple() = default;

		template <typename U, typename... Us>
		tuple(U&& value, Us&&... us):
			tuple<Ts...>(std::forward<Us>(us)...),
			value(std::forward<U>(value)) {
				static_assert(not (sizeof...(Us) > sizeof...(Ts)),
						"Too much arguments for initialized tuple!");
			}
};

template <size_t I, typename T, typename... Ts>
struct tuple_elem {
	static_assert(I-1 < sizeof...(Ts), "index out of bound in tuple!");
	typedef typename tuple_elem<I-1, Ts...>::type type;
};

template <typename T, typename... Ts>
struct tuple_elem<0, T, Ts...> {
	typedef T type;
};

template <size_t I, typename T, typename... Ts>
struct getter {
	static typename tuple_elem<I-1, Ts...>::type get(tuple<T, Ts...>& t) {
		return getter<I-1, Ts...>::get(t);
	}
};

template <typename T, typename... Ts>
struct getter<0, T, Ts...> {
	static T get(tuple<T, Ts...>& t) {
		return t.value;
	}
};

template <size_t I, typename... Ts>
typename tuple_elem<I, Ts...>::type get(tuple<Ts...>& t) {
	return getter<I, Ts...>::get(t);
}


#endif /* TUPLE_H */
