#include "endpoint-base.hpp"

#include <unistd.h>
#include <cstring>
#include <cassert>
#include <stdexcept>
#include <fstream>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

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
		else					{ std::cout << "~" << func << ": " << duration_s  <<  "s" << std::endl; return; }
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

static bool file_exists(const std::string& path) {
	struct stat st;
	return stat(path.c_str(), &st) == 0;
};

static std::string read_file(const std::string& file_path) {
	std::ifstream file(file_path);
	return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
};

static int count_entries(const std::string& path) {
    DIR *dp = opendir(path.c_str());
	if (dp == NULL) throw std::runtime_error("opendir() failed");

    int result = 0;

	struct dirent *entry;
    while ((entry = readdir(dp))) {
		if (entry->d_name[0] == '.') continue;
        result++;
    }

    closedir(dp);

    return result;
}

static html::div fastfetch() {
	PROFILE;

#if 0

	auto div = html::div();

	div.addAttribute("id", "fastfetch");

	std::string cmd = "fastfetch";

	div << (html::span().addAttribute("class", "terminal") << cmd) << html::br();
	
	div << (html::div() << escapeHtmlString(exec("fastfetch -c /etc/fastfetch.jsonc | sed -b 's/\\x1b[^m]*m//g'")));

	return div;

#else

	#define FASTFETCH_KEY(name) (html::span().addAttribute("class", "accented") << name) << ": "

	auto div = html::div();
	div.addAttribute("id", "fastfetch");

	static std::string cmd = "fastfetch";
	div << (html::span().addAttribute("class", "terminal") << cmd) << html::br();

	static std::string title = ([]() {
		char hostname[1024];
		gethostname(hostname, 1024);
		return std::string(getlogin()) + "@" + hostname;
	})();
	div << escapeHtmlString(title) << html::br();

	static std::string separator = "----------------";
	div << separator << html::br();

	static std::string os = ([]() {
			std::string name = ([]() {	
				FILE *os_release = fopen("/usr/lib/os-release", "r");
				assert(os_release);

				char buffer[1024];
				while (fgets(buffer, 1024, os_release)) {
					if (strncmp(buffer, "PRETTY_NAME=", 12) == 0) {
						char *start = strchr(buffer, '"') + 1;
						char *end = strchr(start + 1, '"');
						return std::string(start, end);
					}
				}

				throw std::runtime_error("/usr/lib/os-release does not contain PRETTY_NAME");
			})();

			std::string arch = exec("uname -m");

			return name + " " + arch;
	})();
	div << FASTFETCH_KEY("OS") << os << html::br();

	static std::string host = ([]() {
		std::string name = read_file("/sys/class/dmi/id/product_name");
		name.erase(name.find_last_not_of(" \t\n\r\f\v") + 1);
		std::string version = read_file("/sys/class/dmi/id/product_version");
		version.erase(version.find_last_not_of(" \t\n\r\f\v") + 1);
		return name + " (" + version + ")";
	})();
	div << FASTFETCH_KEY("Host") << host << html::br();

	static std::string bios = ([]() {
		std::string type = (file_exists("/sys/firmware/efi") || file_exists("/sys/firmware/acpi/tables/UEFI")) ? "UEFI" : "BIOS";
		std::string version = read_file("/sys/class/dmi/id/bios_version");
		std::string release = read_file("/sys/class/dmi/id/bios_release");

		return "(" + type + ") " + version + " " + release;
	})();
	div << FASTFETCH_KEY("Bios") << bios << html::br();

	// TODO: Board
	// TODO: Chassis
	
	std::string kernel = exec("uname -r");
	div << FASTFETCH_KEY("Kernel") << kernel << html::br();

	std::string uptime = ([]() {
		struct sysinfo info;
		if (sysinfo(&info) == -1) throw std::runtime_error("sysinfo() failed");

		int uptime = info.uptime;

		int days = uptime / (24 * 3600);
		uptime %= (24 * 3600);
		int hours = uptime / 3600;
		uptime %= 3600;
		int minutes = uptime / 60;

		std::string result;

		if (days > 0) {
			result += std::to_string(days) + " day";
			if (days > 1) result += "s";
			result += ", ";
		}

		if (hours > 0) {
			result += std::to_string(hours) + " hour";
			if (hours > 1) result += "s";
			result += ", ";
		}

		if (minutes > 0) {
			result += std::to_string(minutes) + " minute";
			if (minutes > 1) result += "s";
			result += ", ";
		}

		result.pop_back();
		result.pop_back();

		return result;
	})();
	div << FASTFETCH_KEY("Uptime") << uptime << html::br();
	
	std::string packages = ([]() {
	#define ARCH_LINUX
	#ifdef ARCH_LINUX
		return std::to_string(count_entries("/var/lib/pacman/local")) + " (pacman)";
	#else
	#	error Unimplemented
	#endif
	})();
	div << FASTFETCH_KEY("Packages") << packages << html::br();
	
	// TODO: CPU
	// TODO: CPU usage
	// TODO: GPU
	// TODO: Memory
	// TODO: Swap
	// TODO: Disk
	// TODO: Battery
	// TODO: NetIO
	// TODO: DiskIO
	// TODO: Physical disk

	return div;

#endif
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
