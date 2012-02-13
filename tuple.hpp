/*
 * tuple.hpp
 * Copyright © 2012 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef TUPLE_H
#define TUPLE_H

#include <stdexcept>
#include "variant.hpp"

template <typename... Ts>
struct tuple_range;

template <typename... Ts>
struct tuple {};

template <typename T, typename... Ts>
class tuple<T, Ts...>: private tuple<Ts...> {
	T value;

	template <size_t, typename, typename...>
	friend struct getter;
	template <size_t, typename, typename...>
	friend struct getter_runtime;

	public:
		tuple() = default;

		template <typename U, typename... Us>
		tuple(U&& value, Us&&... us):
			tuple<Ts...>(std::forward<Us>(us)...),
			value(std::forward<U>(value)) {
				static_assert(not (sizeof...(Us) > sizeof...(Ts)),
						"Too much arguments for initialized tuple!");
			}

		tuple_range<T, Ts...> all() const {
			return tuple_range<T, Ts...>(*this);
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

template <size_t I, typename T, typename... Ts>
struct getter_runtime {
	static variant get(size_t idx, const tuple<T, Ts...>& t) {
		if (idx == 0) {
			return variant(t.value);
		}
		return getter_runtime<I-1, Ts...>::get(idx-1, t);
	}
};

template <typename T, typename... Ts>
struct getter_runtime<0, T, Ts...> {
	static variant get(size_t idx, const tuple<T, Ts...>& t) {
		if (idx == 0) {
			return variant(t.value);
		}
		throw std::out_of_range("tuple index out of bound");
	}
};

template <typename... Ts>
variant get_runtime(size_t idx, const tuple<Ts...>& t) {
	return getter_runtime<sizeof...(Ts)-1, Ts...>::get(idx, t);
}

template <typename... Ts>
struct tuple_range {
		typedef tuple<Ts...> tuple_t;

		tuple_range(const tuple_t& t): _t(t) {}

		bool empty() const { return _b > _e; }

		variant front() {
			if (_front.empty())
				_front = get_runtime(_b, _t);
			return _front;
		}

		void pop_front() {
			_front = variant();
			++_b;
		}

		variant back() {
			if (_back.empty())
				_back = get_runtime(_e, _t);
			return _back;
		}

		void pop_back() {
			_front = variant();
			--_b;
		}

		size_t length() const { return sizeof...(Ts); }

		variant at(size_t idx) const { return get_runtime(idx, _t); }

	private:
		const tuple_t& _t;

		size_t _b = 0;
		size_t _e = sizeof...(Ts) -1;

		variant _front;
		variant _back;
};

template <typename... Ts>
tuple<Ts...> make_tuple(Ts&&... values) {
	return tuple<Ts...>(std::forward<Ts>(values)...);
}

#endif /* TUPLE_H */
