#include "../../Manager/SceneManager.h"
#include "../../Manager/AudioManager.h"
#include "../../Utility/Utility.h"
#include "../Actor/Player/Player.h"
#include "../Bullet/BulletSr.h"
#include "GunAWM.h"

GunAWM::GunAWM(const Transform& follow, int frameNo, std::shared_ptr<ActorBase> parent) : GunBase(follow, frameNo, parent)
{
}

void GunAWM::Draw_Object(void)
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

void GunAWM::CreateBullet(void)
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
	std::shared_ptr<BulletBase> tempBullet = std::make_shared<BulletSr>(bulletModelId_, parent_);

	tempBullet->Init();

	// �e����ݒ�
	SetBullet(tempBullet);

	// �z��Ɋi�[
	bullets_.emplace_back(tempBullet);

}

const VECTOR GunAWM::GetScopePos() const
{
	auto t = GetTransform();
	VECTOR ret = t.pos;
	ret = VAdd(ret, t.quaRot.PosAxis({ 0.0f,17.2f,12.3f }));
	return ret;
}

void GunAWM::PlayReloadSound(void)
{
	AudioManager::GetInstance().Play(
		AudioList::AUDIO_TYPE::GUN_RELOAD_AWM,
		transform_.pos,
		500.0f,
		false,
		1.0f
	);
}

void GunAWM::SetParam(void)
{

	// ���f���t�@�C����
	fileName_ = "Model_Gun_AWM";

	// �X�R�[�v���f���t�@�C����
	scopeFileName_ = "AWMScope.mv1";

	// �摜�ǂݍ���
	imgH_ = LoadGraph("Data/Image/SR_Scope.png");

	//// �X�R�[�v�pTransform
	//scpTransform_.pos = { 0.0f,150.0f,0.0f };
	//scpTransform_.quaRotLocal = Quaternion::Euler({ 0.0f,Utility::Deg2RadF(180.0f),0.0f });
	//scpTransform_.scl = { 2.0f,2.0f,2.0f };
	//scpRelativePos_ = { 0.0f,17.2f,15.0f };

	// �e�̃��f���t�@�C����
	bulletFileName_ = "sr.mv1";

	//// �Ǐ]�t���[���ԍ�(�E��)
	//followFrameNo_ = 36;

	// �o�����t���[���ԍ�(�e�̔��ˈʒu)
	barrelFrameNo_ = 0;

	// �o�������΍��W
	barrelRelativePos_ = { 0.0f,15.0f,90.0f };

	// �ő�e��
	maxMagazine_ = 5;

	// ���ˊԊu
	shotDelay_ = 2.0f;

	// ���΍��W
	relativePos_ = RELATIVE_POS;

	// �傫��
	transform_.scl = { 1.0f,1.0f,1.0f };

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
	recoilMag_ = 10.0f;

	randLength_ = 30.0f;
	randRadius_ = 0.0f;

	// ��������
	hipRandLength_ = 150.0f;
	hipRandRadius_ = 30.0f;

	// �X�R�[�v�g��{��
	magnification_ = 4.0f;
}

void GunAWM::PlayBurstSound(void)
{
	AudioManager::GetInstance().Play(
		AudioList::AUDIO_TYPE::GUN_BURST_AWM,
		barrelPos_,
		5500.0f,
		false,
		1.0f
	);
}

void GunAWM::DrawTrans(void)
{
	// ���C�g�̐ݒ�
	//SetUseLighting(false);
	//MV1DrawFrame(scopeModelId_, 1);
	//SetUseLighting(true);

	auto player = std::dynamic_pointer_cast<Player>(parent_.lock());
	if (player->IsADS() && !player->IsAct() && player->IsMe() && player->GetHP() > 0.0f)
	{
		DrawGraph(0, 0, imgH_, true);
	}

}

void GunAWM::Release_Object(void)
{
	// ���f���폜(��)
	transform_.Release();

	// �e�폜(��)
	for (auto& b : bullets_)
	{
		b->Release();
	}
	bullets_.clear();
	DeleteGraph(imgH_);
}
