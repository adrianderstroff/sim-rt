#ifndef PATH_UTIL
#define PATH_UTIL

#include <string>
#include <filesystem>

namespace rt {
	inline std::string fix_path(std::string basepath, std::string filepath) {
		std::filesystem::path path(filepath);

		// return if path is absolute
		if (path.is_absolute()) return path.string();
		
		// add base to the filepath
		std::filesystem::path base(basepath);
		base = base.parent_path();
		base /= path;
		return base.string();
	}
}

#endif//PATH_UTIL