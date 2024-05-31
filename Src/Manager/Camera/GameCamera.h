#pragma once
#include "CameraBase.h"
#include "../../Net/NetManager.h"

class GameCamera : public CameraBase
{
public:

	static constexpr int MAX_PLAYER = (4);

	// �J�����X�s�[�h�FNEAR
	static constexpr float SPEED = 20.0f;

	// �⊮����
	static constexpr float LERP_TIME_DEF = (1.0f);
	
	// �J�����̏������W
	static constexpr VECTOR DEFAULT_CAMERA_POS = { 0.0f,20.0f,13.0f };

	// �J�����ʒu���璍���_�܂ł̑��΍��W
	static constexpr VECTOR RELATIVE_C2T_POS = { 0.0f, 0.0f, 1000.0f };
	
	// FPS���J�����ʒu���璍���_�܂ł̑��΍��W
	static constexpr VECTOR RELATIVE_POS_FPS = { 0.0f, 0.0f, 1000.0f };

	// ADS���J�����ʒu���璍���_�܂ł̑��΍��W
	static constexpr VECTOR RELATIVE_POS_ADS = { 0.0f, 0.0f, 1000.0f };

	// DEATH���J�����ʒu���璍���_�܂ł̑��΍��W
	static constexpr VECTOR RELATIVE_POS_DEATH = { 0.0f, 150.0f, -300.0f };

	// �J����FOV
	static constexpr float FOV_DEFAULT = (1.0f);

	// �J�����V�F�C�N���Ɋ�ƂȂ鋗��
	// �Ǐ]�ΏۂƐk���n�Ƃ̋��������̒l����
	// �傫���Ȃ�ق�ShakePow�͏�����
	// �������Ȃ�ق�ShakePow�͑傫���Ȃ�
	// �܂�A���S�n�ɋ߂��قǗh�ꂪ�傫���Ȃ�
	static constexpr float SHAKE_DIS = (100.0f);

	// �J�������[�h
	enum class MODE
	{
		NONE,
		FPS,			// ��l�̃��[�h
		ADS,			// ADS(�Ə�)
		DEATH,			// �f�X�J����
		FIXED_POINT,	// ��_�J����
		DEBUG,			// �f�o�b�O�p�J����
	};

	struct MultiCameraInfo 
	{
		std::weak_ptr<ObjectBase> follow_;
		MODE mode_ = MODE::NONE;
		VECTOR pos_ = {};
		Quaternion rot_ = {};
		VECTOR targetPos_ = {};
		VECTOR gPos_ = {};
		Quaternion gRot_ = {};
		VECTOR gTargetPos_ = {};
		// �V�F�C�N
		float shakeTime_ = 0.0f;
		float shakePow_ = 0.0f;
		// �J�ڎ���
		float lerpTime_ = 0.0f;
		float cntLerpTime_ = 0.0f;
	};

	GameCamera(void);
	~GameCamera(void);

	void Init(void) override;
	void Update(void);
	void NetUpdate(void) override;

	// �J��������
	void SetBeforeDraw(void) override;
	void SetBeforeDrawFPS(void);
	void SetBeforeDrawADS(void);
	void SetBeforeDrawDeath(void);
	void SetBeforeDrawFixedPoint(void);
	void SetBeforeDrawDebug(void);

	void SetBeforeDrawMulti(void);
	void SetBeforeDrawFPSMulti(int key);
	void SetBeforeDrawADSMulti(int key);
	void SetBeforeDrawDeathMulti(int key);
	void SetBeforeDrawFixedPointMulti(int key);

	// �V�F�C�N
	void UpdateSheke(void);
	void UpdateShekeMulti(void);
	void Shake(float time, float pow, VECTOR pos);
	void ShakeMulti(int key, float time, float pow, VECTOR pos);

	void Draw(void);
	void Release(void);

	VECTOR GetTargetPos(void) const;
	const MODE& GetMode(void)const;
	const MODE& GetMode(int key)const;

	VECTOR GetCameraDir(void) const;

	// �J�������[�h�̕ύX(�⊮���������ꍇ�l��n���K�v������)
	void ChangeMode(MODE mode, float time = 0.0f);
	void ChangeMode(MODE mode, std::weak_ptr<ObjectBase> follow, float time = 0.0f , float fav = FOV_DEFAULT);
	void ChangeMode(MODE mode, VECTOR fixedPos, VECTOR fixedRot, float time = 0.0f);

	void ChangeMode(int key, MODE mode, float time = 0.0f);
	void ChangeMode(int key, MODE mode, std::weak_ptr<ObjectBase> follow, float time = 0.0f);

	// �Ǐ]�Ώۂ̐ݒ�
	void SetFollowTarget(std::weak_ptr<ObjectBase> follow);
	void SetFollowTarget(int key, std::weak_ptr<ObjectBase> follow);

	// �X�V��̃J�������֘A�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[

	VECTOR GetLatestPos(int key);
	VECTOR GetLatestTargetPos(int key);

	// �J��������
	void SetBeforeDrawLatest(int key, VECTOR& pos, Quaternion& rot, VECTOR& targetPos);
	void SetBeforeDrawLatestFPS(int key, VECTOR& gPos, Quaternion& gRot, VECTOR& gTargetPos);
	void SetBeforeDrawLatestADS(int key, VECTOR& gPos, Quaternion& gRot, VECTOR& gTargetPos);
	void SetBeforeDrawLatestDeath(int key, VECTOR& gPos, Quaternion& gRot, VECTOR& gTargetPos);
	void SetBeforeDrawLatestFixedPoint(int key, VECTOR& gPos, Quaternion& gRot, VECTOR& gTargetPos);
	void SetBeforeDrawLatestDebug(int key, VECTOR& gPos, Quaternion& gRot, VECTOR& gTargetPos);

	// �⊮
	void GotoGoalLatest(int key, VECTOR& pos, Quaternion& rot, VECTOR& targetPos,
		VECTOR& gPos, Quaternion& gRot, VECTOR& gTargetPos);

	void UpdateShekeLatest(int key, VECTOR& pos);

	// �[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[


	const MultiCameraInfo& GetCameraInfo(int no) const;

private:

	NetManager& netManager_;
	MultiCameraInfo multiInfo_[MAX_PLAYER];

	int playerNum_;
	int key_;

	// �Ǐ]
	std::weak_ptr<ObjectBase> follow_;
	//std::weak_ptr<ObjectBase> follows_[MAX_PLAYER];

	// �J�������[�h
	MODE mode_;
	// MODE modes_[MAX_PLAYER];

	// �J����FOV
	float fov_;

	// �V�F�C�N
	float shakeTime_;
	float shakePow_;


	// �S�[���[�[�[�[�[�[�[�[�[�[�[�[�[�[

	// �J�����̈ʒu(�ڕW)
	VECTOR gPos_;

	// �J�����̒����_(�ڕW)
	VECTOR gTargetPos_;

	// �J�����̏����(�ڕW)
	VECTOR gCameraUp_;

	// �J�����̉�](�ڕW)
	Quaternion gRot_;

	// �J�����̊p�x(rad)(�ڕW)
	VECTOR gAngles_;

	// �J�����X�P�[��(�ڕW)
	float gFov_;

	// �⊮�p
	float lerpTime_;
	float cntLerpTime_;


	// �J�����ړ���](�ڕW�l��)
	void GotoGoal(void);
	void GotoGoalMulti(void);

	// �J�����������ʒu�ɖ߂�
	void SetDefault(void);
};

