#pragma once
#include <DxLib.h>
#include <vector>
#include "../../Common/Quaternion.h"

class Transform;
class ObjectBase;

class Collider
{

public:

	// �Փ˔�����
	enum class OBJECT_TYPE
	{
		PLAYER_CAP,		// �}�b�v�Ƃ̏Փ˂Ɏg�p
		PLAYER_TOPLINE, // �}�b�v�Ƃ̏Փ˂Ɏg�p(����)
		PLAYER_DOWNLINE,// �}�b�v�Ƃ̏Փ˂Ɏg�p(����)
		PLAYER_HEAD,	// �v���C���[�̓�
		PLAYER_BODY, 	// �v���C���[�̑�
		PLAYER_LIMBS, 	// �v���C���[�̎葫
		PLAYER_BULLET, 	// �v���C���[�̒e
		PLAYER_EYE, 	// �v���C���[�̎���(�J�����������_)
		ENEMY,			// �G
		ENEMY_EYE,		// �G�̎���
		ENEMY_BULLET,	// �G�̒e
		MAP,			// �}�b�v(�I�u�W�F�N�g)
		MAP_UNBREAK,	// �}�b�v(�j��s��-�ǒn��)
		GRENADE_SPHERE,	// �O���l�[�h(����)
		GRENADE_LINE,	// �O���l�[�h(����)
		EXPLOSION,		// ����
		SMOKE,			// �X���[�N
		CAMERA,			// �J����
		SHIELD,			// �V�[���h
		RAY,			// �ː�
	};

	// �Փ˃^�C�v
	enum class COL_TYPE
	{
		NONE,
		MODEL,		// ���f��
		SPHERE,		// ��
		CAPSULE,	// �J�v�Z��
		LINE,		// ����
		MAX
	};


	Collider(std::weak_ptr<ObjectBase> obj);
	~Collider(void);

	// �ʒu�X�V��
	virtual void Update(VECTOR followPos, Quaternion rot);
	virtual void Update(VECTOR followPos, VECTOR rot);

	// 1�t���[���O�X�V
	// �X�V�O���W�����g���ꍇ
	// ���̊֐����e�X�V������1�ԍŏ���
	// 1�x�����Ăяo��
	virtual void PreUpdate(void);

	// �I�u�W�F�N�g�^�C�v��ύX
	void ChangeObjectType(OBJECT_TYPE type);

	// �Փ˂��Ă����I�u�W�F�N�g�z���������(���t���[���Ăяo��)
	void ClearHitObject(void);

	// �f�o�b�O�p�`��
	virtual void DrawDebug(void);

	// �Փ˔�����s����
	bool isCollision;

	// �I�u�W�F�N�g�̎��
	OBJECT_TYPE objType;

	// �Փ˃^�C�v
	COL_TYPE colType;

	// ���W
	VECTOR pos;

	// ���΍��W
	VECTOR relativePos;

	// ��]
	Quaternion quaRot;

	// �傫��
	VECTOR scl;

	// �X�V�O�v�f�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[

	// �傫��
	VECTOR preScl;
	// ��]
	Quaternion preQuaRot;
	// �ʒu
	VECTOR prePos;

	// �[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[

	// �e�̃|�C���^
	std::weak_ptr<ObjectBase> parent_;

};

