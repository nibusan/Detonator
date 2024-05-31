#include "../../Manager/CollisionManager.h"
#include "../../Manager/EffectManager.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/AudioManager.h"
#include "GrenadeSmoke.h"

GrenadeSmoke::GrenadeSmoke(const Transform& follow, int frameNo, std::weak_ptr<ActorBase> parent) : GrenadeBase(follow, frameNo, parent)
{
}

void GrenadeSmoke::OnHit(MV1_COLL_RESULT_POLY info)
{
	// ����������
	Blast();
}

void GrenadeSmoke::SetParam(void)
{
	// ���f���t�@�C����
	fileName_ = "Model_Grenade_SmokeGrenade";

	// �傫��
	transform_.scl = { 0.2f,0.2f,0.2f };

	// �^�C�v
	type_ = GRENADE_TYPE::SMOKE;

	// ���΍��W
	relativePos_ = { 0.0f,0.0f,0.0f };

	damage_ = 0.0f;
}

void GrenadeSmoke::Blast(void)
{
	EffectManager::EFFECT temp;
	temp.pos = transform_.pos;
	temp.rot = transform_.quaRot.ToEuler();
	temp.scl = { 100.0f,100.0f,100.0f };
	temp.speed = 1.0f;
	temp.type = EffectManager::EFFECT_TYPE::SMOKE;
	effectID_ = EffectManager::GetInstance().AddEffect(temp);

	AudioManager::GetInstance().Play(
		AudioList::AUDIO_TYPE::GRENADE_SMOKE,
		transform_.pos,
		2500.0f,
		false,
		1.0f
	);

	transform_.isHide = true;
	// �Ǐ]
	ChangeState(STATE::BLAST);

	// ���̃I�u�W�F�N�g�^�C�v�z�񂩂�v�f�������Ă���
	CollisionManager::GetInstance().DeleteCollider(collider_);

	// �I�u�W�F�N�g�^�C�v��ς��čĂуZ�b�g
	collider_->ChangeObjectType(Collider::OBJECT_TYPE::SMOKE);
	CollisionManager::GetInstance().SetCollider(collider_);


}

void GrenadeSmoke::Reset(void)
{
}
