#pragma once
#include <unordered_map>
#include "memory"
#include"../../../Manager/Controller/PlayerController.h"
#include"../../../Manager/GameManager.h"
#include"../../../Manager/LoadOut.h"
#include "../../ObjectBase.h"
#include"../../../Object/Gun/GunType.h"
#include"../../../Object/Grenade/GrenadeType.h"
#include "../ActorBase.h"
#include "../../Gun/GunBase.h"
#include "../../Grenade/GrenadeBase.h"
#include "../../../Common/Count.h"

class AnimatorController;
class Recoil;

class Player : public ActorBase
{

public:


	enum class ACTION
	{
		IDLE,		// �ҋ@
		WALK_FORWARD,	// ����(�O)
		WALK_LEFT,		// ����(��)
		WALK_RIGHT,		// ����(�E)
		WALK_BACK,		// ����(��)
		RUN,		// ����
		//JUMP_UP,		// �W�����v�J�n
		JUMP_NOW,		// �W�����v��
		SHOT,		// ����
		RELOAD,		// �����[�h
		GRENADE,		// �O���l�[�h
		//DAMAGE,		// �_���[�W
		DEAD,		// ����
		// �ړ����Ȃ���֘A
		
		// ���Ⴊ�݊֘A�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[
		CROUCH,				// ���Ⴊ��
		CROUCH_WALK_FORWARD,// ���Ⴊ�ޕ���(�O)
		CROUCH_LEFT,		// ���Ⴊ�ޕ���(��)
		CROUCH_RIGHT,		// ���Ⴊ�ޕ���(�E)
		CROUCH_BACK,		// ���Ⴊ�ޕ���(��)
		CROUCH_RELOAD,		// ���Ⴊ�ރ����[�h
		CROUCH_SHOT,		// ���Ⴊ�ތ���

	};

	// �v���C���[�Փ˗p����
	enum class BODY
	{
		HEAD,		// ��
		CHEST,		// ��
		WAIST,		// ��
		ARM_LEFT,	// ���r
		WRIST_LEFT,	// �����
		ARM_RIGHT,	// �E�r
		WRIST_RIGHT,// �E���
		THIGH_LEFT,	// ��������
		SHIN_LEFT,	// ������
		THIGH_RIGHT,// �E������
		SHIN_RIGHT,	// �E����
		CAP,		// �}�b�v�Ƃ̏Փ˂Ɏg�p
		TOPLINE, // �}�b�v�Ƃ̏Փ˂Ɏg�p(����)
		DOWNLINE,// �}�b�v�Ƃ̏Փ˂Ɏg�p(����)
		EYE,	// ����	
		RAY,	// �ː�
		MAX,
	};


	// �ړ��X�s�[�hMAX
	static constexpr float WARK_SPEED_MAX = (350.0f);

	// ����(accelerator=�A�N�Z�����[�^�[)
	static constexpr float MOVE_ACC = (40.0f);

	//// ����(decelerate=�f�B�Z�����C�g)
	//static constexpr float MOVE_DEC = (0.2f);

	// �����x
	static constexpr float ACCELERATION = (0.1f);

	// �O���l�[�h�𓊂����
	static constexpr float GRENADE_POW = (20.0f);

	// �����񕜂���܂ł̎���
	static constexpr float AUTO_HEAL_TIME = (7.0f);

	// 1�b�Ԃɉ񕜂���HP
	static constexpr float HEAL_HP = (20.0f);

	// HP���
	static constexpr float MAX_HP = (100.0f);

	// ���X�|�[������܂ł̎���
	static constexpr float RESPAWN_COOL_TIME = (7.0f);

	// �f�X�J�����ɐ؂�ւ���܂ł̎���
	static constexpr float CHANGE_DEATHCAMERA_TIME = (2.0f);

	// ���G����
	static constexpr float INVINCIBLE_TIME = (7.0f);


	static constexpr int GRENADE_MAX = (5);

	// ������炷�Ԋu(MAX�X�s�[�h��)
	static constexpr float SOUND_FOOT = (0.35f);

	Player(LOADOUT load0ut, VECTOR resPos, VECTOR resRot,
		int key = -1, ID id = ID::PLAYER_1, int team = -1, float moveControlPos = 0.0f, float actionControlPos = 0.0f);
	~Player(void);

	// �ϋv�l����
	void Damage(float damage) override;

	// �O���l�[�h�𑝉�
	void GetGrenade(GRENADE_TYPE type , int num = 1);

	// �J���������pPos���擾
	VECTOR GetSyncCameraPos(void)const;

	// �J���������pRot���擾
	const VECTOR& GetSyncCameraRot(void)const;
	VECTOR* GetCameraDir(void);

	// �e�̍��W���擾
	const VECTOR& GetGunPos(void)const;
	const Quaternion& GetGunRot(void)const;
	
	const bool& IsHide(void) const;

	// �A�j���[�V�����I���㏈��
	void AnimEnd(int id) override;

	// �A�j���[�V�����ʒm
	void AnimNotice(int id)override;

	// ������������
	void AddRecoilPower(float pow);

	// ADS����
	const bool IsADS(void);
	const bool IsMe(void);
	const bool IsAct(void);

	//const VECTOR& GetCenterPos(void) const;
	VECTOR* GetHeadPos(void) const;
	VECTOR* GetCenterPos(void) const;
	const VECTOR& GetLatestTargetPos(void) const;

	const int& GetKey(void) const;

	//VECTOR GetBulletVec(void) const;
	int GetRandSeed(void) const;

