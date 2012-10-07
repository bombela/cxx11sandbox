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

	static const bool value = sizeof check((T*)0) == sizeof (yes);
};

template <typename T, typename = typename std::enable_if<
	is_an_expression<T>::value
	>::type>
T make_expression(const T& expr) {
	return expr;
}

template <typename T, typename = typename std::enable_if<
	not is_an_expression<T>::value
	>::type>
value_expression<T> make_expression(const T& v) {
	return {v};
}

template <typename F, typename A, typename B>
binary_expression<A, B, F> make_binary_expression(const A& a, const B& b) {
	return {a, b, F()};
}

template <typename F, typename A>
unary_expression<A, F> make_unary_expression(const A& a) {
	return {a, F()};
}

namespace ops {

struct assign { \
	template <typename A, typename B> \
		auto operator()(A& a, const B& b) const \
		-> decltype(a = b) { return a = b; } \
};

struct subscript { \
	template <typename A, typename B> \
		auto operator()(const A& a, const B& b) const \
		-> decltype(a[b]) { return a[b]; } \
};

#define DEFINE_TMPL_BINARY_OPERATOR(_name, _op) \
struct _name { \
	template <typename A, typename B> \
		auto operator()(const A& a, const B& b) const \
		-> decltype(a _op b) { return a _op b; } \
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
		auto operator()(A& a, const B& b) const \
		-> decltype(a _op b) { return a _op b; } \
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
		auto operator()(A& a) const \
		-> decltype(_op a) { return _op a; } \
	template <typename A> \
		auto operator()(const A& a) const \
		-> decltype(_op a) { return _op (a); } \
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
		auto operator()(A& a) const \
		-> decltype(a _op) { return a _op; } \
};

DEFINE_TMPL_UNARY_POST_OPERATOR(postinc, ++)
DEFINE_TMPL_UNARY_POST_OPERATOR(postdec, --)

#undef DEFINE_TMPL_UNARY_POST_OPERATOR


} // namespace ops

template <typename S>
struct expression {
	typedef expression_tag tag;

	template <typename B>
		auto operator=(const B& b)
		-> decltype(
				make_binary_expression<ops::assign>(
					*(S*)0, make_expression(b)
					)
				) {
		return
				make_binary_expression<ops::assign>(
					static_cast<S&>(*this), make_expression(b)
					)
				;
	}

	template <typename B>
		auto operator[](const B& b)
		-> decltype(
				make_binary_expression<ops::subscript>(
					*(S*)0, make_expression(b)
					)
				) {
		return
				make_binary_expression<ops::subscript>(
					static_cast<S&>(*this), make_expression(b)
					)
				;
	}
};

#define DEFINE_BINARY_OPERATOR(_op, _op_functor) \
template <typename A, typename B, \
		typename = typename std::enable_if< \
				is_an_expression<A>::value and not is_an_expression<B>::value \
			>::type \
	> \
	auto _op(const A& a, const B& b) \
	-> decltype(make_binary_expression<_op_functor>(a, make_expression(b))) { \
	return make_binary_expression<_op_functor>(a, make_expression(b)); \
} \
template <typename A, typename B, typename = typename std::enable_if< \
	is_an_expression<B>::value>::type \
	> \
	auto _op(const A& a, const B& b) \
		-> decltype(make_binary_expression<_op_functor>(make_expression(a), b)) { \
		return make_binary_expression<_op_functor>(make_expression(a), b); \
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
	auto _op(const A& a) \
	-> decltype(make_unary_expression<_op_functor>(a)) { \
	return make_unary_expression<_op_functor>(a); \
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
	auto _op(const A& a, int) \
	-> decltype(make_unary_expression<_op_functor>(a)) { \
	return make_unary_expression<_op_functor>(a); \
} \

DEFINE_UNARY_POST_OPERATOR(operator ++, ops::postinc)
DEFINE_UNARY_POST_OPERATOR(operator --, ops::postdec)

#undef DEFINE_UNARY_POST_OPERATOR

template <typename V>
struct value_expression: expression<value_expression<V>> {
	const V& v;

	value_expression(const V& v): v(v) {}

	template <typename... ARGS>
	const V& operator()(const ARGS&...) const {
		return v;
	}
};

template <typename A, typename B, typename F>
struct binary_expression: expression<binary_expression<A, B, F>> {
	A _a;
	B _b;
	F _f;

	binary_expression(const A& a, const B& b, F f): _a(a), _b(b), _f(f) {}

	template <typename... ARGS>
		auto operator()(ARGS&&... args)
			-> decltype(_f(_a(args...), _b(args...))) {
			return _f(_a(args...), _b(args...));
		}

	using expression<binary_expression<A, B, F>>::operator=;
};

template <typename A, typename F>
struct unary_expression: expression<unary_expression<A, F>> {
	A _a;
	F _f;

	unary_expression(const A& a, F f): _a(a), _f(f) {}

	template <typename... ARGS>
		auto operator()(ARGS&&... args)
			-> decltype(_f(_a(std::forward<ARGS>(args)...))) {
			return _f(_a(std::forward<ARGS>(args)...));
		}

	using expression<unary_expression<A, F>>::operator=;
};

template <int I>
struct arg_expression: expression<arg_expression<I>> {

	template <typename... ARGS>
	auto operator()(ARGS&&... args)
		-> decltype(
			get<I>(tuple<decltype(std::forward<ARGS>(args))...>(std::forward<ARGS>(args)...))
				) {
		return
			get<I>(tuple<decltype(std::forward<ARGS>(args))...>(std::forward<ARGS>(args)...))
				;
	}

	using expression<arg_expression<I>>::operator=;
};

arg_expression<0> _1;
arg_expression<1> _2;
arg_expression<2> _3;
arg_expression<3> _4;
arg_expression<4> _5;
arg_expression<5> _6;
arg_expression<7> _7;
arg_expression<8> _8;
arg_expression<9> _9;

#endif /* LAMBDAEXPR_H */
