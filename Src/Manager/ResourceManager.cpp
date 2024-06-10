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
	// ���\�[�X�t�@�C�����X�g(json)�̓ǂݍ���
	const auto resourceListJson = JsonReader::GetInstance().Parse(RESOURCE_LIST_PATH);

	// �e���\�[�X�t�@�C���̏���o�^
	RegisterResourceFileInfo(resourceListJson);

	// �S�ẴV�[���Ŏg�p���郊�\�[�X�t�@�C����o�^�������X�g��ǂݍ���
	RegisterGlobalUsingResourceList(resourceListJson);

	// �e�V�[���̎g�p���郊�\�[�X�t�@�C����o�^�������X�g��ǂݍ���
	RegisterUsingResourceList(resourceListJson);

}

std::shared_ptr<HandleBase> ResourceManager::GetResourceFile(const std::string& key) {
	return currentUsingResourceFiles_[key];
}

void ResourceManager::LoadSceneResourceFile(SceneManager::SCENE_ID sceneType) {
	// ���݂̃V�[���̎�ނ��擾
	const auto currentSceneType = SceneManager::GetInstance().GetSceneID();
	const auto& usingResourceList = sceneUsingResourceList_[static_cast<int>(currentSceneType)];

	// �؂�ւ���̃V�[���œǂݍ��ރ��\�[�X�t�@�C���̃��X�g���R�s�[����
	auto nextUsingResourceList = sceneUsingResourceList_[static_cast<int>(sceneType)];

	// �s�v�ȃ��\�[�X�t�@�C�����A�����[�h(�폜)����
	for (auto itr = currentUsingResourceFiles_.begin(); itr != currentUsingResourceFiles_.end(); ) {
		// �L�[���O���[�o���̃��\�[�X�t�@�C���̃��X�g�ɓo�^����Ă�ꍇ�͉������Ȃ�
		if (globalUsingResourceList_.find(itr->first) != globalUsingResourceList_.end()) {
			++itr;
			continue;
		}

		// �L�[���؂�ւ���̃V�[���̃��\�[�X�t�@�C���̃��X�g�ɓo�^����Ă�ꍇ�͉������Ȃ�
		if (nextUsingResourceList.find(itr->first) != nextUsingResourceList.end()) {
			// ���̃V�[���œǂ݂��܂Ȃ��Ă����̂Ń��\�[�X�t�@�C���̃��X�g����폜����
			nextUsingResourceList.erase(itr->first);
			++itr;
			continue;
		}
		else {
			// ���݂��Ȃ��ꍇ�͍폜����
			itr->second.reset();
			itr->second = nullptr;
			itr = currentUsingResourceFiles_.erase(itr);
		}
	}

	// �ǂݍ��܂�ĂȂ����\�[�X�t�@�C��������ǂݍ���
	for (auto itr = nextUsingResourceList.begin(); itr != nextUsingResourceList.end(); ++itr) {
		currentUsingResourceFiles_[(*itr)] = LoadResourceFile((*itr));
	}

}

void ResourceManager::RegisterResourceFileInfo(const nlohmann::json& resourceFileListJson) {
	// �e���\�[�X�t�@�C���̏���o�^
	for (int i = 0; i < static_cast<int>(Resource::RESOURCE_FILE_TYPE::MAX); i++) {
		for (const auto& resourceFile : resourceFileListJson["ResourceList"][ResourceFileTag[i]]) {
			// �i�[�p�̃��\�[�X�t�@�C���̏��𐶐�
			RESOURCE_FILE_INFO info;
			info.resourceType = static_cast<Resource::RESOURCE_FILE_TYPE>(i);
			info.filePath =		// �p�X���v�Z �x�[�X�p�X + ���\�[�X�t�@�C���ւ̃p�X
				BASE_ASSETS_PATH / static_cast<std::filesystem::path>(static_cast<std::string>(resourceFile["FilePath"]));
			info.fileName = resourceFile["FileName"];
			info.extension = resourceFile["Extension"];

			// �o�^���ꂽ���\�[�X�t�@�C���̃L�[���L�[�Ƃ��Ċi�[
			resourceFileInfoList_[resourceFile["Key"]] = info;
		}
	}
}

void ResourceManager::RegisterGlobalUsingResourceList(const nlohmann::json& resourceFileListJson) {
	// �S�ẴV�[���Ŏg�p���郊�\�[�X�t�@�C����o�^�������X�g��ǂݍ���
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
	// �e�V�[���̎g�p���郊�\�[�X�t�@�C����o�^�������X�g��ǂݍ���
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
	// �ǂݍ��񂾃��\�[�X�t�@�C�����i�[
	std::shared_ptr<HandleBase> resourceFile;

	// �ǂݍ��ރ��\�[�X�t�@�C���̏����擾
	const auto& resourceFileInfo = resourceFileInfoList_.at(resourceKey);

	// �e���\�[�X�t�@�C���̓ǂݍ��ݏ���
	switch (resourceFileInfo.resourceType) {
	case Resource::RESOURCE_FILE_TYPE::IMAGE: {
		// �摜
		resourceFile = std::make_shared<Graphic>(
			resourceFileInfo.filePath / 
			(resourceFileInfo.fileName + 
				resourceFileInfo.extension)
		);
		break;
	}
	case Resource::RESOURCE_FILE_TYPE::SOUND_2D: {
		// 2D����
		resourceFile = std::make_shared<Sound>(
			resourceFileInfo.filePath / 
			(resourceFileInfo.fileName + 
				resourceFileInfo.extension),
			Sound::DIMENSION_TYPE::_2D
		);
		break;
	}
	case Resource::RESOURCE_FILE_TYPE::SOUND_3D: {
		// 3D����
		resourceFile = std::make_shared<Sound>(
			resourceFileInfo.filePath / 
			(resourceFileInfo.fileName + 
				resourceFileInfo.extension),
			Sound::DIMENSION_TYPE::_3D
		);
		break;
	}
	case Resource::RESOURCE_FILE_TYPE::MODEL: {
		// 3D���f��
		resourceFile = std::make_shared<Model>(
			resourceFileInfo.filePath / 
			(resourceFileInfo.fileName + 
				resourceFileInfo.extension)
		);
		break;
	}
	case Resource::RESOURCE_FILE_TYPE::FONT: {
		// �t�H���g
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
		// �s�N�Z���V�F�[�_�[
		resourceFile = std::make_shared<PixelShader>(
			resourceFileInfo.filePath / 
			(resourceFileInfo.fileName + 
				resourceFileInfo.extension)
		);
		break;
	}
	case Resource::RESOURCE_FILE_TYPE::VERTEX_SHADER: {
		// ���_�V�F�[�_�[
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

	// �f�X�g���N�^���Ă΂ꂽ�ۂɎ����Ńn���h�����������悤�ɂ���
	resourceFile->SetIsAutoDeleteHandle(true);

	return resourceFile;
}