	const int& GetTeam(void) const;
	const float& GetControlPosZ(void) const;

	void UpdateAfterCollision(void) override;

	std::vector<std::shared_ptr<GrenadeBase>>& GetGrenades(void);
	std::shared_ptr<GunBase> GetGun(void);

	// �Փ˂����Ƃ��̏���
	void OnHit(std::weak_ptr<Collider> obj) override;
	void OnHit(std::weak_ptr<Collider> objA, std::weak_ptr<Collider> objB) override;
	void OnHit(MV1_COLL_RESULT_POLY info) override;
	void OnHit(HITRESULT_LINE info) override;
	void OnHit(VECTOR normal) override;

protected:

	int key_;

	// �f�t�H���g�n���h��
	int modelGrHandle_;
	int modelGrHandle2_;

	// �����p�n���h��
	int colorGrHandle_;

	// �C���v�b�g�R���g���[��
	PlayerController& controller_;
	GameManager& gameManager_;

	// �A�j���[�^�[�R���g���[��
	std::unique_ptr<AnimatorController> animCon_;
	std::shared_ptr<Model> animModels_[(int)ACTION::CROUCH_SHOT];

	// �Փ˔���p
	std::shared_ptr<Collider> colliders_[(int)BODY::MAX];

	// �e
	std::shared_ptr<GunBase> gun_;

	// �O���l�[�h
	std::unordered_map<GRENADE_TYPE,int> greNum_;

	// �O���l�[�h�{��
	std::vector<std::shared_ptr<GrenadeBase>> gres_;

	LOADOUT loadOut_;

	// ����Ɏ����Ă���O���l�[�h
	std::weak_ptr<GrenadeBase> greInHand_;

	// ����
	std::unique_ptr<Recoil> recoil_;

	// �J���������p�I�C���[
	VECTOR cameraRot_;
	VECTOR cameraDir_;

	// ���I�C���[
	VECTOR waistRot_;

	// ���X�|�[���ʒu��]
	VECTOR respawnPos_;
	Quaternion respawnQuaRot_;

	// ���݂̃A�N�V����
	ACTION act_;
	ACTION preAct_;
	ACTION actUp_;
	ACTION preActUp_;

	// �ړ�����
	VECTOR moveDir_;
	VECTOR preMoveDir_;

	// �A�N�V���������ǂ���(�ړ��ȊO)
	bool isAct_;

	// �ړ����x
	float moveSpeed_;

	// �W�����v�����ǂ���
	bool isJump_;

	// �n�C�h�����ǂ���
	bool isHide_;

	bool isPutShotPre_;

	// �e�̏��
	GunBase::STATE gunState_;

	// ���ݑI�����Ă��镐��
	GUN_TYPE gunType_;

	// ���ݑI�����Ă���O���l�[�h
	GRENADE_TYPE greType_;

	// �����񕜃J�E���g
	Count autoHeal_;

	// ���X�|�[���J�E���g
	Count respawn_;

	// �f�X�J�����؂�ւ�
	Count deathCamera_;

	// ���G����
	Count invincibleTime_;

	// ���݂̃`�[��
	int team_;

	// �ړ�������W
	float moveControlPosZ_;
	float actionControlPosZ_;

	// �����Ԋu
	float stepFootSound_;

	virtual void Init_Actor(void) override;
	virtual void Update_Actor(void) override;
	virtual void Draw_Actor(void) override;
	virtual void Release_Actor(void) override;

	// �������`��
	void DrawTrans(void) override;

	// �A�N�V�����ύX
	void ChangeAction(ACTION act);
	void ChangeActionUp(ACTION actUp);

	// �e��ԕύX
	void ChangeGunState(GunBase::STATE state);

	// �O���l�[�h�ύX 
	void ChangeNextGrenade(void);

	// �A�j���[�V�����ݒ�
	void AnimationSetting(void);

	// �R���C�_�[�ݒ�
	void ColliderSetting(void);

	// ���S������
	void ProcessDead(void) override;

	// ���X�|�[������
	void Respawn(void);

	// ������
	void AutoHeal(void);

	// ���S������
	void DeadUpdate(void);

	// ����
	virtual void ProcessAction(void);

	// �ړ�����
	virtual void ProcessMove(void);

	// ����(�X�s�[�h��������)
	virtual void Accele(float speed);

	// ����(�X�s�[�h������������)
	virtual void Decelerate(void);

	// ���Ⴊ�ݑ���
	virtual void ProcessCrouch(void);

	// ��]����(���_����)
	virtual void ProcessRotate(void);

	// �W�����v����
	virtual void ProcessJump(void);
	
	// ������
	virtual void ProcessShot(void);

	// ADS����
	virtual void ProcessADS(void);

	// �����[�h����
	virtual void ProcessReload(void);

	// �O���l�[�h����
	virtual void ProcessGrenade(void);

	// �R���C�_�[���W�X�V
	virtual void UpdateCollider(void);

	// ��������
	virtual void UpdateRecoil(void);

	// �O���l�[�h���g��(�g������true��Ԃ�)
	virtual bool UseGrenade(GRENADE_TYPE type);

	// ���������Ȃ�����
	void HideHead(void);
	void DisplayHead(void);

	// ���W��]�ȂǍX�V
	virtual void TransformUpdate(void);

	VECTOR debugPos;

	// ���Q��(�Ō�ɍU�����Ă����I�u�W�F�N�g)
	std::weak_ptr<ObjectBase> lastAtkObj_;

	int test_ = 0;

private:

};