/*
 * range.hpp
 * Copyright © 2012 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <functional>
#include <iostream>
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

class MyClass {
	private:
		int _value = 21;
		double _value2 = 2.1;
		const char* _value3 = "read-only property";

	public:
		const property<int> value {
			[this]{ return _value; },
			[this](int v){ _value = v; },
		};

		const property<double> value2 {
			[this](double v){ _value2 = v; },
			[this]{ return _value2; },
		};

		const property<decltype(_value3)> value3 {
			[this] () -> decltype(_value3) {
				const auto r = _value3;
				_value3 = "already read!";
				return r;
			},
		};
};

int main()
{
	MyClass mc;
	std::cout << mc.value << std::endl;
	mc.value = 33;
	std::cout << mc.value << std::endl;

	std::cout << mc.value2 << std::endl;
	mc.value2 = 3.3;
	std::cout << mc.value2 << std::endl;

	std::cout << mc.value3 << std::endl;
	try {
		mc.value3 = "something";
	} catch(const std::exception& e) {
		std::cout << "Invalid! -> " << e.what() << std::endl;
	}
	std::cout << mc.value3 << std::endl;
	return 0;
}
