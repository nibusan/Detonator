#include "../../Manager/SceneManager.h"
#include "../../Manager/AudioManager.h"
#include "../../Utility/Utility.h"
#include "../Bullet/BulletAR.h"
#include "GunAK47.h"

GunAK47::GunAK47(const Transform& follow, int frameNo, std::weak_ptr<ActorBase> parent) : GunBase(follow, frameNo, parent)
{

}

void GunAK47::CreateBullet(void)
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
	std::shared_ptr<BulletBase> tempBullet = std::make_shared<BulletAR>(bulletModelId_, parent_);

	tempBullet->Init();

	// �e����ݒ�
	SetBullet(tempBullet);

	// �z��Ɋi�[
	bullets_.emplace_back(tempBullet);

}

const VECTOR GunAK47::GetScopePos() const
{
	auto t = GetTransform();
	VECTOR ret = t.pos;
	//ret = VAdd(ret, t.quaRot.PosAxis({ 0.0f,18.1f,-25.0f }));
	ret = VAdd(ret, t.quaRot.PosAxis({ 0.0f,18.1f,-15.0f }));
	return ret;
}

void GunAK47::PlayReloadSound(void)
{
	AudioManager::GetInstance().Play(
		AudioList::AUDIO_TYPE::GUN_RELOAD_AK47,
		transform_.pos,
		500.0f,
		false,
		0.7f
	);
}

void GunAK47::SetParam(void)
{
	// ���f���t�@�C����
	fileName_ = "Model_Gun_AK47";

	// �X�R�[�v���f���t�@�C����
	scopeFileName_ = "Red+Dot+Sight.mv1";

	// �X�R�[�v�pTransform
	scpTransform_.pos = {0.0f,150.0f,0.0f};
	scpTransform_.quaRotLocal = Quaternion::Euler({0.0f,Utility::Deg2RadF(90.0f),0.0f});
	scpTransform_.scl = { 2.0f,2.0f,2.0f };
	scpRelativePos_ = {0.0f,12.0f,-3.0f};

	// �e�̃��f���t�@�C����
	bulletFileName_ = "ar.mv1";



	//// �Ǐ]�t���[���ԍ�(�E��)
	//followFrameNo_ = 36;

	// �o�����t���[���ԍ�(�e�̔��ˈʒu)
	barrelFrameNo_ = 20;

	// �ő�e��
	maxMagazine_ = 30;

	// ���ˊԊu
	shotDelay_ = 0.1f;

	// ���΍��W
	relativePos_ = RELATIVE_POS;

	// ���[�J����]
	VECTOR localRot = { Utility::Deg2RadF(0.0f),Utility::Deg2RadF(90.0f),Utility::Deg2RadF(0.0f) };
	transform_.quaRotLocal = Quaternion::Euler(localRot);

	// ���R�C���p�^�[���ݒ�
	recoil_[0] = Recoil::RECOIL_TYPE::UP;
	recoil_[1] = Recoil::RECOIL_TYPE::UP;
	recoil_[2] = Recoil::RECOIL_TYPE::RIGHT;
	recoil_[3] = Recoil::RECOIL_TYPE::RIGHT;
	recoil_[4] = Recoil::RECOIL_TYPE::LEFT;
	recoil_[5] = Recoil::RECOIL_TYPE::LEFT;

	// ���ˎ��̔����̑傫��
	recoilMag_ = 5.0f;

	randLength_ = 1000.0f;
	randRadius_ = 10.0f;

	// ��������
	hipRandLength_ = 700.0f;
	hipRandRadius_ = 30.0f;

	// �X�R�[�v�g��{��
	magnification_ = 2.0f;
}

void GunAK47::PlayBurstSound(void)
{
	AudioManager::GetInstance().Play(
		AudioList::AUDIO_TYPE::GUN_BURST_AK47,
		barrelPos_,
		5500.0f,
		false,
		0.95f
	);
}

//void GunAK47::SetBullet(std::shared_ptr<BulletBase> bullet)
//{
//	// ���ˈʒu�Ƀ��Z�b�g(�\��������)
//	
//	// ���˕������~��ɂ΂炯������
//	// �N�_
//	auto sP = barrelPos_;
//	// �I�_
//	auto eP = VAdd(sP, VScale(transform_.GetForward(), StoE_LENGTH));
//
//	// �p�x�����_��
//	float radian = Utility::Deg2RadF((float)GetRand(360));
//
//	// ���a�����_��
//	float radius = GetRand((int)RADIUS * 10) / 10.0f;
//
//	// �~��̕���
//	auto up = Quaternion::AngleAxis(radian, transform_.GetForward());
//	auto sDir = up.GetUp();
//
//	// �ڕW�_
//	auto oP = VAdd(eP, VScale(sDir, radius));
//
//	// �n�_����ڕW�_�ւ̃x�N�g��
//	auto vec = VNorm(VSub(oP, sP));
//	
//
//	// ���ˏ���
//	bullet->ReadyBullet(barrelPos_, vec);
//}
