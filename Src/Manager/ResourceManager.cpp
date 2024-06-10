#include <array>
#include "ResourceManager.h"
#include "JsonReader.h"
#include "SceneManager.h"
#include "../Common/Handle/Graphic/Graphic.h"
#include "../Common/Handle/Sound/Sound.h"
#include "../Common/Handle/Model/Model.h"
#include "../Common/Handle/Font/Font.h"
#include "../Common/Handle/PixelShader/PixelShader.h"
#include "../Common/Handle/VertexShader/VertexShader.h"

const std::filesystem::path ResourceManager::BASE_ASSETS_PATH = "Data/";
const std::filesystem::path ResourceManager::RESOURCE_LIST_PATH =  ResourceManager::BASE_ASSETS_PATH / "Data/Resource/ResourceList.json";
const std::array<std::string, static_cast<int>(Resource::RESOURCE_FILE_TYPE::MAX)> ResourceFileTag = {
	"Image",
	"Sound2D",
	"Sound3D",
	"Model",
	"Font",
	"PixelShader",
	"VertexShader"
};

void ResourceManager::Init(void) {
	// リソースファイルリスト(json)の読み込み
	const auto resourceListJson = JsonReader::GetInstance().Parse(RESOURCE_LIST_PATH);

	// 各リソースファイルの情報を登録
	RegisterResourceFileInfo(resourceListJson);

	// 全てのシーンで使用するリソースファイルを登録したリストを読み込む
	RegisterGlobalUsingResourceList(resourceListJson);

	// 各シーンの使用するリソースファイルを登録したリストを読み込む
	RegisterUsingResourceList(resourceListJson);

}

std::shared_ptr<HandleBase> ResourceManager::GetResourceFile(const std::string& key) {
	return currentUsingResourceFiles_[key];
}

void ResourceManager::LoadSceneResourceFile(SceneManager::SCENE_ID sceneType) {
	// 現在のシーンの種類を取得
	const auto currentSceneType = SceneManager::GetInstance().GetSceneID();
	const auto& usingResourceList = sceneUsingResourceList_[static_cast<int>(currentSceneType)];

	// 切り替え先のシーンで読み込むリソースファイルのリストをコピーする
	auto nextUsingResourceList = sceneUsingResourceList_[static_cast<int>(sceneType)];

	// 不要なリソースファイルをアンロード(削除)する
	for (auto itr = currentUsingResourceFiles_.begin(); itr != currentUsingResourceFiles_.end(); ) {
		// キーがグローバルのリソースファイルのリストに登録されてる場合は何もしない
		if (globalUsingResourceList_.find(itr->first) != globalUsingResourceList_.end()) {
			++itr;
			continue;
		}

		// キーが切り替え先のシーンのリソースファイルのリストに登録されてる場合は何もしない
		if (nextUsingResourceList.find(itr->first) != nextUsingResourceList.end()) {
			// 次のシーンで読みこまなくていいのでリソースファイルのリストから削除する
			nextUsingResourceList.erase(itr->first);
			++itr;
			continue;
		}
		else {
			// 存在しない場合は削除する
			itr->second.reset();
			itr->second = nullptr;
			itr = currentUsingResourceFiles_.erase(itr);
		}
	}

	// 読み込まれてないリソースファイルだけを読み込む
	for (auto itr = nextUsingResourceList.begin(); itr != nextUsingResourceList.end(); ++itr) {
		currentUsingResourceFiles_[(*itr)] = LoadResourceFile((*itr));
	}

}

void ResourceManager::RegisterResourceFileInfo(const nlohmann::json& resourceFileListJson) {
	// 各リソースファイルの情報を登録
	for (int i = 0; i < static_cast<int>(Resource::RESOURCE_FILE_TYPE::MAX); i++) {
		for (const auto& resourceFile : resourceFileListJson["ResourceList"][ResourceFileTag[i]]) {
			// 格納用のリソースファイルの情報を生成
			RESOURCE_FILE_INFO info;
			info.resourceType = static_cast<Resource::RESOURCE_FILE_TYPE>(i);
			info.filePath =		// パスを計算 ベースパス + リソースファイルへのパス
				BASE_ASSETS_PATH / static_cast<std::filesystem::path>(static_cast<std::string>(resourceFile["FilePath"]));
			info.fileName = resourceFile["FileName"];
			info.extension = resourceFile["Extension"];

			// 登録されたリソースファイルのキーをキーとして格納
			resourceFileInfoList_[resourceFile["Key"]] = info;
		}
	}
}

