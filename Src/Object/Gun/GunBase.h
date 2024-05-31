#pragma once
#include <memory>
#include <vector>
#include "../ObjectBase.h"
#include "../Bullet/BulletBase.h"
#include "../../Net/NetManager.h"

class ActorBase;

class GunBase : public ObjectBase
{

public:

	enum class STATE
	{
		NONE,
		SHOT,
		//SHOT_ADS,
		RELOAD,
	};

	// �ő呕�U��
	static constexpr int MAX_MAGAZINE_DEF = { 30 };

	// �e��]�ɂ����鎞��
	static constexpr float ROTATE_TIME = { 0.5f };

	// �����_�J��
	static constexpr float TARGET_CHANGE_TIME = { 1.0f };

	GunBase(const Transform& follow, int frameNo, std::weak_ptr<ActorBase> parent);
	~GunBase(void);

	void UpdateAfterCollision(void) override;

	virtual void DrawTrans(void) override;

	// �ʒu��]�X�V
	void UpdateTransform(void);

	// �e���ꔭ����
	void Shot(void);
	void ShotUpdate(void);

	// ���݂̃��R�C���p�^�[���擾
	const Recoil::RECOIL_TYPE GetRecoilType(void) const;
	const int GetMagazine(void) const;
	const int GetMaxMagazine(void) const;

	// �����̑傫�����擾
	const float GetRecoilMag(void) const;

	// �A�����Č������������Z�b�g
	void ResetBurstNum(void);

	// �e�����Z�b�g
	void ReloadGun(void);

	// �e�̏�ԕύX
	void ChangeState(STATE state, float time = 0.0f);

	// ��]�����ǂ���
	bool IsRotate(void) const;

	// �e�������邩�ǂ���
	bool IsRemainMagazine(void)const;

	// �e�����ˉ\���ǂ���
	bool IsShootable(void)const;

	// �g��{���擾
	const float& GetMagnification(void)const;

	virtual void PlayReloadSound(void) = 0;

	const VECTOR& GetBarrelPos(void)const;
	const float& GetRandLength(void)const;
	const float& GetRandRadius(void)const;
	const float& GetShotDelayTime(void)const;
	VECTOR GetFoward(void)const;
	virtual const VECTOR GetScopePos(void)const;

	std::vector<std::shared_ptr<BulletBase>>& GetBullets(void);

protected:

	NetManager& netManager_;

	bool isShot_;

	// ���L���̃|�C���^
	std::weak_ptr<ActorBase> parent_;

	// �G�t�F�N�gID
	std::vector<int> effectID_;

	// ���L����key(�ʐM�Ŏg�p)
	int parentKey_;

	// ���˕������΂炯������ۂ̎n�_����I�_�܂ł̒���
	float randLength_;
	// ���˕������΂炯������ۂ̉~�̔��a
	float randRadius_;

	// ��������
	float hipRandLength_;
	float hipRandRadius_;


	// �X�R�[�v�֘A�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[

	std::unique_ptr<Model> scopeModel_;
	// �X�R�[�v�̃��f��ID
	int scopeModelId_;

	// �X�R�[�v���f���t�@�C����
	std::string scopeFileName_;

	// �X�R�[�v�pTransform
	Transform scpTransform_;

	// �X�R�[�v���΍��W
	VECTOR scpRelativePos_;

	// �X�R�[�v�g��{��
	float magnification_;

	// �f�o�b�O
	VECTOR deg_;

	// �[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[
	
	// �e�֘A�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[
	// �e
	std::vector<std::shared_ptr<BulletBase>> bullets_;

	// �e�̃��f���t�@�C����
	std::string bulletFileName_;

	// �e�̃��f��ID
	int bulletModelId_;
	// �[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[

	// �Ǐ]�Ώ�
	const Transform& follow_;

	// �Ǐ]�t���[���ԍ�
	int followFrameNo_;

	// ���΍��W
	VECTOR relativePos_;

	// ���f���t�@�C����
	std::string fileName_;

	// �e�̏��
	STATE state_;

	STATE preState_;

	// �o�����t���[���ԍ�(�e�̔��ˈʒu)
	int barrelFrameNo_;

	// �o�������΍��W
	VECTOR barrelRelativePos_;

	// �e�̔��ˈʒu
	VECTOR barrelPos_;

	// �ő�e��
	int maxMagazine_;

	// ���ˊԊu
	float shotDelay_;
	float shotDelayCnt_;

	// ���R�C���p�^�[��
	Recoil::RECOIL_TYPE recoil_[MAX_MAGAZINE_DEF];

	// ���ˎ��̔����̑傫��
	float recoilMag_;

	// �A�����Č�������
	int burstCnt_;

	// �e��
	int magazine_;

	// ��]�����ǂ���
	bool isRotate_;

	// ��]�J�n����qua
	Quaternion startQuaRot_;

	// ��]����
	float rotateRate_;
	float rotateTime_;
	float rotateTimeCnt_;

	//
	VECTOR targetPos_;
	VECTOR goalTargetPos_;

	// �J��
	float changeCnt_;
	
	void Init_Object(void) override;
	void Update_Object(void) override;
	void Draw_Object(void) override;
	void Release_Object(void) override;

	// �e���Ǝ��̐ݒ�
	virtual void SetParam(void) = 0;
	virtual void PlayBurstSound(void) = 0;

	// �e��ΏۂɌ����邩�Ǐ]
	void Follow(void);

	// �ʏ펞�̒Ǐ](������̃t���[��������)
	virtual void FollowNone(void);

	// ���C���̒Ǐ](�����_������)
	virtual void FollowShot(void);

	// �����[�h���̒Ǐ]
	virtual void FollowReload(void);

	// �ڕW�̊p�x�Ɍ����ď��X�ɉ�]
	void Rotation(Quaternion goalRot);

	// �ڕW�̃S�[���Ɍ����ď��X�Ɉړ�
	//void Target(VECTOR goalTargetPos);

	// ���ˊԊu����
	void ShotDelay(void);

	// �e����
	virtual void CreateBullet(void) = 0;

	// �e�𔭎ˏ�Ԃɐݒ�
	virtual void SetBullet(std::shared_ptr<BulletBase> bullet);

};
