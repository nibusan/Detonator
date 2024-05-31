#pragma once
#include <functional>
#include "EnemyBase.h"
#include "../AnimatorController.h"
#include "../../Gun/GunBase.h"
#include "../../../Utility/Utility.h"
#include "../../../Common/Count.h"

//class AnimatorController;

class Armour;

class EnemyGeorge : public EnemyBase
{

public:

	static constexpr int ARMOUR_NUM = (3);
	static constexpr int SHIELD_NUM = (6);
	static constexpr VECTOR ARMOUR_RELATIVE_POS[ARMOUR_NUM] = { { 0.0f,0.0f,0.0f} ,
																{ 0.0f,100.0f,0.0f } ,
																{ 0.0f,200.0f,0.0f } };
	

	// ����̍L��
	static constexpr float VIEW_RANGE = (1400.0f);
	static constexpr float VIEW_RANGE_EXPANSION = (5000.0f);

	// ����p
	static constexpr float VIEW_ANGLE = (30.0f);
	static constexpr float VIEW_TIME = (5.0f);

	// ���o�̍L��
	static constexpr float HEARING_RANGE = (700.0f);
	static constexpr float HEARING_RANGE_EXPANSION = (2500.0f);
	static constexpr float HEARING_SEARCH_TIME = (3.0f);

	// ��_�����C�ɂ��鎞��
	static constexpr float DAMAGE_CARE_TIME = (5.0f);

	// ���p�n�_
	static constexpr int TARGET_NUM = (10);
	static constexpr float TARGET_WAIT_TIME = (2.0f);
	static constexpr VECTOR TARGET_POS[TARGET_NUM] = { { 100.0f,0.0f,-240.0f} ,
													   { 100.0f,0.0f,1180.0f } ,
													   { 750.0f,0.0f,1180.0f } ,
	{ 750.0f,0.0f,2130.0f } ,{ -970.0f,0.0f,2130.0f } ,{ -1440.0f,0.0f,1600.0f } ,
	{ -1440.0f,0.0f,1240.0f } ,
	{ -1030.0f,0.0f,850.0f } ,
	{ -1270.0f,0.0f,40.0f } ,
	{ -400.0f,0.0f,-770.0f } };

	// ��]
	static constexpr float ROTATE_POW = (1.0f / 180.0f * DX_PI_F);

	enum class ACTION
	{
		IDLE,		// �ҋ@
		WALK,
		RUN,
		SHOT,
		RELOAD,
		DEAD
	};

	// �Փ˗p����
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
		EYE,
		MAX,
	};

	enum class STATE
	{
		IDLE,
		ROTATE,
		MOVE,
		//RUN,
		SHOT,
		SEARCH
	};


	EnemyGeorge(std::vector<VECTOR*> followPos, bool isFirst);
	~EnemyGeorge(void) = default;

	void Update_EnemyAfterCollision(void)override;

	// �G���Z�b�g���̏�����
	void EnemyReset(void);

	const VECTOR& GetFollowPos(void) const;
	VECTOR GetPosForCamera(void)const;


	std::shared_ptr<GunBase> GetGun(void);

	// �A�j���[�V�����ʒm
	void AnimNotice(int id)override;

	// �Փ˂����Ƃ��̏���
	void OnHit()override;
	void OnHit(std::weak_ptr<Collider> obj) override;
	void OnHit(std::weak_ptr<Collider> objA, std::weak_ptr<Collider> objB) override;
	void OnHit(MV1_COLL_RESULT_POLY info) override;
	void OnHit(HITRESULT_LINE info) override;
	void OnHit(VECTOR normal) override;

private:

	// �f�t�H���g�n���h��
	int modelGrHandle_;
	int modelGrHandle2_;

	// �����p�n���h��
	int colorGrHandle_;

	// �Ǐ]�Ώ�
	std::vector<VECTOR*> followPos_;

	// �A�j���[�^�[�R���g���[��
	std::unique_ptr<AnimatorController> animCon_;

	// ���݂̃A�N�V����
	ACTION act_;
	ACTION preAct_;
	ACTION actUp_;
	ACTION preActUp_;

	// �A�N�V���������ǂ���(�ړ��ȊO)
	//bool isAct_;

	// �Փ˔���p
	std::shared_ptr<Collider> colliders_[(int)BODY::MAX];

	// ���Q��(�Ō�ɍU�����Ă����I�u�W�F�N�g)
	std::weak_ptr<ObjectBase> lastAtkObj_;

	// �e
	std::shared_ptr<GunBase> gun_;

	// �e�̏��
	GunBase::STATE gunState_;

	// �V�[���h
	std::shared_ptr<Armour> armours_[ARMOUR_NUM];

	STATE state_;
	STATE preState_;

	// ��ԊǗ�(�X�V�X�e�b�v)
	std::function<void(void)> stateUpdate_;

	Count waitTime_;
	Count searchHearingTime_;
	Count searchViewTime_;
	Count damageCareTime_;
	
	int targetIndex_;
	VECTOR targetPos_;
	VECTOR targetGoalRot_;

	bool isRotate_;

	// ���I�C���[
	VECTOR waistRot_;

	// ���m�t���O
	bool isNotice_;
	bool isNoticeHearing_;
	bool isNoticeDamage_;

	int aliveShieldNum_;

	void Init_Enemy(void)override;
	void Update_Enemy(void)override;
	void Draw_Enemy(void)override;
	void Release_Enemy(void)override;

	// ���W��]�ȂǍX�V
	void TransformUpdate(void);

	// �A�j���[�V�����ݒ�
	void AnimationSetting(void);

	// �A�N�V�����ύX
	void ChangeAction(ACTION act);
	void ChangeActionUp(ACTION actUp);

	// �R���C�_�[�ݒ�
	void ColliderSetting(void);

	// �R���C�_�[���W�X�V
	void UpdateCollider(void);

	// ���S������
	void ProcessDead(void)override;

	// ���S������
	void DeadUpdate(void);

	
	void ChangeState(STATE state);

	void ChangeGunState(GunBase::STATE state);


	void SearchUpdate(void);
	void SearchExpansionUpdate(void);

	void Idle(void);
	void Rotation(void);
	void WaistRotate(float goalRotX);
	void LazyRotationX(float goalRotX);
	void LazyRotationY(float goalRotY);
	void Move(void);
	void Shot(void);
	void Search(void);


};

