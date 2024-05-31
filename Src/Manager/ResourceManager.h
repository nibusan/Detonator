#pragma once
#include <filesystem>
#include <utility>
#include <unordered_map>
#include <unordered_set>
#include <array>
#include <string>
#include "../Common/StaticSingleton.h"
#include "../Common/nlohmann-json/json.hpp"
#include "../Common/Handle/HandleBase.h"
#include "../Common/Handle/Sound/Sound.h"
#include "SceneManager.h"

class ResourceManager : public StaticSingleton<ResourceManager> {
public:
	static const std::string RESOURCE_PATH;	//リソースのパス
	static const std::string WWDATA;		//ファイルの拡張子

											//読み込むリソースの種類
	enum class RESOURCE_TYPE {
		GRAPHIC = 0,
		SOUND,
		FONT,
		MODEL,
		PIXEL_SHADER,
		VERTEX_SHADER,
		MAX
	};

private:
	using USING_RESOURCE_LIST = std::unordered_set<std::string>;

	// リソースデータのベース
	class RESOURCE_DATA_BASE {
	public:
		RESOURCE_TYPE type;
		std::filesystem::path path;
		std::string fileName;
		std::string extension;
		std::filesystem::path fullPath;
	};

	// サウンドのリソースデータ
	class RESOURCE_DATA_SOUND : public RESOURCE_DATA_BASE {
	public:
		Sound::DIMENSION_TYPE dimensionType;
	};

	struct RESOURCE_LIST {
		std::filesystem::path basePath;
		std::array<std::filesystem::path, static_cast<int>(RESOURCE_TYPE::MAX)> resourcePath;
		std::unordered_map<std::string, std::unique_ptr<RESOURCE_DATA_BASE>> resourceDatas;
		std::array<USING_RESOURCE_LIST, static_cast<int>(RESOURCE_TYPE::MAX)> using_Global;
		std::array<std::array<USING_RESOURCE_LIST, static_cast<int>(RESOURCE_TYPE::MAX)>, static_cast<int>(SceneManager::SCENE_ID::MAX)> using_Scenes;
	};

	RESOURCE_LIST resourceList_;

	//リソースを格納する配列
	std::unordered_map<std::string, std::shared_ptr<HandleBase>> resourceData_;

	//現在シーンで使用しているリソースのリスト
	std::unordered_set<std::string> currentUsingResource_;


	//コンストラクタ
	ResourceManager(void);


	/// <summary>
	/// リソースを読み込む
	/// </summary>
	/// <param name="type"> キー </param>
	void Load(std::string key);
public:
	//デストラクタ
	~ResourceManager(void);
	THIS_CLASS_IS_STATIC_SINGLETON(ResourceManager);

	void Init(void);

	void LoadResource(SceneManager::SCENE_ID sceneID);

	void Release(void);


	// リソースを読み込む(与えられた配列に名前を追加する機能付き)
	//void Load(RESOURCE_TYPE type, int id, std::unordered_set<std::pair<RESOURCE_TYPE, int>>* list);

	/// <summary>
	/// 指定された名前のリソースを削除する
	/// </summary>
	/// <param name="name"> キー </param>
	void Delete(std::string key);

	/// <summary>
	/// 指定された名前のリソースを返す
	/// </summary>
	/// <param name="type"> 種類 </param>
	/// <param name="name"> 名前 </param>
	/// <returns></returns>
	[[nodiscard]] std::shared_ptr<HandleBase> Get(std::string key) const;
};