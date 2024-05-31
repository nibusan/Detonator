#include <EffekseerForDXLib.h>
#include "../../Application.h"
#include "../../Manager/InputManager.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/CollisionManager.h"
#include "../../Utility/Utility.h"
#include "../../Object/Actor/Player/Player.h"
#include "../../Object/Actor/Enemy/EnemyGeorge.h"
#include "../../Object/Common/ColliderLine.h"
#include "../../Object/Map/MapBase.h"
#include "GameCamera.h"

GameCamera::GameCamera() : netManager_(NetManager::GetInstance())
{
	mode_ = MODE::NONE;
	pos_ = { 0.0f, 0.0f, 0.0f };
	targetPos_ = { 0.0f, 0.0f, 0.0f };
	rot_ = Quaternion::Identity();
	angles_ = Utility::VECTOR_ZERO;
	lerpTime_ = 0.0f;
	cntLerpTime_ = 0.0f;
	fov_ = 1.0f;
	shakeTime_ = 0.0f;
	shakePow_ = 0.0f;

	playerNum_ = netManager_.GetPlayerNum();
	if (playerNum_ == 0)playerNum_ = 1;

	key_ = (int)netManager_.GetSelf().playerType;
}

GameCamera::~GameCamera(void)
{
}

void GameCamera::Init(void)
{

}

void GameCamera::Update(void)
{
	//NetUpdate();
}

void GameCamera::NetUpdate(void)
{
	// �}���`����
	//for (int i = 0; i < playerNum_; i++) {
	//	if (key_ == i) continue;
	//	auto& con = InputManager::GetInstance().GetController(i);
	//	auto& info = con.GetCameraInfo();
	//	multiInfo_[i].pos_ = info.pos;
	//	multiInfo_[i].targetPos_ = info.targetPos;
	//	multiInfo_[i].rot_ = Quaternion::Euler(info.rot);
	//}
}

void GameCamera::SetBeforeDraw(void)
{

	// �N���b�v������ݒ肷��(SetDrawScreen�Ń��Z�b�g�����)
	SetCameraNearFar(CAMERA_NEAR, CAMERA_FAR);

	switch (mode_)
	{
	case GameCamera::MODE::NONE:
		SetBeforeDrawDebug();
		break;
	case GameCamera::MODE::FPS:
		SetBeforeDrawFPS();
		break;
	case GameCamera::MODE::ADS:
		SetBeforeDrawADS();
		break;
	case GameCamera::MODE::DEATH:
		SetBeforeDrawDeath();
		break;
	case GameCamera::MODE::FIXED_POINT:
		SetBeforeDrawFixedPoint();
		break;
	case GameCamera::MODE::DEBUG:
		SetBeforeDrawDebug();
		break;
	}

	SetBeforeDrawMulti();

	// �⊮
 	GotoGoal();
	GotoGoalMulti();

	UpdateSheke();
	UpdateShekeMulti();

	//if (shakeTime_ != multiInfo_[0].shakeTime_ ||
	//	shakePow_ != multiInfo_[0].shakePow_)
	//{
	//	int i = 0;
	//}

	multiInfo_[key_].pos_ = pos_;
	multiInfo_[key_].targetPos_ = targetPos_;
	multiInfo_[key_].rot_ = rot_;


	//DrawSphere3D(pos_, 20.0f, 12, 0x000000, 0x000000, true);
	//DrawSphere3D(targetPos_, 20.0f, 12, 0xff0000, 0xff0000, true);
	//DrawLine3D(pos_, { 0.0f,100.0f,0.0f }, 0x00ff00);
	//DrawLine3D({ 100.0f,100.0f,0.0f }, { 0.0f,0.0f,0.0f }, 0xff0000);

	// �J�����̐ݒ�(�ʒu�ƒ����_�ɂ�鐧��)
	SetCameraPositionAndTargetAndUpVec(
		pos_,
		targetPos_,
		cameraUp_
	);



	//DrawSphere3D(pos_, 20.0f, 12, 0x000000, 0x000000, true);
	//DrawSphere3D(targetPos_, 20.0f, 12, 0xff0000, 0xff0000, true);


	// FOV
	SetupCamera_Perspective(fov_);

	Effekseer_Sync3DSetting();

	//DrawSphere3D(targetPos_, 16.0f, 10, 0xffffff, 0xffffff, true);
}

