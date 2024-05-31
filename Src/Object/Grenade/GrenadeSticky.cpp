#include "../../Utility/Utility.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/EffectManager.h"
#include "../../Manager/CollisionManager.h"
#include "../../Manager/AudioManager.h"
#include "../../Manager/Camera/GameCamera.h"
#include "../Common/ColliderSphere.h"
#include "../Common/ColliderLine.h"
#include "../Actor/Player/Player.h"
#include "GrenadeSticky.h"

GrenadeSticky::GrenadeSticky(const Transform& follow, int frameNo, std::weak_ptr<ActorBase> parent) : GrenadeBase(follow, frameNo, parent)
{
}

void GrenadeSticky::OnHit(VECTOR normal, VECTOR hitPos)
{
	transform_.pos = VAdd(hitPos, VScale(normal, 10.0f));
	transform_.quaRot =
		Quaternion::Mult(Quaternion::LookRotation(normal),
			Quaternion::Euler(Utility::Deg2RadF(90.0f), 0.0f, 0.0f));


	pow_ = 0.0f;

	soundHandle_ = AudioManager::GetInstance().Play(
		AudioList::AUDIO_TYPE::GRENADE_STICKY,
		transform_.pos,
		1000.0f,
		true,
		0.6f
	);
}

void GrenadeSticky::SetParam(void)
{
	// ���f���t�@�C����
	fileName_ = "Model_Grenade_StickyGrenade";

	// �傫��
	transform_.scl = { 1.5f,1.5f,1.5f };

	// �^�C�v
	type_ = GRENADE_TYPE::STICKY;

	// ���΍��W
	relativePos_ = { 0.0f,0.0f,0.0f };

	transform_.localPos = { 0.0f,-10.0f,0.0f };

	damage_ = 500.0f;

	blastEffect_.Init(SceneManager::GetInstance().GetDeltaTime(), SceneManager::GetInstance().GetDeltaTime());
	blastEffect_.Set();

	soundHandle_ = 0;
}

void GrenadeSticky::Blast(void)
{
	if (state_ != STATE::NONE) return;



	EffectManager::EFFECT temp;
	temp.pos = transform_.pos;
	temp.rot = transform_.quaRot.Mult(Quaternion::Euler({})).ToEuler();
	temp.scl = { 60.0f,60.0f,60.0f };
	temp.speed = 2.0f;
	temp.type = EffectManager::EFFECT_TYPE::EXPLOSION;
	effectID_ = EffectManager::GetInstance().AddEffect(temp);

	transform_.pos = VAdd(transform_.pos, VScale(transform_.quaRot.GetUp(), 150.0f));

	AudioManager::GetInstance().Stop(soundHandle_);
	AudioManager::GetInstance().Play(
		AudioList::AUDIO_TYPE::GRENADE_EXPLOSION,
		transform_.pos,
		5500.0f,
		false,
		0.8f
	);

	transform_.isHide = true;
	// �Ǐ]
	ChangeState(STATE::BLAST);

	// ���̃I�u�W�F�N�g�^�C�v�z�񂩂�v�f�������Ă���
	CollisionManager::GetInstance().DeleteCollider(collider_);

	// �I�u�W�F�N�g�^�C�v��ς��čĂуZ�b�g
	collider_->ChangeObjectType(Collider::OBJECT_TYPE::EXPLOSION);
	CollisionManager::GetInstance().SetCollider(collider_);

	// �J�����V�F�C�N
	auto camera = std::dynamic_pointer_cast<GameCamera>(SceneManager::GetInstance().GetCamera());
	auto player = std::dynamic_pointer_cast<Player>(parent_.lock());
	if (netManager_.GetSelf().key == player->GetKey()) {
		camera->Shake(1.0f, 5.0f, transform_.pos);
	}
	for (int i = 0; i < 4; i++) {
		camera->ShakeMulti(i, 1.0f, 5.0f, transform_.pos);
	}
}

void GrenadeSticky::UpdateNone(void)
{
	if (pow_ == 0.0f) {
		//if (CheckHitKey(KEY_INPUT_B)) {
		//	Blast();
		//}

		return;
	}
	// �ړ�
	VECTOR movePow = VScale(moveDir_, pow_);
	movePow.y = 0.0f;
	transform_.pos = VAdd(transform_.pos, movePow);

	// ��]
	// pow_��1�Ȃ�0.5�x��]������
	Quaternion rot = Quaternion::AngleAxis(Utility::Deg2RadF(pow_),
		Quaternion::LookRotation(moveDir_).GetUp());
	//auto rot = Quaternion::Euler({ Utility::Deg2RadF(pow_),0.0f,0.0f });
	transform_.quaRot = transform_.quaRot.Mult(rot);

	// �d��
	transform_.Gravity();


}

void GrenadeSticky::UpdateBlast(void)
{
	auto col = std::dynamic_pointer_cast<ColliderSphere>(collider_);
	auto r = col->GetRadius();
	r += 250.0f;
	if (r >= 250.0f) r = 250.0f;
	col->SetRadius(r);

	//auto e = EffectManager::GetInstance().GetData(effectID_);
	//if (!e.lock()->IsPlaying()) {
	//	ChangeState(STATE::END);
	//}

	if (blastEffect_.Update()) {
		ChangeState(STATE::END);
	}
}

void GrenadeSticky::Release_Object(void)
{
	transform_.Release();
	if (soundHandle_ != 0) {
		AudioManager::GetInstance().Stop(soundHandle_);
	}
}



void GrenadeSticky::Reset(void)
{
	blastEffect_.Set();
}
