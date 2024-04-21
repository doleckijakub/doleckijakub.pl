#include "endpoint-base.hpp"

namespace endpoint {
namespace index {

static html::div df() {
	auto div = html::div();

	div.addAttribute("id", "df");

	std::string cmd = "df";

	div << (html::span().addAttribute("class", "terminal") << cmd) << html::br();
	
	div << (html::div() << escapeHtmlString(exec("df")));

	return div;
}

static html::div fortune() {
	auto div = html::div();

	div.addAttribute("id", "fortune");

	std::string cowfile = exec("ls /usr/share/cows | shuf -n 1 | sed 's/.\\{4\\}$//'");
	std::string cmd = "fortune | cowsay -f "s + cowfile;

	div << (html::span().addAttribute("class", "terminal") << cmd) << html::br();

	std::string cowspeech = exec("fortune | cowsay -f "s + cowfile);

	div << (html::div() << escapeHtmlString(cowspeech));

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
		body << (html::main() /* <<  */);
		body << df();
		body << fortune();
		
		html << body;
	}

	return { true, sendHTML(req.response(), html) };
}

} // namespace index
} // namespace endpoint