void ResourceManager::RegisterGlobalUsingResourceList(const nlohmann::json& resourceFileListJson) {
	// 全てのシーンで使用するリソースファイルを登録したリストを読み込む
	const auto globalUsingResourceListJson = 
		JsonReader::GetInstance().Parse(BASE_ASSETS_PATH / resourceFileListJson["GlobalUsingResourceListPath"]);
	for (int i = 0; i < static_cast<int>(Resource::RESOURCE_FILE_TYPE::MAX); i++) {
		for (const auto& resourceKey : globalUsingResourceListJson["ResourceList"][ResourceFileTag[i]]) {
			globalUsingResourceList_.insert(resourceKey);
			currentUsingResourceFiles_[resourceKey] = LoadResourceFile(resourceKey);
		}
	}
}

void ResourceManager::RegisterUsingResourceList(const nlohmann::json& resourceFileListJson) {
	// 各シーンの使用するリソースファイルを登録したリストを読み込む
	for (int i = 0; i < static_cast<int>(SceneManager::SCENE_ID::MAX); i++) {
		const auto usingResourceListJson = 
			JsonReader::GetInstance().Parse(BASE_ASSETS_PATH / resourceFileListJson["UsingResourceListPath"][i]);
		for (int j = 0; j < static_cast<int>(Resource::RESOURCE_FILE_TYPE::MAX); j++) {
			for (const auto& resourceFile : usingResourceListJson["ResourceList"][ResourceFileTag[j]]) {
				sceneUsingResourceList_[i].insert(resourceFile);
			}
		}
	}
}

std::shared_ptr<HandleBase> ResourceManager::LoadResourceFile(const std::string& resourceKey) {
	// 読み込んだリソースファイルを格納
	std::shared_ptr<HandleBase> resourceFile;

	// 読み込むリソースファイルの情報を取得
	const auto& resourceFileInfo = resourceFileInfoList_.at(resourceKey);

	// 各リソースファイルの読み込み処理
	switch (resourceFileInfo.resourceType) {
	case Resource::RESOURCE_FILE_TYPE::IMAGE: {
		// 画像
		resourceFile = std::make_shared<Graphic>(
			resourceFileInfo.filePath / 
			(resourceFileInfo.fileName + 
				resourceFileInfo.extension)
		);
		break;
	}
	case Resource::RESOURCE_FILE_TYPE::SOUND_2D: {
		// 2D音声
		resourceFile = std::make_shared<Sound>(
			resourceFileInfo.filePath / 
			(resourceFileInfo.fileName + 
				resourceFileInfo.extension),
			Sound::DIMENSION_TYPE::_2D
		);
		break;
	}
	case Resource::RESOURCE_FILE_TYPE::SOUND_3D: {
		// 3D音声
		resourceFile = std::make_shared<Sound>(
			resourceFileInfo.filePath / 
			(resourceFileInfo.fileName + 
				resourceFileInfo.extension),
			Sound::DIMENSION_TYPE::_3D
		);
		break;
	}
	case Resource::RESOURCE_FILE_TYPE::MODEL: {
		// 3Dモデル
		resourceFile = std::make_shared<Model>(
			resourceFileInfo.filePath / 
			(resourceFileInfo.fileName + 
				resourceFileInfo.extension)
		);
		break;
	}
	case Resource::RESOURCE_FILE_TYPE::FONT: {
		// フォント
		resourceFile = std::make_shared<Font>(
			resourceFileInfo.filePath / 
			(resourceFileInfo.fileName + 
				resourceFileInfo.extension),
			resourceFileInfo.fileName,
			16
		);
		break;
	}
	case Resource::RESOURCE_FILE_TYPE::PIXEL_SHADER: {
		// ピクセルシェーダー
		resourceFile = std::make_shared<PixelShader>(
			resourceFileInfo.filePath / 
			(resourceFileInfo.fileName + 
				resourceFileInfo.extension)
		);
		break;
	}
	case Resource::RESOURCE_FILE_TYPE::VERTEX_SHADER: {
		// 頂点シェーダー
		resourceFile = std::make_shared<VertexShader>(
			resourceFileInfo.filePath / 
			(resourceFileInfo.fileName + 
				resourceFileInfo.extension)
		);
		break;
	}
	case Resource::RESOURCE_FILE_TYPE::MAX:
	default:
		break;
	}

	// デストラクタが呼ばれた際に自動でハンドルを解放するようにする
	resourceFile->SetIsAutoDeleteHandle(true);

	return resourceFile;
}
