/*
 * property.hpp
 * Copyright © 2012 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef PROPERTY_H
#define PROPERTY_H

#include <functional>
#include <stdexcept>
#include <utility>

template <typename T>
struct property {
	std::function<T ()> _getter;
	std::function<void (T)> _setter;

	static inline void ronly(T) {
		throw std::runtime_error("trying to access a read-only property");
	}

	template <typename F, typename G>
		explicit property(F f, G g, decltype(g(std::declval<T>()))* = nullptr):
			_getter(f), _setter(g) {}

	template <typename F, typename G>
		explicit property(F f, G g, decltype(f(std::declval<T>()))* = nullptr):
			_getter(g), _setter(f) {}

	template <typename F>
		explicit property(F f):
			_getter(f), _setter(ronly) {}

	operator T() const {
		return _getter();
	}

	const property& operator=(const T& value) const {
		_setter(value);
		return *this;
	}
	const property& operator=(T&& value) const {
		_setter(std::move(value));
		return *this;
	}
};

#endif /* PROPERTY_H */
