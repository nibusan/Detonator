#include <fstream>
#include <istream>
#include <sstream>
#include <filesystem>
#include "JsonReader.h"

nlohmann::json JsonReader::Parse(const std::filesystem::path& path) {
	// ��͍ς݂�json���i�[
	nlohmann::json parcedJson_;

	// json�̓ǂݍ���
	std::ifstream ifs(path.string());

	// json����͂���
	ifs >> parcedJson_;
	return parcedJson_;
}
