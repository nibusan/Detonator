#pragma once
#include <filesystem>
#include <array>
#include <unordered_map>
#include <utility>
#include <DxLib.h>
#include "../Common/Handle/Effect/EffectBase.h"
#include "../Common/Handle/Effect/2D/Effect2D.h"
#include "../Common/Handle/Effect/3D/Effect3D.h"
#include "../Manager/GameManager.h"

class EffectManager
{

public:
	
	// ���
	enum class EFFECT_TYPE
	{
		BUBBLE,
		MAGMA,
		UTSUHO,
		SMOKE,
		DESTROY,
		BULLETSMOKE,
		BULLET,
		DAMAGE,
		BARRIER,
		BARRIERLIGHT,
		BULLETBOOST,
		EXPLOSION,
		MAX
	};

	// �G�t�F�N�g�̃f�[�^
	struct EFFECT
	{
		EFFECT_TYPE type;	// ���
		VECTOR pos;			// ���W
		VECTOR scl;			// �X�P�[��
		VECTOR rot;			// �p�x
		float speed;		// �A�j���[�V�����̑��x
	};


	// �C���X�^���X�̐���
	static void CreateInstance(void);

	// �C���X�^���X�̎擾
	static EffectManager& GetInstance(void);

	void Init(void);
	void Update(void);
	void Draw(void);
	void Release(void);

	// ���\�[�X�̔j��
	void Destroy(void);
	
	/// @brief �G�t�F�N�g��ǉ�����
	/// @param effect �G�t�F�N�g�̃f�[�^
	/// @return	�G�t�F�N�g�̃n���h��ID
	int AddEffect(EFFECT& effect);

	/// @brief �w�肳�ꂽ�G�t�F�N�g���폜
	/// @param id AddEffect�ŕԂ��Ă����n���h��ID
	void DeleteEffect(int id);

	/// @brief �w�肳�ꂽ�G�t�F�N�g�̃f�[�^��Ԃ�
	/// @param id AddEffect�ŕԂ��Ă����n���h��ID
	/// @return �G�t�F�N�g��weak_ptr
	const std::weak_ptr<EffectBase> GetData(int id) const;

private:
	// 2D or 3D
	enum class DIMENSION_TYPE
	{
		_2D,
		_3D
	};

	// �G�t�F�N�g�̃f�[�^
	struct EFFECT_DATA
	{
		std::filesystem::path path;
		DIMENSION_TYPE dimensionType;
		std::shared_ptr<EffectBase> resource;
	};

	// �ÓI�C���X�^���X
	static EffectManager* instance_;

	// �G�t�F�N�g���X�g
	std::unordered_map<int, EFFECT_DATA> effectList_;

	// �Đ�����Ă�G�t�F�N�g
	std::unordered_map<int, std::shared_ptr<EffectBase>> effects_;

	// �f�t�H���g�R���X�g���N�^��private�ɂ��āA
	// �O�����琶���ł��Ȃ��l�ɂ���
	EffectManager(void);

	// �R�s�[�R���X�g���N�^�����l
	EffectManager(const EffectManager&);

	// �f�X�g���N�^�����l
	~EffectManager(void) = default;

	// �G�t�F�N�g���X�g�̓ǂݍ���
	void LoadEffectList(void);

	// �G�t�F�N�g���X�g����w�肳�ꂽ�G�t�F�N�g�̃��\�[�X��Ԃ�
	const EFFECT_DATA& GetEffectResource(EFFECT_TYPE type);

	int GetPlay2DEffectHandle(const EFFECT_DATA& resourceEffect) const;
	int GetPlay3DEffectHandle(const EFFECT_DATA& resourceEffect) const;
};