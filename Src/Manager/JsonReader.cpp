#include <fstream>
#include <istream>
#include <sstream>
#include <filesystem>
#include "JsonReader.h"

nlohmann::json JsonReader::Parse(const std::filesystem::path& path) {
	// ‰ðÍÏ‚Ý‚Ìjson‚ðŠi”[
	nlohmann::json parcedJson_;

	// json‚Ì“Ç‚Ýž‚Ý
	std::ifstream ifs(path.string());

	// json‚ð‰ðÍ‚·‚é
	ifs >> parcedJson_;
	return parcedJson_;
}
