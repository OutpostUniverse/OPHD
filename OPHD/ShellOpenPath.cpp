#include "ShellOpenPath.h"

#include <cstdlib>


void shellOpenPath(const std::string& path)
{
	#if defined(_WIN32)
		system(("start " + path).c_str());
	#elif defined(__APPLE__)
		system(("open " + path).c_str());
	#elif defined(__linux__)
		system(("xdg-open " + path).c_str());
	#else
		#pragma message("Open folder on the current platform not implemented.")
	#endif
}
