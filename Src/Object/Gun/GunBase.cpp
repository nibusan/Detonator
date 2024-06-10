#include <math.h>
#include "../../Manager/SceneManager.h"
#include "../../Manager/CollisionManager.h"
#include "../../Manager/EffectManager.h"
#include "../../Manager/AudioManager.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/Camera/GameCamera.h"
#include "../../Utility/Utility.h"
#include "../Actor/ActorBase.h"
#include "../Actor/Player/Player.h"
#include "GunBase.h"

GunBase::GunBase(const Transform& follow, int frameNo, std::weak_ptr<ActorBase> parent) :
follow_(follow),
followFrameNo_(frameNo),
netManager_(NetManager::GetInstance())
{
	parent_ = parent;
	ID_ = parent_.lock()->GetID();

	auto player = std::dynamic_pointer_cast<Player>(parent_.lock());
	if (player == nullptr) return;
	parentKey_ = player->GetKey();

	isShot_ = false;

}

GunBase::~GunBase(void)
{

}

void GunBase::UpdateAfterCollision(void)
{
	// �e�X�V
	for (auto& b : bullets_)
	{
		b->UpdateAfterCollision();
	}
}

void GunBase::Init_Object(void)
{
	SetParam();

	model_ = std::make_unique<Model>(ResourceManager::GetInstance().GetResourceFile(fileName_.c_str())->GetHandle());
	model_->SetIsAutoDeleteHandle(true);
	transform_.SetModel(model_->GetHandle());


	// �e�̔��ˈʒu
	// �Ώۃt���[���̃��[���h�s��(�傫���A��]�A�ʒu)���擾����
	auto mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, barrelFrameNo_);
	auto pos = MGetTranslateElem(mat);	// �s�񂩂�ړ��l�����o��
	barrelPos_ = pos;

	// �Ǐ]
	Follow();

	//scopeModel_ = std::make_unique<Model>(ResourceManager::GetInstance().Get(scopeFileName_.c_str())->GetHandle());
	//scopeModel_->SetIsAutoDeleteHandle(true);
	//scpTransform_.SetModel(scopeModel_->GetHandle());
	//scpTransform_.Update();

	// �X�R�[�v���f�����ǂݍ���
	scopeModelId_ =
		MV1LoadModel(("Data/Model/Gun/Scope/" + scopeFileName_).c_str());
	scpTransform_.SetModel(scopeModelId_);
	scpTransform_.Update();

	// �e���f�����ǂݍ���
	//bulletModelId_ =
	//	MV1LoadModel(("Data/Model/Bullet/" + bulletFileName_).c_str());

	// �ړ���]�傫���������f���ɔ��f
	transform_.Update();


	// �e�̏��
	state_ = STATE::NONE;
	preState_ = state_;

	// �A�����Č�������
	burstCnt_ = 0;

	// �e��
	magazine_ = maxMagazine_;

	// ��]�����ǂ���
	isRotate_ = false;

	// ��]�J�n����qua
	startQuaRot_ = {};

	// ��]����
	rotateRate_ = 0.0f;
	// �J��
	changeCnt_ = TARGET_CHANGE_TIME;

	isShot_ = false;
}

void GunBase::Update_Object(void)
{

	// �Ǐ]
	Follow();

	// �ړ���]�傫���������f���ɔ��f
	transform_.Update();

	// �X�R�[�v�Ǐ]
	scpTransform_.quaRot = transform_.quaRot;
	scpTransform_.pos = VAdd(transform_.pos,
		scpTransform_.quaRot.PosAxis(scpRelativePos_));
	scpTransform_.Update();

	// �e�̔��ˈʒu
	// �Ώۃt���[���̃��[���h�s��(�傫���A��]�A�ʒu)���擾����
	if (barrelFrameNo_ != 0)
	{
		auto mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, barrelFrameNo_);
		auto pos = MGetTranslateElem(mat);	// �s�񂩂�ړ��l�����o��
		barrelPos_ = pos;
	}
	else
	{
		VECTOR lotPos = VAdd(transform_.pos, transform_.quaRot.PosAxis(barrelRelativePos_));
		barrelPos_ = lotPos;
	}

	if (isShot_) {
		ShotUpdate();
	}

	// ���ˊԊu����
	ShotDelay();

	//// �e�X�V
	//for (auto& b : bullets_)
	//{
	//	b->Update();
	//}



	// �G�t�F�N�g�X�V
	std::erase_if(effectID_,
		[](int id) {auto& effect = EffectManager::GetInstance();
	return !effect.GetData(id).lock()->IsPlaying(); });

	auto& effect = EffectManager::GetInstance();
	for (auto id : effectID_) {
		auto& data = effect.GetData(id);
		data.lock()->SetPos(barrelPos_);
		VECTOR localRot = Quaternion::ToEuler(
			transform_.quaRot.Mult(Quaternion::Euler({ 0.0f,90.0f,0.0f }))
		);
		data.lock()->SetAngle(localRot);
	}
	isShot_ = false;
}

