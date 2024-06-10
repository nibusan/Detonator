#pragma once
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <string>
#include <filesystem>
#include "../Common/StaticSingleton.h"
#include "../Common/Handle/HandleBase.h"
#include "ResourceType.h"
#include "SceneManager.h"
#include "../Common/nlohmann-json/json.hpp"

class ResourceManager : public StaticSingleton<ResourceManager> {
public:
	// リソースファイルの型
	using resource_file_list_t = std::unordered_set<std::string>;

	// アセット(リソースファイル)を格納するフォルダへのベースのパス
	static const std::filesystem::path BASE_ASSETS_PATH;

	// リソースリストのjsonファイルへのパス
	static const std::filesystem::path RESOURCE_LIST_PATH;

	// jsonの各リソースファイルの種類を指すタグ(jsonのキー)
	static const std::array<std::string, static_cast<int>(Resource::RESOURCE_FILE_TYPE::MAX)> RESOURCE_FILE_TAG;

	~ResourceManager(void) = default;
	THIS_CLASS_IS_STATIC_SINGLETON(ResourceManager);

	/// @brief 初期化処理 
	void Init(void);

	/// @brief 登録されているリソースファイルのデータを返す
	/// @param key 取得したいリソースファイルのキー
	/// @return キーに対応したリソースファイルのデータ
	std::weak_ptr<HandleBase> GetResourceFile(const std::string& key);

	/// @brief 指定されたシーンに登録されてるリソースファイルを読み込む
	/// @param sceneType シーンの種類
	void LoadSceneResourceFile(SceneManager::SCENE_ID sceneType);

private:
	/// @brief リソースファイルを管理するためのデータ
	/// @param resourceType リソースファイルの種類
	/// @param filePath リソースファイルへのパス
	/// @param fileName リソースファイルの名前
	/// @param extension リソースファイルの拡張子
	struct RESOURCE_FILE_INFO {
		Resource::RESOURCE_FILE_TYPE resourceType;
		std::filesystem::path filePath;
		std::string fileName;
		std::string extension;
	};

	// リソースファイルを管理する連想配列(キー : リソースファイルのキー, 値 : リソースファイルのデータ)
	std::unordered_map<std::string, RESOURCE_FILE_INFO> resourceFileInfoList_;

	// 全てのシーンで使うリソースファイルのキーを管理
	// ここに登録しているものはシーン遷移しても解放されないようになってます
	resource_file_list_t globalUsingResourceList_;

	// 各シーンが使用するリソースファイルのキーを管理
	std::array<resource_file_list_t, static_cast<int>(SceneManager::SCENE_ID::MAX)> sceneUsingResourceList_;

	// 現在読み込まれてるリソースファイルを格納(キー : リソースファイルのキー, 値 : リソースファイル)
	std::unordered_map<std::string, std::shared_ptr<HandleBase>> currentUsingResourceFiles_;

	ResourceManager(void) = default;

	/// @brief 各リソースファイルの情報を登録
	/// @param resourceFileListJson リソースファイルのjson
	void RegisterResourceFileInfo(const nlohmann::json& resourceFileListJson);

	/// @brief 全てのシーンで使用するリソースファイルのキーを登録
	/// @param resourceFileListJson リソースファイルのjson
	void RegisterGlobalUsingResourceList(const nlohmann::json& resourceFileListJson);

	/// @brief 各シーンで使用するリソースファイルのキーを登録
	/// @param resourceFileListJson リソースファイルのjson
	void RegisterUsingResourceList(const nlohmann::json& resourceFileListJson);

	/// @brief 指定されたリソースファイルを読み込む
	/// @param resourceKey リソースファイルのキー
	/// @return 読み込まれたリソースファイル
	std::shared_ptr<HandleBase> LoadResourceFile(const std::string& resourceKey);
};

