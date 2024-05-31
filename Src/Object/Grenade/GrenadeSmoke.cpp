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
	// 爆発させる
	Blast();
}

void GrenadeSmoke::SetParam(void)
{
	// モデルファイル名
	fileName_ = "Model_Grenade_SmokeGrenade";

	// 大きさ
	transform_.scl = { 0.2f,0.2f,0.2f };

	// タイプ
	type_ = GRENADE_TYPE::SMOKE;

	// 相対座標
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
	// 追従
	ChangeState(STATE::BLAST);

	// 元のオブジェクトタイプ配列から要素を消しておく
	CollisionManager::GetInstance().DeleteCollider(collider_);

	// オブジェクトタイプを変えて再びセット
	collider_->ChangeObjectType(Collider::OBJECT_TYPE::SMOKE);
	CollisionManager::GetInstance().SetCollider(collider_);


}

void GrenadeSmoke::Reset(void)
{
}