void GameCamera::SetBeforeDrawFPS(void)
{
	// ��l�̂̏���
	//auto& input = InputManager::GetInstance();
	auto player = std::dynamic_pointer_cast<Player>(follow_.lock());
	auto syncPos = player->GetSyncCameraPos();
	auto syncRot = player->GetSyncCameraRot();

	// �J������]����(�����_)
	gRot_ = Quaternion::Euler(syncRot);

	// �J�������W����
	// �J����FPS�p���΍��W
	VECTOR localCameraRotPos = gRot_.PosAxis(DEFAULT_CAMERA_POS);
	//VECTOR localCameraRotPos = rot_.PosAxis({ 0.0f,30.0f,-200.0f });
	gPos_ = VAdd(syncPos, localCameraRotPos);

	// �����_�̑��΍��W����]
	VECTOR localTargetRotPos = gRot_.PosAxis(RELATIVE_POS_FPS);

	// �����_�����[���h���W�ɕϊ�
	gTargetPos_ = VAdd(gPos_, localTargetRotPos);

	// �J�����̏����
	gCameraUp_ = gRot_.GetUp();

	//DrawSphere3D(gPos_, 20.0f, 12, 0x0000ff, 0x0000ff, false);
}

void GameCamera::SetBeforeDrawADS(void)
{
	// ��l�̂̏���

	//auto& input = InputManager::GetInstance();

	auto player = std::dynamic_pointer_cast<Player>(follow_.lock());
	auto syncPos = player->GetGunPos();
	auto syncRot = player->GetGunRot();

	// �J������]����(�����_)
	gRot_ = syncRot;

	// �J�������W����
	// �J����FPS�p���΍��W
	//VECTOR localCameraRotPos = gRot_.PosAxis(player->GetGunPos());
	////VECTOR localCameraRotPos = rot_.PosAxis({ 0.0f,30.0f,-200.0f });
	//gPos_ = VAdd(syncPos, localCameraRotPos);

	gPos_ = syncPos;

	// �����_�̑��΍��W����]
	VECTOR localTargetRotPos = gRot_.PosAxis(RELATIVE_POS_ADS);

	// �����_�����[���h���W�ɕϊ�
	gTargetPos_ = VAdd(gPos_, localTargetRotPos);
	//gTargetPos_ = CollisionManager::GetInstance().GetGunImpactPos();

	// �J�����̏����
	gCameraUp_ = gRot_.GetUp();
}

void GameCamera::SetBeforeDrawDeath(void)
{

	VECTOR syncPos;
	auto player = std::dynamic_pointer_cast<Player>(follow_.lock());
	if (player == nullptr) {
		auto enemy = std::dynamic_pointer_cast<EnemyGeorge>(follow_.lock());
		if (enemy == nullptr) {
			syncPos = follow_.lock()->GetTransform().pos;
		}
		else {
			syncPos = enemy->GetPosForCamera();
		}
	}
	else {
		syncPos = *player->GetCenterPos();
	}
	//auto syncRot = follow_.lock()->GetTransform().quaRot;

	// �J������]����(�����_)
	gRot_ = gRot_.Mult(
		Quaternion::Euler(0.0f, Utility::Deg2RadF(0.5f), 0.0f));

	// �J�������W����
	// �J����FPS�p���΍��W
	VECTOR localCameraRotPos = gRot_.PosAxis(RELATIVE_POS_DEATH);
	//VECTOR localCameraRotPos = rot_.PosAxis({ 0.0f,30.0f,-200.0f });
	gPos_ = VAdd(syncPos, localCameraRotPos);

	// �����_�̑��΍��W����]
	//VECTOR localTargetRotPos = gRot_.PosAxis(RELATIVE_POS_FPS);

	// �����_�����[���h���W�ɕϊ�
	//gTargetPos_ = VAdd(gPos_, localTargetRotPos);
	gTargetPos_ = syncPos;

	// �J�����̏����
	gCameraUp_ = gRot_.GetUp();

	//DrawSphere3D(gPos_, 20.0f, 12, 0x0000ff, 0x0000ff, false);
}

void GameCamera::SetBeforeDrawFixedPoint(void)
{
}