void GunBase::Draw_Object(void)
{
	// ���f���`��
	transform_.Draw();

	// �e�`��
	for (auto& b : bullets_)
	{
		b->Draw();
	}

	MV1DrawFrame(scopeModelId_, 0);
	// �f�o�b�O
	//DrawSphere3D(targetPos_, 10.0f, 12, 0xffffff, 0xffffff, true);

	//DrawFormatString(1200, 60 + GetFontSize() * 1 + 150 * (int)ID_, 0xff00ff,
	//	"pos : %.2f,%.2f,%.2f", transform_.pos.x, transform_.pos.y, transform_.pos.z);
	//DrawFormatString(1200, 60 + GetFontSize() * 2 + 150 * (int)ID_, 0xff00ff,
	//	"rot : %.2f,%.2f,%.2f", transform_.rot.x, transform_.rot.y, transform_.rot.z);

	//DrawSphere3D(barrelPos_, 1.0f, 10, 0xffffff, 0xffffff, true);
	//DrawSphere3D(deg_, 3.0f, 10, 0xffffff, 0xffffff, true);

}

void GunBase::Release_Object(void)
{
	// ���f���폜(��)
	transform_.Release();

	// �e�폜(��)
	for (auto& b : bullets_)
	{
		b->Release();
	}
	bullets_.clear();
}

void GunBase::DrawTrans(void)
{
	// ���C�g�̐ݒ�
	SetUseLighting(false);
	MV1DrawFrame(scopeModelId_,1);
	SetUseLighting(true);
}

void GunBase::UpdateTransform(void)
{
	// �Ǐ]
	Follow();

	transform_.Update();

	// �X�R�[�v�Ǐ]
	scpTransform_.quaRot = transform_.quaRot;
	scpTransform_.pos = VAdd(transform_.pos,
		scpTransform_.quaRot.PosAxis(scpRelativePos_));
	scpTransform_.Update();

	// �e�̔��ˈʒu
	// �Ώۃt���[���̃��[���h�s��(�傫���A��]�A�ʒu)���擾����
	if (barrelFrameNo_ != 0)
	{
		auto mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, barrelFrameNo_);
		auto pos = MGetTranslateElem(mat);	// �s�񂩂�ړ��l�����o��
		barrelPos_ = pos;
	}
	else
	{
		VECTOR lotPos = VAdd(transform_.pos, transform_.quaRot.PosAxis(barrelRelativePos_));
		barrelPos_ = lotPos;
	}
}

void GunBase::Shot(void)
{
	isShot_ = true;
}

