#include <fstream>
#include <sstream>
#include <istream>
#include "DxLib.h"
#include "../Common/Handle/Graphic/Graphic.h"
#include "../Common/Handle/Sound/Sound.h"
#include "../Common/Handle/Font/Font.h"
#include "../Common/Handle/Font/ImageFont.h"
#include "../Common/Handle/Model/Model.h"
#include "../Common/Handle/PixelShader/PixelShader.h"
#include "../Common/Handle/VertexShader/VertexShader.h"
#include "ResourceManager.h"

const std::string ResourceManager::RESOURCE_PATH = "Resource/";
const std::string ResourceManager::WWDATA = ".wwdata";

//コンストラクタ
ResourceManager::ResourceManager(void) {

}

//デストラクタ
ResourceManager::~ResourceManager(void) {

}

void ResourceManager::Init(void) {
	nlohmann::json json_ResourceList;
	std::ifstream ifs("Data/Data/Resource/ResourceList.json");
	ifs >> json_ResourceList;

	resourceList_.basePath = std::filesystem::path(std::string(json_ResourceList["BasePath"]));
	for (int i = 0; i < json_ResourceList["ResourcePath"].size(); i++) {
		resourceList_.resourcePath[i] = std::filesystem::path(std::string(json_ResourceList["ResourcePath"][i]));
	}

	for (int i = 0; i < json_ResourceList["ResourceData"].size(); i++) {
		RESOURCE_TYPE type = json_ResourceList["ResourceData"][i]["TypeID"];
		for (int j = 0; j < json_ResourceList["ResourceData"][i]["Datas"].size(); j++) {
			std::string key = json_ResourceList["ResourceData"][i]["Datas"][j]["Key"];
			std::filesystem::path calcPath = json_ResourceList["BasePath"];
			calcPath /= json_ResourceList["ResourcePath"][i];
			calcPath /= json_ResourceList["ResourceData"][i]["Datas"][j]["Path"];
			std::string fileName = json_ResourceList["ResourceData"][i]["Datas"][j]["FileName"];
			std::string extension = json_ResourceList["ResourceData"][i]["Datas"][j]["Extension"];

			switch (type) {
			case ResourceManager::RESOURCE_TYPE::GRAPHIC: {
				RESOURCE_DATA_BASE resData = {
					type,
					calcPath,
					fileName,
					extension,
					(calcPath / fileName).string() + "." + extension
				};
				resourceList_.resourceDatas[key] = std::make_unique<RESOURCE_DATA_BASE>(resData);
			}
														break;
			case ResourceManager::RESOURCE_TYPE::SOUND: {
				RESOURCE_DATA_SOUND resData = {
					type,
					calcPath,
					fileName,
					extension,
					(calcPath / fileName).string() + "." + extension,
					json_ResourceList["ResourceData"][i]["Datas"][j]["SoundData"]["DimensionType"]
				};
				resourceList_.resourceDatas[key] = std::make_unique<RESOURCE_DATA_SOUND>(resData);
			}	
													  break;
			case ResourceManager::RESOURCE_TYPE::FONT: {
				RESOURCE_DATA_BASE resData = {
					type,
					calcPath,
					fileName,
					extension,
					(calcPath / fileName).string() + "." + extension
				};
				resourceList_.resourceDatas[key] = std::make_unique<RESOURCE_DATA_BASE>(resData);
			}
													 break;
			case ResourceManager::RESOURCE_TYPE::MODEL: {
				RESOURCE_DATA_BASE resData = {
					type,
					calcPath,
					fileName,
					extension,
					(calcPath / fileName).string() + "." + extension
				};
				resourceList_.resourceDatas[key] = std::make_unique<RESOURCE_DATA_BASE>(resData);
			}
													  break;
			case ResourceManager::RESOURCE_TYPE::PIXEL_SHADER: {
				RESOURCE_DATA_BASE resData = {
					type,
					calcPath,
					fileName,
					extension,
					(calcPath / fileName).string() + "." + extension
				};
				resourceList_.resourceDatas[key] = std::make_unique<RESOURCE_DATA_BASE>(resData);
			}
															 break;
			case ResourceManager::RESOURCE_TYPE::VERTEX_SHADER: {
				RESOURCE_DATA_BASE resData = {
					type,
					calcPath,
					fileName,
					extension,
					(calcPath / fileName).string() + "." + extension
				};
				resourceList_.resourceDatas[key] = std::make_unique<RESOURCE_DATA_BASE>(resData);
			}
															  break;
			case ResourceManager::RESOURCE_TYPE::MAX:
			default:
				break;
			}
		}
	}

	for (int i = 0; i < json_ResourceList["Global"].size(); i++) {
		RESOURCE_TYPE type = json_ResourceList["Global"][i]["TypeID"];
		if (json_ResourceList["Global"][i]["Datas"] == nullptr) continue;
		for (int j = 0; j < json_ResourceList["Global"][i]["Datas"].size(); j++) {
			resourceList_.using_Global[i].insert(json_ResourceList["Global"][i]["Datas"][j]);
			Load(json_ResourceList["Global"][i]["Datas"][j]);
		}
	}

	for (int i = 0; i < json_ResourceList["Scene"].size(); i++) {
		SceneManager::SCENE_ID sceneID = json_ResourceList["Scene"][i]["SceneID"];
		for (int j = 0; j < json_ResourceList["Scene"][i]["UsingResource"].size(); j++) {
			RESOURCE_TYPE type = json_ResourceList["Scene"][i]["UsingResource"][j]["TypeID"];
			if (json_ResourceList["Scene"][i]["UsingResource"][j]["Datas"] == nullptr) continue;
			for (int k = 0; k < json_ResourceList["Scene"][i]["UsingResource"][j]["Datas"].size(); k++) {
				resourceList_.using_Scenes[i][j].insert(json_ResourceList["Scene"][i]["UsingResource"][j]["Datas"][k]);
			}
		}
	}
}