void GameCamera::SetBeforeDrawDebug(void)
{
 	//Mouse();

	auto& input = InputManager::GetInstance();

	// �ړ�
	float rad = 0.0f;
	float movePow = 5.0f;
	if (input.IsNew(KEY_INPUT_UP))
	{
		pos_ = VAdd(pos_, VScale(Quaternion::ToEuler(rot_.GetForward()), movePow));
	}
	if (input.IsNew(KEY_INPUT_LEFT))
	{
		pos_ = VAdd(pos_, VScale(Quaternion::ToEuler(rot_.GetLeft()), movePow));
	}
	if (input.IsNew(KEY_INPUT_RIGHT))
	{
		pos_ = VAdd(pos_, VScale(Quaternion::ToEuler(rot_.GetRight()), movePow));
	}
	if (input.IsNew(KEY_INPUT_DOWN))
	{
		pos_ = VAdd(pos_, VScale(Quaternion::ToEuler(rot_.GetBack()), movePow));
	}

	// �J���������
	if (input.IsNew(KEY_INPUT_Q)) { pos_.y += movePow; }
	// �J����������
	if (input.IsNew(KEY_INPUT_E)) { pos_.y -= movePow; }


	// �����_�̑��΍��W����]
	VECTOR localRotPos = rot_.PosAxis(RELATIVE_C2T_POS);

	// �����_�����[���h���W�ɕϊ�
	targetPos_ = VAdd(pos_, localRotPos);

	// �J�����̏����
	cameraUp_ = rot_.GetUp();
}

void GameCamera::SetBeforeDrawMulti(void)
{
	int i = 0;
	for (auto& info : multiInfo_) {
		switch (info.mode_)
		{
		case GameCamera::MODE::FPS:
			SetBeforeDrawFPSMulti(i);
			break;
		case GameCamera::MODE::ADS:
			SetBeforeDrawADSMulti(i);
			break;
		case GameCamera::MODE::DEATH:
			SetBeforeDrawDeathMulti(i);
			break;
		case GameCamera::MODE::FIXED_POINT:
			SetBeforeDrawFixedPointMulti(i);
			break;
		}
		i++;
	}
}

void GameCamera::SetBeforeDrawFPSMulti(int key)
{
	auto& info = multiInfo_[key];
	auto player = std::dynamic_pointer_cast<Player>(info.follow_.lock());
	auto syncPos = player->GetSyncCameraPos();
	auto syncRot = player->GetSyncCameraRot();

	// �J������]����(�����_)
	info.gRot_ = Quaternion::Euler(syncRot);

	// �J�������W����
	// �J����FPS�p���΍��W
	VECTOR localCameraRotPos = info.gRot_.PosAxis(DEFAULT_CAMERA_POS);
	//VECTOR localCameraRotPos = rot_.PosAxis({ 0.0f,30.0f,-200.0f });
	info.gPos_ = VAdd(syncPos, localCameraRotPos);

	// �����_�̑��΍��W����]
	VECTOR localTargetRotPos = info.gRot_.PosAxis(RELATIVE_POS_FPS);

	// �����_�����[���h���W�ɕϊ�
	info.gTargetPos_ = VAdd(info.gPos_, localTargetRotPos);

	// �J�����̏����
	//info.gCameraUp_ = info.gRot_.GetUp();
}

void GameCamera::SetBeforeDrawADSMulti(int key)
{
	auto& info = multiInfo_[key];
	auto player = std::dynamic_pointer_cast<Player>(info.follow_.lock());
	auto syncPos = player->GetGunPos();
	auto syncRot = player->GetGunRot();

	// �J������]����(�����_)
	info.gRot_ = syncRot;

	// �J�������W����
	// �J����FPS�p���΍��W
	//VECTOR localCameraRotPos = gRot_.PosAxis(player->GetGunPos());
	////VECTOR localCameraRotPos = rot_.PosAxis({ 0.0f,30.0f,-200.0f });
	//gPos_ = VAdd(syncPos, localCameraRotPos);

	info.gPos_ = syncPos;

	// �����_�̑��΍��W����]
	VECTOR localTargetRotPos = info.gRot_.PosAxis(RELATIVE_POS_ADS);

	// �����_�����[���h���W�ɕϊ�
	info.gTargetPos_ = VAdd(info.gPos_, localTargetRotPos);
	//gTargetPos_ = CollisionManager::GetInstance().GetGunImpactPos();

	// �J�����̏����
	//info.gCameraUp_ = info.gRot_.GetUp();
}

