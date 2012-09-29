/*
 * timer.hpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef TIMER_H
#define TIMER_H

#include <benchmark/clock.hpp>
#include <iostream>
#include <sstream>
#include <fstream>

namespace benchmark {

class Timer
{
	public:
		Timer(const char* pkg,
				const char* name,
				int cnt):
			_pkg(pkg),
			_name(name),
			_cnt(cnt),
			_start(clock::time()) { }
		~Timer()
		{
			clock::nanos_t diff = clock::time() - _start;
			std::ostringstream os;
			os
				<< "elapsed=" << diff << "nsec, "
				<< "cnt=" << _cnt << ", "
				<< "res=" << clock::res() << "nsec, "
				<< "pkg=" << _pkg << ", "
				<< "name=" << _name;
			const std::string& record = os.str();
			std::cout << record << std::endl;
			std::ofstream of("bench.log", std::ios_base::app);
			of << record << std::endl;
		}

	private:
		const char*    _pkg;
		const char*    _name;
		int            _cnt;
		clock::nanos_t _start;
};

} // namespace benchmark

#endif /* TIMER_H */
