#include "endpoint-base.hpp"

#include <iostream>
#include <chrono>

static int p = 0;

struct profiler {

	static void pp() {
		for (int i = 0; i < p; i++) {
			std::cout << "  ";
		}
	}
	
	const char *func;
	std::chrono::high_resolution_clock::time_point start;

	profiler(const char *func) : func(func) {
		start = std::chrono::high_resolution_clock::now();

		pp();
		std::cout << func << std::endl;
		p++;
	}

	~profiler() {
		auto end = std::chrono::high_resolution_clock::now();
		p--;
		pp();
		auto duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
		auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		auto duration_s = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
		
		if (duration_us < 1000) { std::cout << "~" << func << ": " << duration_us << "us" << std::endl; return; }
		if (duration_ms < 1000) { std::cout << "~" << func << ": " << duration_ms << "ms" << std::endl; return; }
		else                    { std::cout << "~" << func << ": " << duration_s  <<  "s" << std::endl; return; }
	}

};

#define PROFILE profiler e(__func__)

namespace endpoint {
namespace index {

static auto searchbars() {
	PROFILE;

	auto main = html::main();
	
	main.addAttribute("style", "z-index: 1;");

	const static auto searchbar = [](const char *action, const char *name, const char *placeholder) {
		return html::form()
			.addAttribute("action", action)
			.addAttribute("method", "GET")
			<< html::input()
			.addAttribute("autofocus")
			.addAttribute("class", "search")
			.addAttribute("name", name)
			.addAttribute("placeholder", placeholder);
	};

	main << searchbar("https://fusion.doleckijakub.pl/search", "q", "fusion");
	main << searchbar("https://google.com/search", "q", "google");
	main << searchbar("https://duckduckgo.com", "q", "ddg");

	return main;
}

static html::div fastfetch() {
	PROFILE;

	auto div = html::div();

	div.addAttribute("id", "fastfetch");

	std::string cmd = "fastfetch";

	div << (html::span().addAttribute("class", "terminal") << cmd) << html::br();
	
	div << (html::div() << escapeHtmlString(exec("fastfetch -c /etc/fastfetch.jsonc | sed -b 's/\\x1b[^m]*m//g'")));

	return div;
}

static html::div df() {
	PROFILE;

	auto div = html::div();

	div.addAttribute("id", "df");

	std::string cmd = "df";

	div << (html::span().addAttribute("class", "terminal") << cmd) << html::br();
	
	div << (html::div() << escapeHtmlString(exec(cmd)));

	return div;
}

static html::div fortune() {
	PROFILE;

	auto div = html::div();

	div.addAttribute("id", "fortune");

	static std::string cowfiles[] = {
		"bong",
		"bud-frogs",
		"bunny",
		"cower",
		"default",
		"elephant-in-snake",
		"flaming-sheep",
		"head-in",
		"kitty",
		"koala",
		"kosh",
		"luke-koala",
		"meow",
		"moofasa",
		"moose",
		"mutilated",
		"satanic",
		"sheep",
		"skeleton",
		"small",
		"supermilker",
		"three-eyes",
		"tux",
		"vader",
		"vader-koala",
		"www",
	};

	std::string cowfile = cowfiles[rand() % std::size(cowfiles)];
	std::string cmd = "fortune | cowsay -f "s + cowfile;

	div << (html::span().addAttribute("class", "terminal") << cmd) << html::br();

	div << (html::div() << escapeHtmlString(exec(cmd)));

	return div;
}

static html::div pstree() {
	PROFILE;

	auto div = html::div();

	div.addAttribute("id", "pstree");

	std::string cmd = "pstree";

	div << (html::span().addAttribute("class", "terminal") << cmd) << html::br();

	div << (html::div() << escapeHtmlString(exec(cmd)));

	return div;
}

endpointDispatchResult serve(http::request &req) {
	PROFILE;

	if (req.url.path.size() != 0)
		return {false, false};

	auto html = html::html();

	{
		auto head = html::head();
		head << html::link().addAttribute("rel", "stylesheet").addAttribute("href", "/index.css");
		
		html << head;
	}

	{
		auto body = html::body();
		body << searchbars();
		body << df();
		body << pstree();
		body << fastfetch();
		body << fortune();
		
		html << body;
	}

	return { true, sendHTML(req.response(), html) };
}

} // namespace index
} // namespace endpoint