void GameCamera::SetBeforeDrawDeathMulti(int key)
{
	auto& info = multiInfo_[key];
	VECTOR syncPos;
	auto player = std::dynamic_pointer_cast<Player>(info.follow_.lock());
	if (player == nullptr) {
		auto enemy = std::dynamic_pointer_cast<EnemyGeorge>(info.follow_.lock());
		if (enemy == nullptr) {
			syncPos = info.follow_.lock()->GetTransform().pos;
		}
		else {
			syncPos = enemy->GetPosForCamera();
		}
	}
	else {
		syncPos = *player->GetCenterPos();
	}
	//auto syncRot = follow_.lock()->GetTransform().quaRot;

	// �J������]����(�����_)
	info.gRot_ = info.gRot_.Mult(
		Quaternion::Euler(0.0f, Utility::Deg2RadF(0.5f), 0.0f));

	// �J�������W����
	// �J����FPS�p���΍��W
	VECTOR localCameraRotPos = info.gRot_.PosAxis(RELATIVE_POS_DEATH);
	//VECTOR localCameraRotPos = rot_.PosAxis({ 0.0f,30.0f,-200.0f });
	info.gPos_ = VAdd(syncPos, localCameraRotPos);

	// �����_�̑��΍��W����]
	//VECTOR localTargetRotPos = gRot_.PosAxis(RELATIVE_POS_FPS);

	// �����_�����[���h���W�ɕϊ�
	//gTargetPos_ = VAdd(gPos_, localTargetRotPos);
	info.gTargetPos_ = syncPos;

	// �J�����̏����
	//info.gCameraUp_ = info.gRot_.GetUp();
}

void GameCamera::SetBeforeDrawFixedPointMulti(int key)
{
}

void GameCamera::UpdateSheke(void)
{
	if (shakeTime_ <= 0.0f) return;

	shakeTime_ -= SceneManager::GetInstance().GetDeltaTime();
	if (shakeTime_ <= 0.0f)
	{
		shakeTime_ = 0.0f;
		shakePow_ = 0.0f;
	}
	
	pos_.x += (float)((int)(shakeTime_ * 100.0f) % 2 - 2) * shakePow_;
}

void GameCamera::UpdateShekeMulti(void)
{
	for (auto& info : multiInfo_) {
		if (info.shakeTime_ <= 0.0f) continue;

		info.shakeTime_ -= SceneManager::GetInstance().GetDeltaTime();
		if (info.shakeTime_ <= 0.0f)
		{
			info.shakeTime_ = 0.0f;
			info.shakePow_ = 0.0f;
		}

		info.pos_.x += (float)((int)(info.shakeTime_ * 100.0f) % 2 - 2) * info.shakePow_;
	}
}

void GameCamera::Shake(float time, float pow, VECTOR pos)
{
	shakeTime_ = time;

	// �Ǐ]�ΏۂƐk���n�Ƃ̋���
	float dis = Utility::Distance(follow_.lock()->GetTransform().pos, pos);

	shakePow_ = pow * (SHAKE_DIS / dis);
}

void GameCamera::ShakeMulti(int key, float time, float pow, VECTOR pos)
{
	if (multiInfo_[key].follow_.lock() == nullptr) return;

	multiInfo_[key].shakeTime_ = time;

	// �Ǐ]�ΏۂƐk���n�Ƃ̋���
	float dis = Utility::Distance(multiInfo_[key].follow_.lock()->GetTransform().pos, pos);

	multiInfo_[key].shakePow_ = pow * (SHAKE_DIS / dis);
}

void GameCamera::Draw(void)
{
}

void GameCamera::Release(void)
{
}

VECTOR GameCamera::GetLatestPos(int key)
{

	VECTOR cPos = multiInfo_[key].pos_;
	Quaternion cRot = multiInfo_[key].rot_;
	VECTOR tPos = multiInfo_[key].targetPos_;

	SetBeforeDrawLatest(key, cPos, cRot, tPos);

	return cPos;
}

VECTOR GameCamera::GetLatestTargetPos(int key)
{
	VECTOR cPos = multiInfo_[key].pos_;
	Quaternion cRot = multiInfo_[key].rot_;
	VECTOR tPos = multiInfo_[key].targetPos_;

	SetBeforeDrawLatest(key, cPos, cRot, tPos);

	return tPos;

}