void GunBase::ShotUpdate(void)
{
	// ���ˊԊu
	if (shotDelayCnt_ > 0.0f || magazine_ <= 0.0f) return;

	PlayBurstSound();

	// �e����
	CreateBullet();

	// ���ˊԊu�Đݒ�
	shotDelayCnt_ = shotDelay_;

	// �e��
	magazine_--;

	// �A�����Č�������
	burstCnt_++;


	//EffectManager::EFFECT temp;
	//temp.pos = barrelPos_;
	//VECTOR localRot = Quaternion::ToEuler(
	//	transform_.quaRot.Mult(Quaternion::Euler({ 0.0f,90.0f,0.0f }))
	//);
	//temp.rot = localRot;
	//temp.scl = { 5.0f,5.0f,5.0f };
	//temp.speed = 1.0f;
	//temp.type = EffectManager::EFFECT_TYPE::BULLET;
	//effectID_.emplace_back(
	//	EffectManager::GetInstance().AddEffect(temp));

	EffectManager::EFFECT temp;
	temp.pos = barrelPos_;
	VECTOR localRot = Quaternion::ToEuler(
		transform_.quaRot.Mult(Quaternion::Euler({ 0.0f,90.0f,0.0f }))
	);
	temp.rot = localRot;
	temp.scl = { 5.0f,5.0f,5.0f };
	temp.speed = 1.0f;
	temp.type = EffectManager::EFFECT_TYPE::BULLET;
	effectID_.emplace_back(
		EffectManager::GetInstance().AddEffect(temp));
}

const Recoil::RECOIL_TYPE GunBase::GetRecoilType(void) const
{
	return recoil_[burstCnt_];
}

const int GunBase::GetMagazine(void) const
{
	return magazine_;
}

const int GunBase::GetMaxMagazine(void) const
{
	return maxMagazine_;
}

const float GunBase::GetRecoilMag(void) const
{
	return recoilMag_;
}

void GunBase::ResetBurstNum(void)
{
	burstCnt_ = 0.0f;
}

void GunBase::ReloadGun(void)
{
	magazine_ = maxMagazine_;
	burstCnt_ = 0.0f;
}

void GunBase::ChangeState(STATE state, float time)
{
	preState_ = state_;

	if (state_ == state) return;

	state_ = state;
	isRotate_ = true;

	// ��]����
	rotateRate_ = 0.0f;
	rotateTimeCnt_ = rotateTime_ = time;

	// ��]�J�n����qua
	startQuaRot_ = transform_.quaRot;

	changeCnt_ = 0.0f;
}


void GunBase::Follow(void)
{
	switch (state_)
	{
	case GunBase::STATE::NONE:
		FollowNone();
		break;
	case GunBase::STATE::SHOT:
		FollowShot();
		break;
	case GunBase::STATE::RELOAD:
		FollowReload();
		break;
	}
}

void GunBase::FollowNone(void)
{

	//// �Ώۃt���[���̃��[���h�s��(�傫���A��]�A�ʒu)���擾����
	//auto mat = MV1GetFrameLocalWorldMatrix(follow_.modelId, followFrameNo_);
	////auto scl = MGetSize(mat);			// �s�񂩂�傫�������o��
	//auto rot = MGetRotElem(mat);		// �s�񂩂��]�����o��
	//auto pos = MGetTranslateElem(mat);	// �s�񂩂�ړ��l�����o��


	// ��]����
	// �t���[��A�ƃt���[��B�̃x�N�g������p�x�����߁A
	// ���̉�]��������

	// �����A�e�̉�]�_�����炷�K�v������(���΍��W��OK)

	// �Ώۃt���[���̃��[���h�s��(�傫���A��]�A�ʒu)���擾����(��)
	auto mat = MV1GetFrameLocalWorldMatrix(follow_.modelId, 15);
	auto fraPosA = MGetTranslateElem(mat);	// �s�񂩂�ړ��l�����o��
	//Target(fraPosA);

	// �Ώۃt���[���̃��[���h�s��(�傫���A��]�A�ʒu)���擾����(��)
	mat = MV1GetFrameLocalWorldMatrix(follow_.modelId, followFrameNo_);
	auto fraPosB = MGetTranslateElem(mat);	// �s�񂩂�ړ��l�����o��

	// 2�_����p�x�����߂�(BtoA)
	auto diff = VSub(fraPosA, fraPosB);

	// ���ʊp
	float azimuth = atan2f(diff.x, diff.z);

	// �p
	float elevation = atanf(-diff.y / Utility::MagnitudeF({ diff.x,0.0f,diff.z }));

	// Z���͉�]�����Ȃ�
	VECTOR rot = { elevation ,azimuth , 0.0f };

	// ��]��ݒ�
	// ��]���Ȃ�
	if (isRotate_)Rotation(Quaternion::Euler(rot));
	else transform_.quaRot = Quaternion::Euler(rot);

	// �ʒu����
	transform_.pos = VAdd(fraPosB, transform_.quaRot.PosAxis(relativePos_));

	//transform_.quaRot = Quaternion::GetRotation(rot);
	//auto qua = Quaternion::GetRotation(rot);
	//
	//// ���ꂪ��̏����
	//auto a = qua.GetUp();
	//
	// �t���[���̏���������ɂ��ĉ�](��]��)
	//auto axis = Quaternion::AngleAxis(d * DX_PI_F / 180.0f, qua.GetUp());
	//auto axisF = Quaternion::AngleAxis(-90.0f * DX_PI_F / 180.0f, qua.GetForward());/*
	//auto axisR = Quaternion::AngleAxis(-90.0f * DX_PI_F / 180.0f, qua.GetRight());*/
	//
	// �t���[���̏���������ɂ��ĉ�](��]��)
	// Unity���ZXY�̏��ŉ�]������
	//Quaternion gunAxisY = Quaternion::AngleAxis(-90.0f * DX_PI_F / 180.0f, qua.GetUp());
	//Quaternion gunAxisX = Quaternion::AngleAxis(20.0f * DX_PI_F / 180.0f, qua.GetRight());
	//Quaternion gunAxisZ = Quaternion::AngleAxis(-45.0f * DX_PI_F / 180.0f, qua.GetForward());
	//
	// ��������]�ʍ쐬
	//Quaternion rotPow = Quaternion::Identity();
	//rotPow = Quaternion::Mult(gunAxisZ, Quaternion::Mult(gunAxisX, gunAxisY));
	//
	// ��]����
	//transform_.quaRot = rotPow.Mult(qua);
	//
	// ���W����
	//VECTOR localPos = Quaternion::PosAxis(transform_.quaRot, relativePos_);
	//transform_.pos = VAdd(pos, localPos);
	//transform_.pos = pos;
}

