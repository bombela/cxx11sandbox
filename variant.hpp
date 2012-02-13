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
#include <typeinfo>
#include <assert.h>

#include "tools.hpp"

class variant {
	public:
		~variant() = default;
		variant() = default;

		template <typename T>
		variant(T&& value, typename std::enable_if<
			not std::is_same<typename std::decay<T>::type, variant>::value
				>::type* = nullptr):
			_holder(new holder<typename std::decay<T>::type>(
						std::forward<T>(value)
						)) {}

		variant(const variant& o): _holder(o._holder->clone()) {}
		variant& operator=(const variant& o) {
			_holder.reset(o._holder->clone());
			return *this;
		}

		variant(variant&& o): _holder(std::move(o._holder)) {}
		variant& operator=(variant&& o) {
			_holder = std::move(o._holder);
			return *this;
		}

		template <typename T>
			T& cast() const {
				if (_holder->get_typeinfo() != typeid(T)) {
					throw std::bad_cast();
				}
				return *static_cast<T*>(_holder->get_ptr());
			}

		template <typename T>
			T& cast_unsafe() const {
				return *static_cast<T*>(_holder->get_ptr());
			}

		std::string str() const { return _holder->get_str(); }

	private:
		struct place_holder {
			virtual ~place_holder() {}
			virtual void* get_ptr() = 0;
			virtual std::string get_str() const = 0;
			virtual place_holder* clone() const = 0;
			virtual const std::type_info& get_typeinfo() const = 0;
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

				virtual holder* clone() const {
					return new holder(value);
				}

				const std::type_info& get_typeinfo() const {
					return typeid(T);
				}
			};
};

std::ostream& operator<<(std::ostream& os, const variant& v)
{
	std::ostream::sentry init(os);
	if (init) {
		os << "variant(" << v.str() << ")";
	}
	return os;
}

#endif /* VARIANT_H */
