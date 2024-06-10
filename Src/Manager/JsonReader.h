#pragma once
#include <filesystem>
#include "../Common/StaticSingleton.h"
#include "../Common/nlohmann-json/json.hpp"

class JsonReader : public StaticSingleton<JsonReader> {
public:
	~JsonReader(void) = default;
	THIS_CLASS_IS_STATIC_SINGLETON(JsonReader);

	/// @brief jsonを解析する
	/// @param path jsonのパス
	/// @return 解析済みのjsonデータ(連想配列)
	nlohmann::json Parse(const std::filesystem::path& path);
private:

	JsonReader(void) = default;
};