void GameCamera::SetBeforeDrawLatest(int key, VECTOR& pos, Quaternion& rot, VECTOR& targetPos)
{
	VECTOR gPos = pos;
	Quaternion gRot = rot;
	VECTOR gTargetPos = targetPos;

	switch (multiInfo_[key].mode_)
	{
	case GameCamera::MODE::NONE:
		SetBeforeDrawLatestDebug(key, gPos, gRot, gTargetPos);
		break;
	case GameCamera::MODE::FPS:
		SetBeforeDrawLatestFPS(key, gPos, gRot, gTargetPos);
		break;
	case GameCamera::MODE::ADS:
		SetBeforeDrawLatestADS(key, gPos, gRot, gTargetPos);
		break;
	case GameCamera::MODE::DEATH:
		SetBeforeDrawLatestDeath(key, gPos, gRot, gTargetPos);
		break;
	case GameCamera::MODE::FIXED_POINT:
		SetBeforeDrawLatestFixedPoint(key, gPos, gRot, gTargetPos);
		break;
	case GameCamera::MODE::DEBUG:
		SetBeforeDrawLatestDebug(key, gPos, gRot, gTargetPos);
		break;
	}

	// �⊮
	GotoGoalLatest(key, pos, rot, targetPos, gPos, gRot, gTargetPos);

	UpdateShekeLatest(key, pos);
}

void GameCamera::SetBeforeDrawLatestFPS(int key, VECTOR& pos, Quaternion& rot, VECTOR& targetPos)
{
	auto player = std::dynamic_pointer_cast<Player>(multiInfo_[key].follow_.lock());
	auto syncPos = player->GetSyncCameraPos();
	auto syncRot = player->GetSyncCameraRot();

	// �J������]����(�����_)
	rot = Quaternion::Euler(syncRot);

	// �J�������W����
	// �J����FPS�p���΍��W
	VECTOR localCameraRotPos = rot.PosAxis(DEFAULT_CAMERA_POS);
	//VECTOR localCameraRotPos = rot_.PosAxis({ 0.0f,30.0f,-200.0f });
	pos = VAdd(syncPos, localCameraRotPos);

	// �����_�̑��΍��W����]
	VECTOR localTargetRotPos = rot.PosAxis(RELATIVE_POS_FPS);

	// �����_�����[���h���W�ɕϊ�
	targetPos = VAdd(pos, localTargetRotPos);

	//// �J�����̏����
	//gCameraUp_ = rot.GetUp();
}

void GameCamera::SetBeforeDrawLatestADS(int key, VECTOR& pos, Quaternion& rot, VECTOR& targetPos)
{
	auto player = std::dynamic_pointer_cast<Player>(multiInfo_[key].follow_.lock());
	auto syncPos = player->GetGunPos();
	auto syncRot = player->GetSyncCameraRot();

	// �J������]����(�����_)
	rot = Quaternion::Euler(syncRot);

	// �J�������W����
	// �J����FPS�p���΍��W
	//VECTOR localCameraRotPos = gRot_.PosAxis(player->GetGunPos());
	////VECTOR localCameraRotPos = rot_.PosAxis({ 0.0f,30.0f,-200.0f });
	//gPos_ = VAdd(syncPos, localCameraRotPos);

	pos = syncPos;

	// �����_�̑��΍��W����]
	VECTOR localTargetRotPos = rot.PosAxis(RELATIVE_POS_ADS);

	// �����_�����[���h���W�ɕϊ�
	targetPos = VAdd(pos, localTargetRotPos);

	//// �J�����̏����
	//gCameraUp_ = rot.GetUp();
}

void GameCamera::SetBeforeDrawLatestDeath(int key, VECTOR& pos, Quaternion& rot, VECTOR& targetPos)
{
}

void GameCamera::SetBeforeDrawLatestFixedPoint(int key, VECTOR& pos, Quaternion& rot, VECTOR& targetPos)
{
}

void GameCamera::SetBeforeDrawLatestDebug(int key, VECTOR& pos, Quaternion& rot, VECTOR& targetPos)
{
}

void GameCamera::GotoGoalLatest(int key, VECTOR& pos, Quaternion& rot, VECTOR& targetPos, VECTOR& gPos, Quaternion& gRot, VECTOR& gTargetPos)
{
	// �⊮�p
	auto lTime = multiInfo_[key].lerpTime_;
	auto lCntTime = multiInfo_[key].cntLerpTime_;
	if (lTime != 0.0f)
	{
		lCntTime -= SceneManager::GetInstance().GetDeltaMult(1.0f);
		if (lCntTime <= 0.0f)lCntTime = 0.0f;
	}

	// ����
	float rate = 1.0f;
	if (lTime > 0.0f)
	{
		rate -= (lCntTime / lTime);
	}

	if (rate >= 1.0f)
	{
		// �J�����̈ʒu
		pos = gPos;

		// �J�����̒����_
		targetPos = gTargetPos;

		// �J�����̉�]
		rot = gRot;
	}
	else
	{
		// �J�����̈ʒu
		pos = Utility::Lerp(pos, gPos, rate);

		// �J�����̒����_
		targetPos = Utility::Lerp(targetPos, gTargetPos, rate);

		// �J�����̉�]
		rot = Quaternion::Slerp(rot, gRot, rate);
	}
}

