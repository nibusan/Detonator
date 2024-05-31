#include "../../Manager/SceneManager.h"
#include "../../Manager/AudioManager.h"
#include "../../Utility/Utility.h"
#include "../Bullet/BulletAR.h"
#include "../Actor/Player/Player.h"
#include "GunM4.h"

GunM4::GunM4(const Transform& follow, int frameNo, std::weak_ptr<ActorBase> parent) : GunBase(follow, frameNo, parent)
{
}

void GunM4::CreateBullet(void)
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

	// �_���[�W�ݒ�
	for (auto& b : bullets_)
	{
		b->SetDamage(13.0f);
	}

	// �e����ݒ�
	SetBullet(tempBullet);

	// �z��Ɋi�[
	bullets_.emplace_back(tempBullet);

}

const VECTOR GunM4::GetScopePos() const
{
	auto t = GetTransform();
	VECTOR ret = t.pos;
	ret = VAdd(ret, t.quaRot.PosAxis({ 0.0f,13.5f,-23.5f }));
	//deg_ = ret;
	return ret;
}

void GunM4::PlayReloadSound(void)
{
	AudioManager::GetInstance().Play(
		AudioList::AUDIO_TYPE::GUN_RELOAD_M4,
		transform_.pos,
		500.0f,
		false,
		0.75f
	);
}

void GunM4::SetParam(void)
{
	// ���f���t�@�C����
	fileName_ = "Model_Gun_M4";

	// �X�R�[�v���f���t�@�C����
	scopeFileName_ = "Red+Dot+Sight.mv1";

	// �X�R�[�v�pTransform
	scpTransform_.pos = { 0.0f,150.0f,0.0f };
	scpTransform_.quaRotLocal = Quaternion::Euler({ 0.0f,Utility::Deg2RadF(90.0f),0.0f });
	scpTransform_.scl = { 2.0f,2.0f,2.0f };
	scpRelativePos_ = { 0.0f,7.5f,-12.0f };

	// �e�̃��f���t�@�C����
	bulletFileName_ = "ar.mv1";

	//// �Ǐ]�t���[���ԍ�(�E��)
	//followFrameNo_ = 36;

	// �o�����t���[���ԍ�(�e�̔��ˈʒu)
	barrelFrameNo_ = 0;

	// �o�������΍��W
	barrelRelativePos_ = { 0.0f,6.0f,52.5f };

	// �ő�e��
	maxMagazine_ = 28;

	// ���ˊԊu
	shotDelay_ = 0.1f;

	// �傫��
	transform_.scl = { 1.5f,1.5f,1.5f };

	// ���΍��W
	relativePos_ = RELATIVE_POS;

	// ���[�J����]
	VECTOR localRot = { Utility::Deg2RadF(0.0f),Utility::Deg2RadF(-90.0f),Utility::Deg2RadF(0.0f) };
	transform_.quaRotLocal = Quaternion::Euler(localRot);

	// ���R�C���p�^�[���ݒ�
	recoil_[0] = Recoil::RECOIL_TYPE::UP;
	recoil_[1] = Recoil::RECOIL_TYPE::UP;
	recoil_[2] = Recoil::RECOIL_TYPE::RIGHT;
	recoil_[3] = Recoil::RECOIL_TYPE::RIGHT;
	recoil_[4] = Recoil::RECOIL_TYPE::LEFT;
	recoil_[5] = Recoil::RECOIL_TYPE::LEFT;

	// ���ˎ��̔����̑傫��
	recoilMag_ = 3.5f;

	randLength_ = 1200.0f;
	randRadius_ = 8.0f;
	// ��������
	hipRandLength_ = 800.0f;
	hipRandRadius_ = 15.0f;

	// �X�R�[�v�g��{��
	magnification_ = 2.0f;	

}

void GunM4::PlayBurstSound(void)
{
	AudioManager::GetInstance().Play(
		AudioList::AUDIO_TYPE::GUN_BURST_M4,
		barrelPos_,
		5500.0f,
		false,
		0.8f
	);
}

void GunM4::Draw_Object(void)
{
	auto player = std::dynamic_pointer_cast<Player>(parent_.lock());
	if (player->IsADS() && !player->IsAct() && player->IsMe())
	{
	}
	else
	{
		// ���f���`��
		transform_.Draw();
	}

	// �e�`��
	for (auto& b : bullets_)
	{
		b->Draw();
	}

	MV1DrawFrame(scopeModelId_, 0);
}
