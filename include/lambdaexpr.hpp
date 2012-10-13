/*
 * lambdaexpr.hpp
 * Copyright © 2012 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef LAMBDAEXPR_H
#define LAMBDAEXPR_H

#include <tuple.hpp>

struct expression_tag;

template <typename S>
struct expression;

template <typename V>
struct value_expression;

template <typename A, typename B, typename F>
struct binary_expression;

template <typename A, typename F>
struct unary_expression;

template <typename T>
struct is_an_expression_tag {
	static const bool value = false;
};

template <>
struct is_an_expression_tag<expression_tag> {
	static const bool value = true;
};

template <typename T>
struct is_an_expression {
	typedef char yes;
	struct no { yes _[2]; };

	template <typename U, typename = typename std::enable_if<
			is_an_expression_tag<typename U::tag>::value
			>::type>
		static yes check(U*);

	static no check(...);

	static const bool value = sizeof check((
				typename std::remove_reference<T>::type *)0) == sizeof (yes);
};

template <typename T, typename = typename std::enable_if<
	is_an_expression<T>::value
	>::type>
T&& make_expression(T&& expr) {
	return std::forward<T>(expr);
}

template <typename T, typename = typename std::enable_if<
	not is_an_expression<T>::value
	>::type>
value_expression<T> make_expression(T&& v) {
	return {std::forward<T>(v)};
}

template <typename F, typename A, typename B>
auto make_binary_expression(A&& a, B&& b)
	-> binary_expression<typename std::remove_reference<A>::type,
	typename std::remove_reference<B>::type, F> {
	return {std::forward<A>(a), std::forward<B>(b), F()};
}

template <typename F, typename A>
auto make_unary_expression(A&& a)
	-> unary_expression<typename std::remove_reference<A>::type, F> {
	return {std::forward<A>(a), F()};
}

namespace ops {

struct assign {
	template <typename A, typename B>
		auto operator()(A& a, B&& b) const
		-> decltype(a = std::forward<B>(b)) { return a = std::forward<B>(b); }
};

struct subscript {
	template <typename A, typename B>
		auto operator()(A&& a, B&& b) const
		-> decltype(std::forward<A>(a)[std::forward<B>(b)]) {
			return std::forward<A>(a)[std::forward<B>(b)];
		}
};

#define DEFINE_TMPL_BINARY_OPERATOR(_name, _op) \
struct _name { \
	template <typename A, typename B> \
		auto operator()(A&& a, B&& b) const \
		-> decltype(std::forward<A>(a) _op std::forward<B>(b)) { \
			return std::forward<A>(a) _op std::forward<B>(b); } \
};

DEFINE_TMPL_BINARY_OPERATOR(add, +)
DEFINE_TMPL_BINARY_OPERATOR(sub, -)
DEFINE_TMPL_BINARY_OPERATOR(mul, *)
DEFINE_TMPL_BINARY_OPERATOR(div, /)
DEFINE_TMPL_BINARY_OPERATOR(mod, %)
DEFINE_TMPL_BINARY_OPERATOR(bitand_, &)
DEFINE_TMPL_BINARY_OPERATOR(bitor_, |)
DEFINE_TMPL_BINARY_OPERATOR(xor_, ^)
DEFINE_TMPL_BINARY_OPERATOR(lshift, <<)
DEFINE_TMPL_BINARY_OPERATOR(rshift, >>)
DEFINE_TMPL_BINARY_OPERATOR(eq, ==)
DEFINE_TMPL_BINARY_OPERATOR(neq, !=)
DEFINE_TMPL_BINARY_OPERATOR(gt, >)
DEFINE_TMPL_BINARY_OPERATOR(lt, <)
DEFINE_TMPL_BINARY_OPERATOR(ge, >=)
DEFINE_TMPL_BINARY_OPERATOR(le, <=)
DEFINE_TMPL_BINARY_OPERATOR(and_, and)
DEFINE_TMPL_BINARY_OPERATOR(or_, or)
DEFINE_TMPL_BINARY_OPERATOR(mptr_of, ->*)

#undef DEFINE_TMPL_BINARY_OPERATOR

#define DEFINE_TMPL_BINARY_COMPOUND_OPERATOR(_name, _op) \
struct _name { \
	template <typename A, typename B> \
		auto operator()(A&& a, B&& b) const \
		-> decltype(std::forward<A>(a) _op std::forward<B>(b)) { \
			return std::forward<A>(a) _op std::forward<B>(b); } \
};

DEFINE_TMPL_BINARY_COMPOUND_OPERATOR(cadd, +=)
DEFINE_TMPL_BINARY_COMPOUND_OPERATOR(csub, -=)
DEFINE_TMPL_BINARY_COMPOUND_OPERATOR(cmul, *=)
DEFINE_TMPL_BINARY_COMPOUND_OPERATOR(cdiv, /=)
DEFINE_TMPL_BINARY_COMPOUND_OPERATOR(cmod, %=)
DEFINE_TMPL_BINARY_COMPOUND_OPERATOR(cbitand_, &=)
DEFINE_TMPL_BINARY_COMPOUND_OPERATOR(cbitor_, |=)
DEFINE_TMPL_BINARY_COMPOUND_OPERATOR(cxor_, ^=)
DEFINE_TMPL_BINARY_COMPOUND_OPERATOR(clshift, <<=)
DEFINE_TMPL_BINARY_COMPOUND_OPERATOR(crshift, >>=)

#undef DEFINE_TMPL_BINARY_COMPOUND_OPERATOR

#define DEFINE_TMPL_UNARY_OPERATOR(_name, _op) \
struct _name { \
	template <typename A> \
		auto operator()(A&& a) const \
		-> decltype(_op std::forward<A>(a)) { return _op std::forward<A>(a); } \
};

DEFINE_TMPL_UNARY_OPERATOR(addr_of, &)
DEFINE_TMPL_UNARY_OPERATOR(indir, *)
DEFINE_TMPL_UNARY_OPERATOR(plus, +)
DEFINE_TMPL_UNARY_OPERATOR(moins, -)
DEFINE_TMPL_UNARY_OPERATOR(inc, ++)
DEFINE_TMPL_UNARY_OPERATOR(dec, --)
DEFINE_TMPL_UNARY_OPERATOR(not_, !)
DEFINE_TMPL_UNARY_OPERATOR(bitnot_, ~)

#undef DEFINE_TMPL_UNARY_OPERATOR

#define DEFINE_TMPL_UNARY_POST_OPERATOR(_name, _op) \
struct _name { \
	template <typename A> \
		auto operator()(A&& a) const \
		-> decltype(std::forward<A>(a) _op) { return std::forward<A>(a) _op; } \
};

DEFINE_TMPL_UNARY_POST_OPERATOR(postinc, ++)
DEFINE_TMPL_UNARY_POST_OPERATOR(postdec, --)

#undef DEFINE_TMPL_UNARY_POST_OPERATOR


} // namespace ops

template <typename S>
struct expression {
	typedef expression_tag tag;

	template <typename B>
		auto operator=(B&& b)
		-> decltype(
				make_binary_expression<ops::assign>(
					*(S*)0, make_expression(std::forward<B>(b))
					)
				) {
		return
				make_binary_expression<ops::assign>(
					static_cast<S&>(*this), make_expression(std::forward<B>(b))
					)
				;
	}

	template <typename B>
		auto operator[](B&& b)
		-> decltype(
				make_binary_expression<ops::subscript>(
					*(S*)0, make_expression(std::forward<B>(b))
					)
				) {
		return
				make_binary_expression<ops::subscript>(
					static_cast<S&>(*this), make_expression(std::forward<B>(b))
					)
				;
	}

	template <typename E, typename... ARGS>
		struct _eval_type {
			typedef decltype(((S*)0)->_eval(std::declval<E>(),
						std::forward<ARGS>(std::declval<ARGS>())...
						)) type;
		};

	template <typename... ARGS>
		auto operator()(ARGS&&... args)
			-> typename _eval_type<S, ARGS...>::type {
			return static_cast<S&>(*this)._eval(static_cast<S&>(*this),
					std::forward<ARGS>(args)...);
		}
};

#define DEFINE_BINARY_OPERATOR(_op, _op_functor) \
template <typename A, typename B, \
		typename = typename std::enable_if< \
				is_an_expression<A>::value or is_an_expression<B>::value \
			>::type \
	> \
	auto _op(A&& a, B&& b) \
	-> decltype(make_binary_expression<_op_functor>( \
				make_expression(std::forward<A>(a)), \
				make_expression(std::forward<B>(b)))) { \
	return make_binary_expression<_op_functor>( \
			make_expression(std::forward<A>(a)), \
			make_expression(std::forward<B>(b))); \
}

DEFINE_BINARY_OPERATOR(operator +  , ops::add);
DEFINE_BINARY_OPERATOR(operator -  , ops::sub);
DEFINE_BINARY_OPERATOR(operator *  , ops::mul);
DEFINE_BINARY_OPERATOR(operator /  , ops::div);
DEFINE_BINARY_OPERATOR(operator %  , ops::mod);
DEFINE_BINARY_OPERATOR(operator &  , ops::bitand_);
DEFINE_BINARY_OPERATOR(operator |  , ops::bitor_);
DEFINE_BINARY_OPERATOR(operator ^  , ops::xor_);
DEFINE_BINARY_OPERATOR(operator << , ops::lshift);
DEFINE_BINARY_OPERATOR(operator >> , ops::rshift);
DEFINE_BINARY_OPERATOR(operator += , ops::cadd);
DEFINE_BINARY_OPERATOR(operator -= , ops::csub);
DEFINE_BINARY_OPERATOR(operator *= , ops::cmul);
DEFINE_BINARY_OPERATOR(operator /= , ops::cdiv);
DEFINE_BINARY_OPERATOR(operator %= , ops::cmod);
DEFINE_BINARY_OPERATOR(operator &= , ops::cbitand_);
DEFINE_BINARY_OPERATOR(operator |= , ops::cbitor_);
DEFINE_BINARY_OPERATOR(operator ^= , ops::cxor_);
DEFINE_BINARY_OPERATOR(operator <<=, ops::clshift);
DEFINE_BINARY_OPERATOR(operator >>=, ops::crshift);
DEFINE_BINARY_OPERATOR(operator == , ops::eq);
DEFINE_BINARY_OPERATOR(operator != , ops::neq);
DEFINE_BINARY_OPERATOR(operator >  , ops::gt);
DEFINE_BINARY_OPERATOR(operator <  , ops::lt);
DEFINE_BINARY_OPERATOR(operator >= , ops::ge);
DEFINE_BINARY_OPERATOR(operator <= , ops::le);
DEFINE_BINARY_OPERATOR(operator && , ops::and_);
DEFINE_BINARY_OPERATOR(operator || , ops::or_);
DEFINE_BINARY_OPERATOR(operator ->* , ops::mptr_of);

#undef DEFINE_BINARY_OPERATOR

#define DEFINE_UNARY_OPERATOR(_op, _op_functor) \
template <typename A, typename = typename std::enable_if< \
	is_an_expression<A>::value>::type \
	> \
	auto _op(A&& a) \
	-> decltype(make_unary_expression<_op_functor>(std::forward<A>(a))) { \
	return make_unary_expression<_op_functor>(std::forward<A>(a)); \
} \

DEFINE_UNARY_OPERATOR(operator &, ops::addr_of)
DEFINE_UNARY_OPERATOR(operator *, ops::indir)
DEFINE_UNARY_OPERATOR(operator +, ops::plus)
DEFINE_UNARY_OPERATOR(operator -, ops::moins)
DEFINE_UNARY_OPERATOR(operator ++, ops::inc)
DEFINE_UNARY_OPERATOR(operator --, ops::dec)
DEFINE_UNARY_OPERATOR(operator ! , ops::not_)
DEFINE_UNARY_OPERATOR(operator ~ , ops::bitnot_)

#undef DEFINE_UNARY_OPERATOR

#define DEFINE_UNARY_POST_OPERATOR(_op, _op_functor) \
template <typename A, typename = typename std::enable_if< \
	is_an_expression<A>::value>::type \
	> \
	auto _op(A&& a, int) \
	-> decltype(make_unary_expression<_op_functor>(std::forward<A>(a))) { \
	return make_unary_expression<_op_functor>(std::forward<A>(a)); \
} \

DEFINE_UNARY_POST_OPERATOR(operator ++, ops::postinc)
DEFINE_UNARY_POST_OPERATOR(operator --, ops::postdec)

#undef DEFINE_UNARY_POST_OPERATOR

// A value_expression<V> should aways be instantiated trough make_expression().
// In the deducted type context of make_expression(),
// V == rvalue, and V& == lvalue.
template <typename V>
struct value_expression: expression<value_expression<V>> {
	V&& _v;

	value_expression(const value_expression& from): _v(std::move(from._v)) {}
	value_expression(value_expression&& from): _v(std::move(from._v)) {}

	value_expression(V&& v): _v(std::move(v)) {}

	template <typename E, typename... ARGS>
	V&& _eval(const E&, ARGS&&...) const {
		return std::move(_v);
	}

	template <int I>
		struct arg_cnt {
			constexpr static size_t value = 0;
		};
};

// Here the specialization for V&. In this case, we handle everything that is
// not an rvalue expression.
template <typename V>
struct value_expression<V&>: expression<value_expression<V>> {
	const V& _v;

	value_expression(const V& v): _v(v) {}

	template <typename E, typename... ARGS>
	const V& _eval(const E&, ARGS&&...) const {
		return _v;
	}

	template <int I>
		struct arg_cnt {
			constexpr static size_t value = 0;
		};
};

template <typename A, typename B, typename F>
struct binary_expression: expression<binary_expression<A, B, F>> {
	A _a;
	B _b;
	F _f;

	binary_expression(const A& a, const B& b, F f): _a(a), _b(b), _f(f) {}

	template <typename E, typename... ARGS>
		auto _eval(const E& eroot, ARGS&&... args)
			-> decltype(_f(
						_a._eval(eroot, std::forward<ARGS>(args)...),
						_b._eval(eroot, std::forward<ARGS>(args)...))
					) {
			return _f(
					_a._eval(eroot, std::forward<ARGS>(args)...),
					_b._eval(eroot, std::forward<ARGS>(args)...)
					);
		}

	template <int I>
		struct arg_cnt {
			constexpr static size_t value = A::template arg_cnt<I>::value
				+ B::template arg_cnt<I>::value;
		};

	using expression<binary_expression<A, B, F>>::operator=;
};

template <typename A, typename F>
struct unary_expression: expression<unary_expression<A, F>> {
	A _a;
	F _f;

	unary_expression(const A& a, F f): _a(a), _f(f) {}

	template <typename E, typename... ARGS>
		auto _eval(const E& eroot, ARGS&&... args)
			-> decltype(_f(_a._eval(eroot, std::forward<ARGS>(args)...))) {
			return _f(_a._eval(eroot, std::forward<ARGS>(args)...));
		}

	template <int I>
		struct arg_cnt {
			constexpr static size_t value = A::template arg_cnt<I>::value;
		};

	using expression<unary_expression<A, F>>::operator=;
};

template <int I>
struct arg_expression: expression<arg_expression<I>> {

	template <typename E, typename... ARGS>
		struct return_type {

			// T& -> T&, const T& -> const T&, T&& -> T&&
			typedef decltype(get<I>(std::declval< tuple<ARGS&&...> >())
					) uval_type;
			// T& -> T&, const T& -> const T&, T&& -> const T&
			typedef decltype(get<I>(std::declval< tuple<const ARGS&...> >())
					) lval_type;

			typedef typename std::conditional<
				// if in the whole expression currently being evaluated,
				// starting from the root "E", there is only one use of the
				// argument #I, then we can safely apply an universal reference
				// to this argument. In other cases (the argument #I is used
				// more than once), we will use an lvalue reference.
				// The reason of all of that, is you want perfect forwarding on
				// all arguments used only one time, while you surely do not
				// want any "double move" on all arguments used more than once
				// in the expression. Ex:
				//  - (_1 + _2)(42, 3.14) => both _1 and _2 can be treated as
				//  rvalue references.
				//  - (_1 + _2 + _1)(42, 3.14) => _1 cannot be treated as an
				//  rvalue anymore, because the corresponding argument would
				//  then potentially be moved twice. _2 however can be treated
				//  as an rvalue.
				//  If the argument is initially an lvalue, all this complex
				//  setup will simply lead to an lvalue reference (due to the
				//  reference collapsing rules of C++11).
					E::template arg_cnt<I>::value == 1,
					uval_type, lval_type>::type type;
		};

	template <typename E, typename... ARGS>
	auto _eval(const E&, ARGS&&... args)
		-> typename return_type<E, ARGS...>::type {
		return get<I>(tuple<ARGS&&...>(std::forward<ARGS>(args)...));
	}

	template <int J>
		struct arg_cnt {
			constexpr static size_t value = (J == I) ? 1 : 0;
		};

	using expression<arg_expression<I>>::operator=;
};

arg_expression<0> _1;
arg_expression<1> _2;
arg_expression<2> _3;
arg_expression<3> _4;
arg_expression<4> _5;
arg_expression<5> _6;
arg_expression<6> _7;
arg_expression<7> _8;
arg_expression<8> _9;

#endif /* LAMBDAEXPR_H */
