/*
 * variant.hpp
 * Copyright © 2012 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef VARIANT_H
#define VARIANT_H

#include <string>
#include <sstream>
#include <memory>

#include "tools.hpp"

class variant {
	public:
		variant() = default;

		template <typename T>
		variant(T&& value): _holder(new holder<T>(std::forward<T>(value))) {}

		template <typename T>
			T& get() const {
				return *static_cast<T*>(_holder->get_ptr());
			}

		std::string str() const { return _holder->get_str(); }

	private:
		struct place_holder {
			virtual ~place_holder() {}
			virtual void* get_ptr() = 0;
			virtual std::string get_str() const = 0;
		};
		std::unique_ptr<place_holder> _holder;

		template <typename T>
			struct holder: place_holder {
				T value;

				template <typename U>
					holder(U&& v): value(std::forward<U>(v)) {}

				void* get_ptr() { return &value; }
				std::string get_str() const {
					std::ostringstream os;
					os << value;
					return os.str();
				}
			};
};

#endif /* VARIANT_H */