void GunBase::Rotation(Quaternion goalRot)
{

	rotateTimeCnt_ -= SceneManager::GetInstance().GetDeltaTime();
	if (rotateTimeCnt_ <= 0.0f)rotateTimeCnt_ = 0.0f;
	float rate = (rotateTime_ - rotateTimeCnt_) / rotateTime_;
	// ��]����
	//rotateRate_ += SceneManager::GetInstance().GetDeltaMult(5.0f);
	//if (rotateRate_ >= 1.0f)
	if (rate >= 1.0f)
	{
		//transform_.quaRot = goalRot;
		//rotateRate_ = 1.0f;
		//isRotate_ = false;
		transform_.quaRot = goalRot;
		isRotate_ = false;
	}
	else
	{
		// ���X�ɉ�]
		//transform_.quaRot = Quaternion::Slerp(startQuaRot_, goalRot, rotateRate_);
		transform_.quaRot = Quaternion::Slerp(startQuaRot_, goalRot, rate);
	}



	//// ��]�����ǂ���
	//if (fabsf(Quaternion::Angle(transform_.quaRot, goalRot)) < Utility::Deg2RadF(3.0f))
	//{
	//	transform_.quaRot = goalRot;
	//	isRotate_ = false;
	//}
	
}

//void GunBase::Target(VECTOR goalTargetPos)
//{
//	//changeCnt_ += SceneManager::GetInstance().GetDeltaTime();
//	//if (changeCnt_ >= TARGET_CHANGE_TIME)
//	//{
//	//	changeCnt_ = TARGET_CHANGE_TIME;
//	//}
//	//float rate = changeCnt_ / TARGET_CHANGE_TIME;
//	//targetPos_ = Utility::Lerp(targetPos_, goalTargetPos, rate);
//}


