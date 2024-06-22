#pragma once

#include <chrono>

class profiler {
	
	const char *func;
	std::chrono::high_resolution_clock::time_point start;
    static int indent;

public:

	profiler(const char *func);
	~profiler();

};

#define PROFILE_FUNC profiler e(__func__)