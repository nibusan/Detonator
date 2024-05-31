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
	static const std::string RESOURCE_PATH;	//���\�[�X�̃p�X
	static const std::string WWDATA;		//�t�@�C���̊g���q

											//�ǂݍ��ރ��\�[�X�̎��
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

	// ���\�[�X�f�[�^�̃x�[�X
	class RESOURCE_DATA_BASE {
	public:
		RESOURCE_TYPE type;
		std::filesystem::path path;
		std::string fileName;
		std::string extension;
		std::filesystem::path fullPath;
	};

	// �T�E���h�̃��\�[�X�f�[�^
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

	//���\�[�X���i�[����z��
	std::unordered_map<std::string, std::shared_ptr<HandleBase>> resourceData_;

	//���݃V�[���Ŏg�p���Ă��郊�\�[�X�̃��X�g
	std::unordered_set<std::string> currentUsingResource_;


	//�R���X�g���N�^
	ResourceManager(void);


	/// <summary>
	/// ���\�[�X��ǂݍ���
	/// </summary>
	/// <param name="type"> �L�[ </param>
	void Load(std::string key);
public:
	//�f�X�g���N�^
	~ResourceManager(void);
	THIS_CLASS_IS_STATIC_SINGLETON(ResourceManager);

	void Init(void);

	void LoadResource(SceneManager::SCENE_ID sceneID);

	void Release(void);


	// ���\�[�X��ǂݍ���(�^����ꂽ�z��ɖ��O��ǉ�����@�\�t��)
	//void Load(RESOURCE_TYPE type, int id, std::unordered_set<std::pair<RESOURCE_TYPE, int>>* list);

	/// <summary>
	/// �w�肳�ꂽ���O�̃��\�[�X���폜����
	/// </summary>
	/// <param name="name"> �L�[ </param>
	void Delete(std::string key);

	/// <summary>
	/// �w�肳�ꂽ���O�̃��\�[�X��Ԃ�
	/// </summary>
	/// <param name="type"> ��� </param>
	/// <param name="name"> ���O </param>
	/// <returns></returns>
	[[nodiscard]] std::shared_ptr<HandleBase> Get(std::string key) const;
};