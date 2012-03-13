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

struct tuple_map_tag_t {} tuple_map_tag;

template <typename... Ts>
struct tuple_range;

template <typename... Ts>
struct tuple;

template <>
struct tuple<> {
	private:
		// special case, make everything private because
		// its the end of recursion, and nobody should
		// be able to create an empty tuple by mistake.

		template <typename...>
			friend struct tuple;

		explicit tuple() {}

		// yes there is a definition of an empty tuple,
		// but that avoid having a special tuple definition
		// for one item :D
		template <typename F>
			explicit tuple(tuple_map_tag_t, F, tuple<>) {}
};

// note that this is not a tuple as defined by the C++11 standard.
// there is either more funny feature, and either less safe ones.
// the C++11 make_tuple doesn't bind reference by default for example.
template <typename T, typename... Ts>
struct tuple<T, Ts...>: tuple<Ts...> {

		T _p_value; // everything is public (first for gcc whining), and
		// anyway, making this attribute public as well as inheriting in public
		// make everything simply far easier. Who care about diving into some
		// private implementation details shit anyway?
		// who care if somebody can cast the poor tuple and break everything!

		tuple() = default;

		// some magic with rvalues my friend.
		template <typename U, typename... Us,
				 // this little enable_if is the tricks
				 // to avoid interference with second constructor.
				 typename = typename std::enable_if<
					 sizeof... (Ts) == sizeof... (Us)
					 >::type>
			explicit tuple(U&& value, Us&&... values):
				tuple<Ts...>(std::forward<Us>(values)...),
				_p_value(std::forward<U>(value)) {
				}

		// this constructor copy a tuple by passing element trough some functor.
		template <typename F, typename U, typename... Us>
			explicit tuple(tuple_map_tag_t, F f, const tuple<U, Us...>& t):
				tuple<Ts...>(tuple_map_tag, f,
						static_cast<const tuple<Us...>&>(t)
						),
				_p_value(f(t._p_value)) {
				}

		// a little optimization for an rvalue tuple :)
		// if the functor support perfect forwarding, you win.
		template <typename F, typename U, typename... Us>
			explicit tuple(tuple_map_tag_t, F f, tuple<U, Us...>&& t):
				tuple<Ts...>(tuple_map_tag, f,
						static_cast<tuple<Us...>&&>(t)
						),
				_p_value(f(std::move(t._p_value))) {
				}

		~tuple() = default;
		tuple(const tuple&) = default;
		tuple(tuple&&) = default;
		tuple& operator=(const tuple&) = default;
		tuple& operator=(tuple&&) = default;

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

template <typename F, typename T, typename... Ts>
struct tuple_foreach_impl {
	static void apply(tuple<T, Ts...>& t, F f) {
		f(t._p_value);
		tuple_foreach_impl<F, Ts...>::apply(t, f);
	}
	static void apply(const tuple<T, Ts...>& t, F f) {
		f(t._p_value);
		tuple_foreach_impl<F, Ts...>::apply(t, f);
	}
};

template <typename F, typename T>
struct tuple_foreach_impl<F, T> {
	static void apply(tuple<T>& t, F f) {
		f(t._p_value);
	}
	static void apply(const tuple<T>& t, F f) {
		f(t._p_value);
	}
};

template <typename... Ts, typename F>
void tuple_foreach(tuple<Ts...>& t, F f) {
	tuple_foreach_impl<F, Ts...>::apply(t, f);
}

template <typename... Ts, typename F>
void tuple_foreach(const tuple<Ts...>& t, F f) {
	tuple_foreach_impl<F, Ts...>::apply(t, f);
}

template <typename F, typename T, typename... Ts>
struct tuple_reduce_impl {

	static auto apply(F f, const tuple<T, Ts...>& t) ->
		decltype(tuple_reduce_impl<F, Ts...>::apply(f, t, t._p_value)) {
			return tuple_reduce_impl<F, Ts...>::apply(f, t, t._p_value);
		}

