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
	// ���\�[�X�t�@�C���̌^
	using resource_file_list_t = std::unordered_set<std::string>;

	// �A�Z�b�g(���\�[�X�t�@�C��)���i�[����t�H���_�ւ̃x�[�X�̃p�X
	static const std::filesystem::path BASE_ASSETS_PATH;

	// ���\�[�X���X�g��json�t�@�C���ւ̃p�X
	static const std::filesystem::path RESOURCE_LIST_PATH;

	// json�̊e���\�[�X�t�@�C���̎�ނ��w���^�O(json�̃L�[)
	static const std::array<std::string, static_cast<int>(Resource::RESOURCE_FILE_TYPE::MAX)> RESOURCE_FILE_TAG;

	~ResourceManager(void) = default;
	THIS_CLASS_IS_STATIC_SINGLETON(ResourceManager);

	/// @brief ���������� 
	void Init(void);

	/// @brief �o�^����Ă��郊�\�[�X�t�@�C���̃f�[�^��Ԃ�
	/// @param key �擾���������\�[�X�t�@�C���̃L�[
	/// @return �L�[�ɑΉ��������\�[�X�t�@�C���̃f�[�^
	std::weak_ptr<HandleBase> GetResourceFile(const std::string& key);

	/// @brief �w�肳�ꂽ�V�[���ɓo�^����Ă郊�\�[�X�t�@�C����ǂݍ���
	/// @param sceneType �V�[���̎��
	void LoadSceneResourceFile(SceneManager::SCENE_ID sceneType);

private:
	/// @brief ���\�[�X�t�@�C�����Ǘ����邽�߂̃f�[�^
	/// @param resourceType ���\�[�X�t�@�C���̎��
	/// @param filePath ���\�[�X�t�@�C���ւ̃p�X
	/// @param fileName ���\�[�X�t�@�C���̖��O
	/// @param extension ���\�[�X�t�@�C���̊g���q
	struct RESOURCE_FILE_INFO {
		Resource::RESOURCE_FILE_TYPE resourceType;
		std::filesystem::path filePath;
		std::string fileName;
		std::string extension;
	};

	// ���\�[�X�t�@�C�����Ǘ�����A�z�z��(�L�[ : ���\�[�X�t�@�C���̃L�[, �l : ���\�[�X�t�@�C���̃f�[�^)
	std::unordered_map<std::string, RESOURCE_FILE_INFO> resourceFileInfoList_;

	// �S�ẴV�[���Ŏg�����\�[�X�t�@�C���̃L�[���Ǘ�
	// �����ɓo�^���Ă�����̂̓V�[���J�ڂ��Ă��������Ȃ��悤�ɂȂ��Ă܂�
	resource_file_list_t globalUsingResourceList_;

	// �e�V�[�����g�p���郊�\�[�X�t�@�C���̃L�[���Ǘ�
	std::array<resource_file_list_t, static_cast<int>(SceneManager::SCENE_ID::MAX)> sceneUsingResourceList_;

	// ���ݓǂݍ��܂�Ă郊�\�[�X�t�@�C�����i�[(�L�[ : ���\�[�X�t�@�C���̃L�[, �l : ���\�[�X�t�@�C��)
	std::unordered_map<std::string, std::shared_ptr<HandleBase>> currentUsingResourceFiles_;

	ResourceManager(void) = default;

	/// @brief �e���\�[�X�t�@�C���̏���o�^
	/// @param resourceFileListJson ���\�[�X�t�@�C����json
	void RegisterResourceFileInfo(const nlohmann::json& resourceFileListJson);

	/// @brief �S�ẴV�[���Ŏg�p���郊�\�[�X�t�@�C���̃L�[��o�^
	/// @param resourceFileListJson ���\�[�X�t�@�C����json
	void RegisterGlobalUsingResourceList(const nlohmann::json& resourceFileListJson);

	/// @brief �e�V�[���Ŏg�p���郊�\�[�X�t�@�C���̃L�[��o�^
	/// @param resourceFileListJson ���\�[�X�t�@�C����json
	void RegisterUsingResourceList(const nlohmann::json& resourceFileListJson);

	/// @brief �w�肳�ꂽ���\�[�X�t�@�C����ǂݍ���
	/// @param resourceKey ���\�[�X�t�@�C���̃L�[
	/// @return �ǂݍ��܂ꂽ���\�[�X�t�@�C��
	std::shared_ptr<HandleBase> LoadResourceFile(const std::string& resourceKey);
};

