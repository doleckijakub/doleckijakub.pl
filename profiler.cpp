#include "profiler.hpp"

#include <iostream>

int profiler::indent = 0;

static void print_indent(int indent) {
    std::cout << "\e[32;1mprofiler\e[0m::";
    for (int i = 0; i < indent; i++) {
        std::cout << "  ";
    }
}

profiler::profiler(const char *func) : func(func) {
    start = std::chrono::high_resolution_clock::now();

    print_indent(indent++);
    std::cout << func << std::endl;
}

profiler::~profiler() {
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    auto duration_s = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();

    print_indent(--indent);
    
    if (duration_us < 1000) { std::cout << "~" << func << ": " << duration_us << "us" << std::endl; return; }
    if (duration_ms < 1000) { std::cout << "~" << func << ": " << duration_ms << "ms" << std::endl; return; }
    else					{ std::cout << "~" << func << ": " << duration_s  <<  "s" << std::endl; return; }
}
