/*
 * iterator.hpp
 * Copyright © 2011 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef ITERATOR_H
#define ITERATOR_H

#include <coroutine/builder.hpp>

namespace coroutine {

	template <typename S, typename F, typename... CONFIGS>
		struct iterator {
			/*
			typedef
				typename builder<S, F, CONFIGS...>::type
				coroutine_t;
				*/

			F _f;
			iterator(F f): _f(f) {}

			// fucking F part of coroutine signature...

			template <typename... Args>
				int operator()(Args&&... args) {
					return 42;
				}
		};


	template <typename S, typename... CONFIG, typename F>
		auto iter(F f) -> iterator<S, F, CONFIG...>
		{
			return iterator<S, F, CONFIG...>(f);
		}

	template <typename... CONFIG, typename F>
		auto iterf(F f) -> iterator<
			typename details::extract_signature<F>::type, F, CONFIG...>
		{
			return iterator<
				typename details::extract_signature<F>::type, F, CONFIG...>(f);
		}

} // namespace coroutine

#endif /* ITERATOR_H */
