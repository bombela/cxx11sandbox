/*
 * benchmark.hpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <list>
#include <functional>
#include <algorithm>
#include <cassert>

#include <benchmark/timer.hpp>

namespace benchmark {

class Bench;

namespace details {

	std::list<Bench*> benchs;

} // namespace details

class Bench
{
	public:
		Bench() { details::benchs.push_back(this); }
		virtual ~Bench() {}
		virtual void do_bench(const char* pkg) const = 0;
};

void runalls(const char* pkg)
{ std::for_each(details::benchs.begin(), details::benchs.end(),
			std::bind2nd(std::mem_fun(&Bench::do_bench), pkg));
}

} // namespace benchmark

#define BENCH_STR(str) #str
#define BENCH_CAT_I(a, b) a##b
#define BENCH_CAT(a, b) BENCH_CAT_I(a, b)

#define BENCH(name, _cnt_)\
struct BENCH_CAT(bench_, name): benchmark::Bench { \
	void do_bench(const char* pkg) const { \
		unsigned cnt = _cnt_; \
		benchmark::Timer t(pkg, BENCH_STR(name), cnt); \
		for (;cnt;cnt--) bench_impl(cnt); }\
	void bench_impl(unsigned) const; \
} BENCH_CAT(bench_instance_, name); \
__attribute__((noinline)) \
void BENCH_CAT(bench_, name)::bench_impl( \
		unsigned __attribute__((unused)) BENCH_CNT) const \

#define BENCH_WF(name, _cnt_, _fixture_expr_) \
struct BENCH_CAT(bench_, name): benchmark::Bench { \
	typedef decltype(_fixture_expr_) fixture_t; \
	void do_bench(const char* pkg) const { \
		unsigned cnt = _cnt_; \
		fixture_t fixture = _fixture_expr_; \
		benchmark::Timer t(pkg, BENCH_STR(name), cnt); \
		for (;cnt;cnt--) bench_impl(cnt, fixture); }\
	void bench_impl(unsigned, fixture_t&) const; \
} BENCH_CAT(bench_instance_, name); \
__attribute__((noinline)) \
void BENCH_CAT(bench_, name)::bench_impl( \
		unsigned __attribute__((unused)) BENCH_CNT, \
		fixture_t& __attribute__((unused)) BENCH_FIXTURE) const \

#define BENCH_MAIN(pkg) \
int main() { \
	benchmark::runalls(#pkg); \
	return 0; \
}

template <typename T>
__attribute__((noinline))
extern void BENCH_SWALLOW(const T&) {}

#define BENCH_ASSERT(__X__) \
	assert(__X__); \
	BENCH_SWALLOW(__X__);

#endif /* BENCHMARK_H */