void GameCamera::UpdateShekeLatest(int key, VECTOR& pos)
{
	if (multiInfo_[key].shakeTime_ <= 0.0f) return;

	auto time = multiInfo_[key].shakeTime_;
	auto pow = multiInfo_[key].shakePow_;

	time -= SceneManager::GetInstance().GetDeltaTime();
	if (time <= 0.0f)
	{
		time = 0.0f;
		pow = 0.0f;
	}

	pos.x += (float)((int)(time * 100.0f) % 2 - 2) * pow;
}

const GameCamera::MultiCameraInfo& GameCamera::GetCameraInfo(int no) const
{
	return multiInfo_[no];
}

VECTOR GameCamera::GetTargetPos(void) const
{
	//auto player = std::dynamic_pointer_cast<Player>(follow_);
	//auto syncPos = player->GetGunPos();
	//auto syncRot = player->GetSyncCameraRot();

	//// �J������]����(�����_)
	//auto gRot = Quaternion::Euler(syncRot);

	//// �J�������W����
	//// �J����FPS�p���΍��W
	////VECTOR localCameraRotPos = gRot_.PosAxis(player->GetGunPos());
	//////VECTOR localCameraRotPos = rot_.PosAxis({ 0.0f,30.0f,-200.0f });
	////gPos_ = VAdd(syncPos, localCameraRotPos);

	//auto gPos = syncPos;

	//// �����_�̑��΍��W����]
	//VECTOR localTargetRotPos = gRot.PosAxis(RELATIVE_POS_ADS);

	//// �����_�����[���h���W�ɕϊ�
	//auto gTargetPos = VAdd(gPos, localTargetRotPos);
	//return gTargetPos;
	return targetPos_;
}

const GameCamera::MODE& GameCamera::GetMode(void) const
{
	return mode_;
}

const GameCamera::MODE& GameCamera::GetMode(int key) const
{
	return multiInfo_[key].mode_;
}

VECTOR GameCamera::GetCameraDir(void) const
{
	return rot_.GetForward();
}

void GameCamera::ChangeMode(MODE mode, float time)
{
	if (mode_ == mode) return;
	if (mode_ == MODE::DEBUG) return;

	// �J�����̏����ݒ�
	SetDefault();

	// �J�������[�h�̕ύX
	mode_ = mode;

	// �ύX���̏���������
	switch (mode_)
	{
	case GameCamera::MODE::NONE:
		break;
	case GameCamera::MODE::FPS:
		break;
	case GameCamera::MODE::ADS:
		break;
	case GameCamera::MODE::DEATH:
		auto localRot = gRot_.ToEuler();
		localRot.x = 0.0f;
		localRot.z = 0.0f;
		gRot_ = Quaternion::Euler(localRot);
		gFov_ = 1.0f;
		break;
	case GameCamera::MODE::FIXED_POINT:
		break;
	case GameCamera::MODE::DEBUG:
		break;
	}

	// �⊮�p
	if (lerpTime_ != 0.0f)
	{
		float rate = cntLerpTime_ / lerpTime_;
		lerpTime_ = time;
		cntLerpTime_ = time * (1.0f - rate);
	}
	else
	{
		lerpTime_ = time;
		cntLerpTime_ = lerpTime_;
	}
}

