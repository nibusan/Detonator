#include "../../Manager/AudioManager.h"
#include "../Bullet/BulletGeorge.h"
#include "../../Utility/Utility.h"
#include "../Actor/Enemy/EnemyGeorge.h"
#include "GunEnemy.h"

GunEnemy::GunEnemy(const Transform& follow, int frameNo, std::weak_ptr<ActorBase> parent) : GunBase(follow, frameNo, parent)
{
}

void GunEnemy::CreateBullet(void)
{
	// �e�̔z����񂵁A�ė��p�\�Ȃ��̂�T��
	for (auto& b : bullets_)
	{
		auto bPtr = std::dynamic_pointer_cast<ObjectBase>(b);
		auto transform = bPtr->GetTransform();
		// �\����Ԃ̂��͍̂ė��p�ł��Ȃ�
		if (!transform.isHide) continue;

		// �e����ݒ�
		SetBullet(b);

		return;
	}

	// �ꎞ�I�Ȓe
	std::shared_ptr<BulletBase> tempBullet = std::make_shared<BulletGeorge>(bulletModelId_, parent_);

	tempBullet->Init();

	// �e����ݒ�
	SetBullet(tempBullet);

	// �z��Ɋi�[
	bullets_.emplace_back(tempBullet);
}

void GunEnemy::PlayReloadSound(void)
{
	AudioManager::GetInstance().Play(
		AudioList::AUDIO_TYPE::GUN_RELOAD_AK47,
		transform_.pos,
		500.0f,
		false,
		0.7f
	);
}

void GunEnemy::SetParam(void)
{
	// ���f���t�@�C����
	fileName_ = "Model_Gun_SS2";

	// �X�R�[�v���f���t�@�C����
	scopeFileName_ = "Red+Dot+Sight.mv1";

	// �o�����t���[���ԍ�(�e�̔��ˈʒu)
	barrelFrameNo_ = 6;

	// �ő�e��
	maxMagazine_ = 9999;

	// ���ˊԊu
	shotDelay_ = 0.1f;

	// ���΍��W
	relativePos_ = RELATIVE_POS;

	// ���[�J����]
	VECTOR localRot = { Utility::Deg2RadF(0.0f),Utility::Deg2RadF(180.0f),Utility::Deg2RadF(0.0f) };
	transform_.quaRotLocal = Quaternion::Euler(localRot);

	randLength_ = 1000.0f;
	randRadius_ = 10.0f;

	transform_.scl = { 1.5f,1.5f,1.5f };
}

void GunEnemy::PlayBurstSound(void)
{
	AudioManager::GetInstance().Play(
		AudioList::AUDIO_TYPE::GUN_BURST_AK47,
		barrelPos_,
		5500.0f,
		false,
		0.95f
	);
}

void GunEnemy::FollowNone(void)
{
	// �Ώۃt���[���̃��[���h�s��(�傫���A��]�A�ʒu)���擾����(��)
	auto mat = MV1GetFrameLocalWorldMatrix(follow_.modelId, 21);
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
}

void GunEnemy::FollowShot(void)
{
	auto enemy = std::dynamic_pointer_cast<EnemyGeorge>(parent_.lock());

	VECTOR gTarPos = enemy->GetFollowPos();

	// �Ώۃt���[���̃��[���h�s��(�傫���A��]�A�ʒu)���擾����
	auto mat = MV1GetFrameLocalWorldMatrix(follow_.modelId, followFrameNo_);
	auto fraPos = MGetTranslateElem(mat);	// �s�񂩂�ړ��l�����o��

	targetPos_ = gTarPos;

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

void GunEnemy::FollowReload(void)
{
	// �Ώۃt���[���̃��[���h�s��(�傫���A��]�A�ʒu)���擾����
	auto mat = MV1GetFrameLocalWorldMatrix(follow_.modelId, followFrameNo_);
	auto fraPos = MGetTranslateElem(mat);	// �s�񂩂�ړ��l�����o��
	auto rot = MGetRotElem(mat);	// �s�񂩂��]�����o��

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

void GunEnemy::SetBullet(std::shared_ptr<BulletBase> bullet)
{

	// �p�x�����_��
	float radian = Utility::Deg2RadF((float)GetRand(360));

	// ���a�����_��
	float radius = GetRand((int)randRadius_ * 10) / 10.0f;


	// ���˕������~��ɂ΂炯������
	// �N�_
	auto sP = barrelPos_;
	// �I�_
	auto eP = VAdd(sP, VScale(transform_.GetForward(), randLength_));




	// �~��̕���
	auto up = Quaternion::AngleAxis(radian, transform_.GetForward());
	auto sDir = up.GetUp();

	// �ڕW�_
	auto oP = VAdd(eP, VScale(sDir, radius));

	// �n�_����ڕW�_�ւ̃x�N�g��
	auto vec = VNorm(VSub(oP, sP));

	// ���ˏ���
	bullet->ReadyBullet(barrelPos_, vec);

	magazine_ = maxMagazine_;
}