void GunBase::FollowShot(void)
{
	// ��]����
	// �����_�ƃt���[���̃x�N�g������p�x�����߁A
	// ���̉�]��������

	// �����A�e�̉�]�_�����炷�K�v������(���΍��W��OK)

	// �����_�̍��W
	//auto camera = std::dynamic_pointer_cast<GameCamera>(SceneManager::GetInstance().GetCamera());
	//VECTOR gTarPos = CollisionManager::GetInstance().GetGunImpactPos();
	auto player = std::dynamic_pointer_cast<Player>(parent_.lock());
	
	VECTOR gTarPos = player->GetLatestTargetPos();
	//gTarPos = camera->GetLatestTargetPos();
	//Target(gTarPos);
	

	// �Ώۃt���[���̃��[���h�s��(�傫���A��]�A�ʒu)���擾����
	auto mat = MV1GetFrameLocalWorldMatrix(follow_.modelId, followFrameNo_);
	auto fraPos = MGetTranslateElem(mat);	// �s�񂩂�ړ��l�����o��

	//if (Utility::Distance(targetPos_, gTarPos) >= 1.0f && preState_ == state_)
	//{
	//	//auto dif = VSub(gTarPos, targetPos_);
	//	//auto vec = VNorm(dif);
	//	//targetPos_ = VAdd(targetPos_,VScale(vec,5.0f));
	//	targetPos_ = Utility::Lerp(targetPos_, gTarPos, 0.05f);
	//}
	//else
	//{
		targetPos_ = gTarPos;
	//}

	// 2�_����p�x�����߂�
	auto diff = VSub(targetPos_, fraPos);

	// ���ʊp
	float azimuth = atan2f(diff.x, diff.z);

	// �p
	float elevation = atanf(-diff.y / Utility::MagnitudeF({ diff.x,0.0f,diff.z }));

	// Z���͉�]�����Ȃ�
	VECTOR rot = { elevation ,azimuth , 0.0f };

	// ��]��ݒ�
	// ��]���Ȃ�
	if (isRotate_)Rotation(Quaternion::Euler(rot));
	else transform_.quaRot = Quaternion::Euler(rot);

	// �ʒu����
	transform_.pos = VAdd(fraPos, transform_.quaRot.PosAxis(relativePos_));
}

void GunBase::FollowReload(void)
{
	// �Ώۃt���[���̃��[���h�s��(�傫���A��]�A�ʒu)���擾����
	auto mat = MV1GetFrameLocalWorldMatrix(follow_.modelId, followFrameNo_);
	//auto mat = MV1GetFrameLocalWorldMatrix(follow_.modelId, 38);
	auto fraPos = MGetTranslateElem(mat);	// �s�񂩂�ړ��l�����o��
	auto rot = MGetRotElem(mat);	// �s�񂩂��]�����o��

	//transform_.quaRot = Quaternion::GetRotation(rot);
	auto qua = Quaternion::GetRotation(rot);

	// �t���[���̏���������ɂ��ĉ�](��]��)
	// Unity���ZXY�̏��ŉ�]������
	Quaternion gunAxisY = Quaternion::AngleAxis(-90.0f * DX_PI_F / 180.0f, qua.GetUp());
	Quaternion gunAxisX = Quaternion::AngleAxis(20.0f * DX_PI_F / 180.0f, qua.GetRight());
	Quaternion gunAxisZ = Quaternion::AngleAxis(-45.0f * DX_PI_F / 180.0f, qua.GetForward());
	//
	// ��������]�ʍ쐬
	Quaternion rotPow = Quaternion::Identity();
	rotPow = Quaternion::Mult(gunAxisZ, Quaternion::Mult(gunAxisX, gunAxisY));

	//auto gTargetPos = rotPow.PosAxis({ 0.0f,0.0f,100.0f });
	//Target(gTargetPos);



	// ��]����
	if (isRotate_)
	{
		// �ڕW
		VECTOR goal = rotPow.Mult(qua).ToEuler();

		Rotation(Quaternion::Euler(goal));
	}
	else transform_.quaRot = rotPow.Mult(qua);
	

	// �ʒu����
	transform_.pos = VAdd(fraPos, transform_.quaRot.PosAxis(relativePos_));
}

void GunBase::ShotDelay(void)
{
	// ���ˊԊu����
	if (shotDelayCnt_ > 0.0f)
	{
		shotDelayCnt_ -= SceneManager::GetInstance().GetDeltaMult(1.0f);
		if (shotDelayCnt_ <= 0.0f)shotDelayCnt_ = 0.0f;
	}
}

