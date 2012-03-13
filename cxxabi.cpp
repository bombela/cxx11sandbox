/*
 * cxxabi.cpp
 * Copyright © 2012 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>

#include <cxxabi.h>

namespace internal {

// wrapper for typeid
template <typename T>
struct TypeWrapper {};

} // namespace internal

// typeName resolution
template <typename T>
std::string typeName()
{
	int status;
	char* demangledName = abi::__cxa_demangle(
			typeid(internal::TypeWrapper<T>).name(), 0, 0, &status);

	std::string r;
	if (status == 0)
	{
		r = demangledName;

		// remove useless TypeWrapper name
		std::string::size_type pos = r.find("TypeWrapper<");
		if (pos != std::string::npos)
		{
			r.erase(0, pos + 12);
			r.erase(r.size() - 1, 1);
		}
	}
	else
	{
		std::ostringstream os;
		os << "demangle error(" << status << ")";
	}
	::free(demangledName);
	return r;
}

#if __INCLUDE_LEVEL__ == 0

template <typename T, int> struct Lolita {};

int main()
{
	std::cout << typeName<int>() << std::endl;
	std::cout << typeName< Lolita<char, 42> >() << std::endl;
	return 0;
}

#endif