	template <typename V>
		static auto apply(F f, const tuple<T, Ts...>& t, V v) ->
			decltype(tuple_reduce_impl<F, Ts...>::apply(f, t, f(v, t._p_value))) {
			return tuple_reduce_impl<F, Ts...>::apply(f, t, f(v, t._p_value));
		}
};

template <typename F, typename T>
struct tuple_reduce_impl<F, T> {

	static auto apply(F, const tuple<T>& t) ->
		decltype(t._p_value) {
			return t._p_value;
		}

	template <typename V>
		static auto apply(F f, const tuple<T>& t, V v) ->
			decltype(f(v, t._p_value)) {
			return f(v, t._p_value);
		}
};

template <typename F, typename... Ts>
auto tuple_reduce(F f, const tuple<Ts...>& t) ->
	decltype(tuple_reduce_impl<F, Ts...>::apply(f, t)) {
	return tuple_reduce_impl<F, Ts...>::apply(f, t);
}

template <typename F, typename T, typename... Ts>
struct tuple_reduce_impl2 {
	template <typename V>
		static auto apply(F f, const tuple<T, Ts...>& t, V v) ->
			decltype(tuple_reduce_impl2<F, Ts...>::apply(f, t, f(v, t._p_value))) {
			return tuple_reduce_impl2<F, Ts...>::apply(f, t, f(v, t._p_value));
		}
};

template <typename F, typename T>
struct tuple_reduce_impl2<F, T> {
	template <typename V>
		static auto apply(F f, const tuple<T>& t, V v) ->
			decltype(f(v, t._p_value)) {
			return f(v, t._p_value);
		}
};

template <typename F, typename T, typename... Ts>
auto tuple_reduce(F f, const tuple<Ts...>& t, T init) ->
	decltype(tuple_reduce_impl2<F, Ts...>::apply(f, t, init)) {
	return tuple_reduce_impl2<F, Ts...>::apply(f, t, init);
}

template <size_t I, typename T, typename... Ts>
struct getter {
	static typename tuple_elem<I-1, Ts...>::type get(tuple<T, Ts...>& t) {
		return getter<I-1, Ts...>::get(t);
	}
};

template <typename T, typename... Ts>
struct getter<0, T, Ts...> {
	static T get(tuple<T, Ts...>& t) {
		return t._p_value;
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
			return variant(t._p_value);
		}
		return getter_runtime<I-1, Ts...>::get(idx-1, t);
	}
};

template <typename T, typename... Ts>
struct getter_runtime<0, T, Ts...> {
	static variant get(size_t idx, const tuple<T, Ts...>& t) {
		if (idx == 0) {
			return variant(t._p_value);
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
		size_t _e = (sizeof...(Ts) - 1);

		variant _front;
		variant _back;
};

template <typename... Ts>
tuple<Ts...> make_tuple(Ts&&... values) {
	return tuple<Ts...>(std::forward<Ts>(values)...);
}

template <typename F, typename... Ts>
auto tuple_map(F f, const tuple<Ts...>& t) ->
	tuple<typename std::result_of<F (Ts)>::type...> {
		return tuple<typename std::result_of<F (Ts)>::type...>(
				tuple_map_tag, f, t);
}

template <typename F, typename... Ts>
auto tuple_map(F f, tuple<Ts...>&& t) ->
	tuple<typename std::result_of<F (Ts)>::type...> {
		return tuple<typename std::result_of<F (Ts)>::type...>(
				tuple_map_tag, f, std::move(t));
}

struct tuple_elem_printer {
	bool first = true;
	std::ostream& os;
	tuple_elem_printer(std::ostream& os): os(os) {}
	template <typename T>
		void operator()(const T& v) {
			if (first)
				first = false;
			else
				os << ", ";
			os << v;
		}
};

template <typename... Ts>
std::ostream& operator<<(std::ostream& os, const tuple<Ts...>& t)
{
	std::ostream::sentry init(os);
	if (init) {
		os << "tuple(";
		tuple_foreach(t, tuple_elem_printer(os));
		os << ")";
	}
	return os;
}

#endif /* TUPLE_H */
