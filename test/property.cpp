/*
 * property.hpp
 * Copyright © 2012 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <iostream>
#include <property.hpp>

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