void GameCamera::ChangeMode(MODE mode, std::weak_ptr<ObjectBase> follow, float time, float fav)
{
	if (mode_ == mode) return;
	if (mode_ == MODE::DEBUG) return;

	if (mode != MODE::FPS && mode != MODE::ADS &&
		mode != MODE::DEATH && mode != MODE::FIXED_POINT) return;

	// �J�����̏����ݒ�
	SetDefault();

	// �J�������[�h�̕ύX
	mode_ = mode;

	// �ύX���̏���������
	switch (mode_)
	{
	case GameCamera::MODE::FPS:
		// �Ǐ]�Ώ�
		follow_ = follow;
		gFov_ = FOV_DEFAULT;
		break;
	case GameCamera::MODE::ADS:
		// �Ǐ]�Ώ�
		follow_ = follow;
		gFov_ = 1.0f / fav;
		break;
	case GameCamera::MODE::DEATH:
		// �Ǐ]�Ώ�
		follow_ = follow;
		break;
	case GameCamera::MODE::FIXED_POINT:
		// �Ǐ]�Ώ�
		follow_ = follow;
		gPos_ = {300,550,-2200};
		//gPos_ = {-600,1550,-3200};
		gTargetPos_ = { -300,850,-500 };
		gCameraUp_ = { 0,1,0 };
		gRot_ = {};
		gAngles_ = {};
		gFov_ = 1.0f;
		break;
	}

	// �⊮�p
	if (lerpTime_ != 0.0f)
	{
		float rate = cntLerpTime_ / lerpTime_;
		lerpTime_ = time;
		cntLerpTime_ = time * (1.0f - rate);
	}
	else
	{
		lerpTime_ = time;
		cntLerpTime_ = lerpTime_;
	}
}

void GameCamera::ChangeMode(MODE mode, VECTOR fixedPos, VECTOR fixedRot,float time)
{
	if (mode_ == mode) return;
	if (mode_ == MODE::DEBUG) return;

	if (mode != MODE::FIXED_POINT) return;

	// �J�����̏����ݒ�
	SetDefault();

	// �J�������[�h�̕ύX
	mode_ = mode;

	switch (mode_)
	{
	case GameCamera::MODE::FIXED_POINT:
		gPos_ = fixedPos;
		gRot_ = Quaternion::Euler(fixedRot);
		break;
	}

	// �⊮�p
	if (lerpTime_ != 0.0f)
	{
		float rate = cntLerpTime_ / lerpTime_;
		lerpTime_ = time;
		cntLerpTime_ = time * (1.0f - rate);
	}
	else
	{
		lerpTime_ = time;
		cntLerpTime_ = lerpTime_;
	}
}

void GameCamera::ChangeMode(int key, MODE mode, float time)
{
	if (multiInfo_[key].mode_ == mode) return;

	auto& info = multiInfo_[key];

	// �J�������[�h�̕ύX
	info.mode_ = mode;

	// �ύX���̏���������
	switch (info.mode_)
	{
	case GameCamera::MODE::FPS:
		break;
	case GameCamera::MODE::ADS:
		break;
	case GameCamera::MODE::DEATH:
		break;
	case GameCamera::MODE::FIXED_POINT:
		info.gPos_ = { 300,550,-2200 };
		info.gTargetPos_ = { -300,850,-500 };
		info.gRot_ = {};
		break;
	}

	// �⊮�p
	if (info.lerpTime_ != 0.0f)
	{
		float rate = info.cntLerpTime_ / info.lerpTime_;
		info.lerpTime_ = time;
		info.cntLerpTime_ = time * (1.0f - rate);
	}
	else
	{
		info.lerpTime_ = time;
		info.cntLerpTime_ = info.lerpTime_;
	}
}

void GameCamera::ChangeMode(int key, MODE mode, std::weak_ptr<ObjectBase> follow, float time)
{
	if (multiInfo_[key].mode_ == mode) return;

	auto& info = multiInfo_[key];

	// �J�������[�h�̕ύX
	info.mode_ = mode;

	// �ύX���̏���������
	switch (info.mode_)
	{
	case GameCamera::MODE::FPS:
		break;
	case GameCamera::MODE::ADS:
		break;
	case GameCamera::MODE::DEATH:
		break;
	case GameCamera::MODE::FIXED_POINT:
		info.gPos_ = { 300,550,-2200 };
		info.gTargetPos_ = { -300,850,-500 };
		info.gRot_ = {};
		break;
	}

	// �⊮�p
	if (info.lerpTime_ != 0.0f)
	{
		float rate = info.cntLerpTime_ / info.lerpTime_;
		info.lerpTime_ = time;
		info.cntLerpTime_ = time * (1.0f - rate);
	}
	else
	{
		info.lerpTime_ = time;
		info.cntLerpTime_ = info.lerpTime_;
	}

	info.follow_ = follow;
}

void GameCamera::SetFollowTarget(std::weak_ptr<ObjectBase> follow)
{
	follow_ = follow;
}

