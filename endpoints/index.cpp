#include "endpoint-base.hpp"

namespace endpoint {
namespace index {

static auto searchbars() {
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
	auto div = html::div();

	div.addAttribute("id", "fastfetch");

	std::string cmd = "fastfetch";

	div << (html::span().addAttribute("class", "terminal") << cmd) << html::br();
	
	div << (html::div() << escapeHtmlString(exec("fastfetch -c /etc/fastfetch.jsonc | sed -b 's/\\x1b[^m]*m//g'")));

	return div;
}

static html::div df() {
	auto div = html::div();

	div.addAttribute("id", "df");

	std::string cmd = "df";

	div << (html::span().addAttribute("class", "terminal") << cmd) << html::br();
	
	div << (html::div() << escapeHtmlString(exec(cmd)));

	return div;
}

static html::div fortune() {
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
	auto div = html::div();

	div.addAttribute("id", "pstree");

	std::string cmd = "pstree";

	div << (html::span().addAttribute("class", "terminal") << cmd) << html::br();

	div << (html::div() << escapeHtmlString(exec(cmd)));

	return div;
}

endpointDispatchResult serve(http::request &req) {
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
