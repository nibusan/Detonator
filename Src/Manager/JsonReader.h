#pragma once
#include <filesystem>
#include "../Common/StaticSingleton.h"
#include "../Common/nlohmann-json/json.hpp"

class JsonReader : public StaticSingleton<JsonReader> {
public:
	~JsonReader(void) = default;
	THIS_CLASS_IS_STATIC_SINGLETON(JsonReader);

	/// @brief json����͂���
	/// @param path json�̃p�X
	/// @return ��͍ς݂�json�f�[�^(�A�z�z��)
	nlohmann::json Parse(const std::filesystem::path& path);
private:

	JsonReader(void) = default;
};