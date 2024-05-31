#include "../../Manager/SceneManager.h"
#include "../../Manager/AudioManager.h"
#include "../../Utility/Utility.h"
#include "../Bullet/BulletSub.h"
#include "GunM24R.h"

GunM24R::GunM24R(const Transform& follow, int frameNo, std::weak_ptr<ActorBase> parent) : GunBase(follow, frameNo, parent)
{
}

void GunM24R::CreateBullet(void)
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
	std::shared_ptr<BulletBase> tempBullet = std::make_shared<BulletSub>(bulletModelId_, parent_);

	tempBullet->Init();

	// �e����ݒ�
	SetBullet(tempBullet);

	// �z��Ɋi�[
	bullets_.emplace_back(tempBullet);

}

const VECTOR GunM24R::GetScopePos() const
{
	auto t = GetTransform();
	VECTOR ret = t.pos;
	ret = VAdd(ret, t.quaRot.PosAxis({ 0.0f,11.0f,-3.0f }));
	//deg_ = ret;
	return ret;
}

void GunM24R::PlayReloadSound(void)
{
	AudioManager::GetInstance().Play(
		AudioList::AUDIO_TYPE::GUN_RELOAD_M24R,
		transform_.pos,
		500.0f,
		false,
		1.0f
	);
}

void GunM24R::SetParam(void)
{

	// ���f���t�@�C����
	fileName_ = "Model_Gun_M24R";

	// �X�R�[�v���f���t�@�C����
	scopeFileName_ = "Red+Dot+Sight.mv1";

	// �X�R�[�v�pTransform
	scpTransform_.pos = { 0.0f,150.0f,0.0f };
	scpTransform_.quaRotLocal = Quaternion::Euler({ 0.0f,Utility::Deg2RadF(90.0f),0.0f });
	scpTransform_.scl = { 2.0f,2.0f,2.0f };
	scpRelativePos_ = { 0.0f,5.0f,5.0f };

	// �e�̃��f���t�@�C����
	bulletFileName_ = "sub.mv1";

	//// �Ǐ]�t���[���ԍ�(�E��)
	//followFrameNo_ = 36;

	// �o�����t���[���ԍ�(�e�̔��ˈʒu)
	barrelFrameNo_ = 0;

	// �o�������΍��W
	barrelRelativePos_ = { 0.0f,5.0f,30.0f };

	// �ő�e��
	maxMagazine_ = 25;

	// ���ˊԊu
	shotDelay_ = 0.05f;

	// ���΍��W
	relativePos_ = RELATIVE_POS;

	// �傫��
	transform_.scl = { 8.0f,8.0f,8.0f };

	// ���[�J����]
	VECTOR localRot = { Utility::Deg2RadF(0.0f),Utility::Deg2RadF(180.0f),Utility::Deg2RadF(0.0f) };
	transform_.quaRotLocal = Quaternion::Euler(localRot);

	// ���R�C���p�^�[���ݒ�
	recoil_[0] = Recoil::RECOIL_TYPE::UP;
	recoil_[1] = Recoil::RECOIL_TYPE::UP;
	recoil_[2] = Recoil::RECOIL_TYPE::RIGHT;
	recoil_[3] = Recoil::RECOIL_TYPE::RIGHT;
	recoil_[4] = Recoil::RECOIL_TYPE::LEFT;
	recoil_[5] = Recoil::RECOIL_TYPE::LEFT;

	// ���ˎ��̔����̑傫��
	recoilMag_ = 8.0f;

	randLength_ = 400.0f;
	randRadius_ = 20.0f;

	// ��������
	hipRandLength_ = 200.0f;
	hipRandRadius_ = 30.0f;

	// �X�R�[�v�g��{��
	magnification_ = 1.5f;
}

void GunM24R::PlayBurstSound(void)
{
	AudioManager::GetInstance().Play(
		AudioList::AUDIO_TYPE::GUN_BURST_M24R,
		barrelPos_,
		5500.0f,
		false,
		0.8f
	);
}