bool GunBase::IsRotate(void) const
{
	return isRotate_;
}

bool GunBase::IsRemainMagazine(void) const
{
	return magazine_ > 0;
}

bool GunBase::IsShootable(void) const
{
	//return !(shotDelayCnt_ > 0.0f || magazine_ <= 0.0f);
	return (shotDelayCnt_ <= 0.0f && magazine_ > 0);
}

const float& GunBase::GetMagnification(void) const
{
	return magnification_;
}

const VECTOR& GunBase::GetBarrelPos(void) const
{
	return barrelPos_;
}

const float& GunBase::GetRandLength(void) const
{
	return randLength_;
}

const float& GunBase::GetRandRadius(void) const
{
	return randRadius_;
}

const float& GunBase::GetShotDelayTime(void) const
{
	return shotDelay_;
}

VECTOR GunBase::GetFoward(void) const
{
	return transform_.GetForward();
}

const VECTOR GunBase::GetScopePos(void) const
{
	return {};
}

std::vector<std::shared_ptr<BulletBase>>& GunBase::GetBullets(void)
{
	return bullets_;
}

void GunBase::SetBullet(std::shared_ptr<BulletBase> bullet)
{
	// ���ˈʒu�Ƀ��Z�b�g(�\��������)

	//// ���˕������~��ɂ΂炯������
	//// �N�_
	//auto sP = barrelPos_;
	//// �I�_
	//auto eP = VAdd(sP, VScale(transform_.GetForward(), randLength_));

	//// �p�x�����_��
	//float radian = Utility::Deg2RadF((float)GetRand(360));

	//// ���a�����_��
	//float radius = GetRand((int)randRadius_ * 10) / 10.0f;

	//// �~��̕���
	//auto up = Quaternion::AngleAxis(radian, transform_.GetForward());
	//auto sDir = up.GetUp();

	//// �ڕW�_
	//auto oP = VAdd(eP, VScale(sDir, radius));

	//// �n�_����ڕW�_�ւ̃x�N�g��
	//auto vec = VNorm(VSub(oP, sP));

	//// �e�X�g
	//vec = VNorm(VSub(eP, sP));

	//// ���ˏ���
	////bullet->ReadyBullet(barrelPos_, vec);

	//int frame = netManager_.GetFrameNo();

	//if (parentKey_ != -1 && 
	//	parentKey_ != netManager_.GetSelf().key) {
	//	vec = netManager_.GetBulletDir(parentKey_, vec);
	//}

	//if (parentKey_ != -1 &&
	//	parentKey_ == netManager_.GetSelf().key) {
	//	netManager_.SetBulletDir(vec);
	//}



	auto player = std::dynamic_pointer_cast<Player>(parent_.lock());
	if (player == nullptr) return;
	int seed = player->GetRandSeed();

	float randLength;
	float randRadius;
	if (player->IsADS()) {
		randLength = randLength_;
		randRadius = randRadius_;
	}
	else {
		randLength = hipRandLength_;
		randRadius = hipRandRadius_;
	}


	// �p�x�����_��
	//float radian = Utility::Deg2RadF((float)GetRand(360));
	float radian = Utility::Deg2RadF((float)(seed % 360));

	// ���a�����_��
	//float radius = GetRand((int)randRadius_ * 10) / 10.0f;

	float radius = 0.0f;
	if (randRadius != 0.0f) {
		radius = (seed % (int)(randRadius * 10.0f)) / 10.0f;
	}

	// ���˕������~��ɂ΂炯������
	// �N�_
	auto sP = barrelPos_;
	// �I�_
	auto eP = VAdd(sP, VScale(transform_.GetForward(), randLength));


	

	// �~��̕���
	auto up = Quaternion::AngleAxis(radian, transform_.GetForward());
	auto sDir = up.GetUp();

	// �ڕW�_
	auto oP = VAdd(eP, VScale(sDir, radius));

	// �n�_����ڕW�_�ւ̃x�N�g��
	auto vec = VNorm(VSub(oP, sP));

	// ���ˏ���
	bullet->ReadyBullet(barrelPos_, vec);
}
