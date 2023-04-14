#include "ShellOpenPath.h"

#include <string_view>
#include <cstdlib>
#include <tuple>


namespace
{
	#if defined(_WIN32)
		#include <windows.h>
		#include <shellapi.h>
	#elif defined(__APPLE__)
		constexpr const std::string_view ShellOpenCommand{"open"};
	#elif defined(__linux__) || defined(__FreeBSD__)
		constexpr const std::string_view ShellOpenCommand{"xdg-open"};
	#else
		#pragma message("Constant `ShellOpenCommand` not defined for the current platform.")
	#endif
}


void shellOpenPath(const std::string& path)
{
	#if defined(_WIN32)
		std::ignore = ShellExecuteA(NULL, "explore", path.c_str(), NULL, NULL, SW_NORMAL);
	#else
		// Explicitly ignore implementation defined return value
		std::ignore = std::system((std::string{ShellOpenCommand} + " " + path).c_str());
	#endif
}
