/*
 * tuple.hpp
 * Copyright © 2012 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef TUPLE_H
#define TUPLE_H

template <typename... Ts>
struct tuple {};

template <typename T, typename... Ts>
struct tuple<T, Ts...>: private tuple<Ts...> {
	T head;

	tuple() = default;

	template <typename U, typename... Us>
	tuple(U&& head, Us&&... us):
		head(std::forward(head)),
		tuple<Ts...>(std::forward(us)...) {}
};

#endif /* TUPLE_H */