void ResourceManager::LoadResource(SceneManager::SCENE_ID sceneID) {
	std::unordered_set<std::string> usingResourceData;

	for(auto& usingResource : resourceList_.using_Scenes[static_cast<int>(sceneID)]) {
		for (auto& key : usingResource) {
			Load(key);
			usingResourceData.insert(key);
		}
	}

	for (int i = 0; i < currentUsingResource_.size(); i++) {
		if(currentUsingResource_.size() <= 0) continue;
		for (auto key : currentUsingResource_) {
			if (usingResourceData.find(key) == usingResourceData.end()) {
				Delete(key);
			}
		}
	}

	currentUsingResource_ = usingResourceData;
}

void ResourceManager::Release(void) {
	std::vector<std::string> key;
	for (auto itr = resourceData_.begin(); itr != resourceData_.end(); ++itr) {
		key.push_back(itr->first);
	}

	for (int i = 0; i < key.size(); i++) {
		Delete(key[i]);
	}
	resourceData_.clear();
	currentUsingResource_.clear();
}

void ResourceManager::Load(std::string key) {
	if (resourceData_.find(key) != resourceData_.end()) return;

	//生成したリソースのポインタ格納用
	std::shared_ptr<HandleBase> res;

	switch (resourceList_.resourceDatas[key]->type) {
	case ResourceManager::RESOURCE_TYPE::GRAPHIC: {
		res = std::make_shared<Graphic>(resourceList_.resourceDatas[key]->fullPath);
		break;
	}
	case ResourceManager::RESOURCE_TYPE::SOUND:{
		auto resData = static_cast<RESOURCE_DATA_SOUND*>(resourceList_.resourceDatas[key].get());
		if(resData->dimensionType == Sound::DIMENSION_TYPE::_3D) {
			SetCreate3DSoundFlag(true);
			res = std::make_shared<Sound>(
				resourceList_.resourceDatas[key]->fullPath,
				resData->dimensionType
				);
			SetCreate3DSoundFlag(false);
		}
		else {
			res = std::make_shared<Sound>(
				resourceList_.resourceDatas[key]->fullPath,
				resData->dimensionType
				);
		}
		break;
	}
	case ResourceManager::RESOURCE_TYPE::FONT: {
		if(resourceList_.resourceDatas[key]->extension == "otf" || resourceList_.resourceDatas[key]->extension == "ttf"){
			res = std::make_shared<Font>(resourceList_.resourceDatas[key]->fullPath, resourceList_.resourceDatas[key]->fileName, 64);
		}
		else {
			/*int* graphArray = new int[10*1];
			CreateDivGraphFromMem(decodeFile.data, decodeFile.size, 10*1, 10, 1, 40, 64, &graphArray[0]);
			res = std::make_shared<ImageFont>(graphArray, 10,1,40,64,0x30);*/
		}
		break;
	}
	case ResourceManager::RESOURCE_TYPE::MODEL: {
		res = std::make_shared<Model>(resourceList_.resourceDatas[key]->fullPath);
		break;
	}
	case ResourceManager::RESOURCE_TYPE::PIXEL_SHADER: {
		res = std::make_shared<PixelShader>(resourceList_.resourceDatas[key]->fullPath);
		break;
	}
	case ResourceManager::RESOURCE_TYPE::VERTEX_SHADER: {
		res = std::make_shared<VertexShader>(resourceList_.resourceDatas[key]->fullPath);
		break;
	}
	default:
		break;
	}

	//どこからも参照されなくなったら自動的にハンドルを破棄する
	res->SetIsAutoDeleteHandle(true);

	resourceData_[key] = res;
}

void ResourceManager::Delete(std::string key) {
	if (resourceData_.find(key) == resourceData_.end()) return;
	resourceData_.erase(key);
}

std::shared_ptr<HandleBase> ResourceManager::Get(std::string key) const {
	return resourceData_.at(key);
}

//void ResourceManager::OutPutLoadHandleName(void) {
//	std::string resultMsg;
//
//	resultMsg += "Graphic";
//	for (auto itr = graphics_.begin(); itr != graphics_.end(); ++itr) {
//		resultMsg += "\n	" + itr->first;
//	}
//	resultMsg += "\n\n";
//
//	resultMsg += "Sound";
//	for (auto itr = sounds_.begin(); itr != sounds_.end(); ++itr) {
//		resultMsg += "\n	" + itr->first;
//	}
//	resultMsg += "\n\n";
//
//	resultMsg += "3DModel";
//	for (auto itr = models_.begin(); itr != models_.end(); ++itr) {
//		resultMsg += "\n	" + itr->first;
//	}
//	resultMsg += "\n\n";
//
//	std::ofstream ofs("ResourceData.txt");
//	ofs.write(resultMsg.c_str(), resultMsg.length());
//	ofs.close();
//}