void GameCamera::SetFollowTarget(int key, std::weak_ptr<ObjectBase> follow)
{
	multiInfo_[key].follow_ = follow;
}

void GameCamera::GotoGoal(void)
{
	if (mode_ == MODE::DEBUG) return;

	// �⊮�p
	if (lerpTime_ != 0.0f)
	{
		cntLerpTime_ -= SceneManager::GetInstance().GetDeltaMult(1.0f);
		if (cntLerpTime_ <= 0.0f)cntLerpTime_ = 0.0f;
	}

	// ����
	float rate = 1.0f;
	if (lerpTime_ > 0.0f)
	{
		rate -= (cntLerpTime_ / lerpTime_);
	}

	if (rate >= 1.0f)
	{
		// �J�����̈ʒu
		pos_ = gPos_;

		// �J�����̒����_
		targetPos_ = gTargetPos_;

		// �J�����̏����
		cameraUp_ = gCameraUp_;

		// �J�����̉�]
		rot_ = gRot_;

		// �J�����̊p�x(rad)
		angles_ = gAngles_;

		// �J�����X�P�[��
		fov_ = gFov_;
	}
	else
	{
		// �J�����̈ʒu
		pos_ = Utility::Lerp(pos_, gPos_, rate);

		// �J�����̒����_
		targetPos_ = Utility::Lerp(targetPos_, gTargetPos_, rate);

		// �J�����̏����
		cameraUp_ = Utility::Lerp(cameraUp_, gCameraUp_, rate);

		// �J�����̉�]
		rot_ = Quaternion::Slerp(rot_, gRot_, rate);

		// �J�����̊p�x(rad)
		angles_ = Utility::Lerp(angles_, gAngles_, rate);

		// �J����FOV
		fov_ = Utility::Lerp(fov_, gFov_, rate);
	}

	//// �R���C�_�[�X�V
	//if (follow_ == nullptr) return;
	//auto col = std::dynamic_pointer_cast<ColliderLine>(collider_);
	//switch (mode_)
	//{
	//case GameCamera::MODE::FPS:
	//	col->Update({0.0f,0.0f,0.0f}, pos_, transform_.rot);
	//	col->DrawDebug();
	//	
	//	break;
	//case GameCamera::MODE::ADS:
	//	//col->Update(follow_->GetTransform().pos, pos_, transform_.rot);
	//	break;
	//}
	
	//DrawSphere3D({0.0f,0.0f,0.0f}, 20.0f, 12, 0x00ff00, 0x00ff00, true);

}

void GameCamera::GotoGoalMulti(void)
{
	// �⊮�p
	for (auto& info : multiInfo_) {
		if (info.lerpTime_ != 0.0f)
		{
			info.cntLerpTime_ -= SceneManager::GetInstance().GetDeltaMult(1.0f);
			if (info.cntLerpTime_ <= 0.0f)info.cntLerpTime_ = 0.0f;
		}

		// ����
		float rate = 1.0f;
		if (info.lerpTime_ > 0.0f)
		{
			rate -= (info.cntLerpTime_ / info.lerpTime_);
		}

		if (rate >= 1.0f)
		{
			// �J�����̈ʒu
			info.pos_ = info.gPos_;

			// �J�����̒����_
			info.targetPos_ = info.gTargetPos_;

			//// �J�����̏����
			//info.cameraUp_ = info.gCameraUp_;

			// �J�����̉�]
			info.rot_ = info.gRot_;

			// �J�����̊p�x(rad)
			//info.angles_ = info.gAngles_;

			// �J�����X�P�[��
			//info.fov_ = info.gFov_;
		}
		else
		{
			// �J�����̈ʒu
			info.pos_ = Utility::Lerp(info.pos_, info.gPos_, rate);

			// �J�����̒����_
			info.targetPos_ = Utility::Lerp(info.targetPos_, info.gTargetPos_, rate);

			// �J�����̏����
			//info.cameraUp_ = Utility::Lerp(info.cameraUp_, info.gCameraUp_, rate);

			// �J�����̉�]
			info.rot_ = Quaternion::Slerp(info.rot_, info.gRot_, rate);

			// �J�����̊p�x(rad)
			//info.angles_ = Utility::Lerp(info.angles_, info.gAngles_, rate);

			// �J����FOV
			//info.fov_ = Utility::Lerp(info.fov_, info.gFov_, rate);
		}
	}
}

void GameCamera::SetDefault(void)
{
}
