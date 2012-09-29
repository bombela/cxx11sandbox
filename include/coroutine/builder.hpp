/*
 * builder.hpp
 * Copyright © 2011 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef BUILDER_H
#define BUILDER_H

#include <coroutine/coroutine.hpp>
#include <coroutine/best_context.hpp>
#include <functional>

namespace coroutine {

	namespace details {

		template <template <typename> class PREDICATE,
			bool B, typename HEAD, typename... TAIL>
			struct find_if_iter;

		template <template <typename> class PREDICATE, typename... TAIL>
			struct find_if;

		template <template <typename> class PREDICATE,
				 typename HEAD, typename... TAIL>
			struct find_if<PREDICATE, HEAD, TAIL...>
			: find_if_iter<PREDICATE, PREDICATE<HEAD>::value, HEAD, TAIL...> {};

		template <template <typename> class PREDICATE,
			typename HEAD, typename... TAIL>
			struct find_if_iter<PREDICATE, true, HEAD, TAIL...> {
				typedef HEAD type;
			};

		template <template <typename> class PREDICATE,
			typename HEAD, typename... TAIL>
			struct find_if_iter<PREDICATE, false, HEAD, TAIL...>
				: find_if<PREDICATE, TAIL...> {};

		template <typename S>
			struct decay_sign_rm_ptr;

		template <typename R, typename... ARGS>
			struct decay_sign_rm_ptr<R (ARGS...)> {
				typedef R type (ARGS...);
			};

		template <typename R, typename... ARGS>
			struct decay_sign_rm_ptr<R (*)(ARGS...)> {
				typedef R type (ARGS...);
			};

		template <typename S>
			struct decay_sign_rm_yielder;

		template <typename R, typename A1, typename... ARGS>
			struct decay_sign_rm_yielder<R (yielder<R (A1)>, ARGS...)> {
				typedef R type (ARGS...);
			};

		template <typename R, typename... ARGS>
			struct decay_sign_rm_yielder<R (ARGS...)> {
				typedef R type (ARGS...);
			};

		template <typename S>
			struct decay_sign {
				typedef typename decay_sign_rm_ptr<S>::type sign;
				typedef typename decay_sign_rm_yielder<sign>::type type;
			};

	} // namespace details

	template <typename S, typename F, typename... CONFIGS>
		struct builder {
			typedef typename details::decay_sign<S>::type sign_t;
			typedef F func_t;

			typedef typename
				details::find_if<context::is, CONFIGS..., context::best>::type
				config_context_tag;

			typedef typename context::resolve_alias<config_context_tag>::type
				context_tag;

			typedef typename
				details::find_if<stack::is, CONFIGS...,
					typename context_tag::default_stack>::type
				stack_tag;

			typedef
				typename context_tag::template default_stack_size<stack_tag>
				default_stack_size_type;

			typedef typename
				details::find_if<stack::is_size, CONFIGS...,
					default_stack_size_type>::type
				stack_size_type;

			static const size_t stack_size = stack_size_type::value;
			typedef stack::stack<stack_tag, stack_size> stack_type;
			typedef context::context<context_tag, stack_type> context_type;

			typedef coroutine<sign_t, func_t, context_type> type;
		};


	template <typename S, typename... CONFIG, typename F>
		auto coro_make(F f) -> typename builder<S, F, CONFIG...>::type
		{
			return typename builder<S, F, CONFIG...>::type(f);
		}

	template <typename S, typename... CONFIG, typename F>
		auto coro(F f) -> typename builder<S, F, CONFIG...>::type
		{
			return typename builder<S, F, CONFIG...>::type(f);
		}

	template <typename S, typename... CONFIG, typename F, typename... ARGS>
		auto coro(F f, ARGS... args) -> decltype(
			coro_make<S>(std::bind(f, std::placeholders::_1,
						std::forward<ARGS>(args)...))
				)
		{
			return coro_make<S, CONFIG...>(std::bind(f, std::placeholders::_1,
						std::forward<ARGS>(args)...));
		}

	namespace details {
		template <typename S>
			struct extract_signature;

		template <typename Y, typename R>
			struct extract_signature<R (*)(Y)> {
				typedef R (type) ();
			};

		template <typename Y, typename R, typename FV>
			struct extract_signature<R (*)(Y, FV)> {
				typedef R (type) (FV);
			};

	} // namespace details

	template <typename... CONFIG, typename F, typename... ARGS>
		auto corof(F f, ARGS&&... args) -> decltype(
				coro<typename details::extract_signature<F>::type,
				CONFIG..., F, ARGS...>(f, std::forward<ARGS>(args)...)
			)
		{
			return coro<typename details::extract_signature<F>::type,
			CONFIG..., F, ARGS...>(f, std::forward<ARGS>(args)...);
		}

} // namespace coroutine


#define COROL_DEF(...) \
	__VA_ARGS__ )

#define COROL(sign, ...) \
	::coroutine::coro<sign (__VA_ARGS__)>(\
			[=] (::coroutine::yielder<sign (__VA_ARGS__)> yield, ##__VA_ARGS__)\
			COROL_DEF


#endif /* BUILDER_H */
