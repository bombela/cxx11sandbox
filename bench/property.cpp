
#include <benchmark/benchmark.hpp>
#include <property.hpp>

BENCH(construct_getset, 10000) {

	class SomeClass {
		private:
			int _value = 21;
		public:
			int get() const { return _value; }
			void set(int v) { _value = v; }
	};

	BENCH_SWALLOW(SomeClass());
};

BENCH(construct_property, 10000) {

	class SomeClass {
		private:
			int _value = 21;

		public:
			const property<int> value {
				[this]{ return _value; },
				[this](int v){ _value = v; },
			};
	};

	BENCH_SWALLOW(SomeClass());
};

class SomeClass_getset {
	private:
		int _value = 21;
	public:
		int get() const { return _value; }
		void set(int v) { _value = v; }
};

BENCH_WF(getset, 10000, SomeClass_getset()) {
	BENCH_FIXTURE.set(BENCH_CNT % 512);
	BENCH_ASSERT(BENCH_FIXTURE.get() == BENCH_CNT % 512);
}

BENCH_WF(get, 10000, SomeClass_getset()) {
	BENCH_ASSERT(BENCH_FIXTURE.get() == 21);
}

class SomeClass_prop {
	private:
		int _value = 21;

	public:
		const property<int> value {
			[this]{ return _value; },
			[this](int v){ _value = v; },
		};
};

BENCH_WF(property_getset, 10000, SomeClass_prop()) {
	BENCH_FIXTURE.value = BENCH_CNT % 512;
	BENCH_ASSERT(BENCH_FIXTURE.value == BENCH_CNT % 512);
}

BENCH_WF(property_get, 10000, SomeClass_prop()) {
	BENCH_ASSERT(BENCH_FIXTURE.value == 21);
}

BENCH_MAIN(property)
