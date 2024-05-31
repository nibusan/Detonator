#include "../../../Manager/CollisionManager.h"
#include "../../../Manager/AudioManager.h"
#include "../../../Manager/GameManager.h"
#include "../../../Manager/ResourceManager.h"
#include "../../Bullet/BulletBase.h"
#include "../../Grenade/GrenadeBase.h"
#include "../../Common/ColliderCapsule.h"
#include "../../../Utility/Utility.h"
#include "Armour.h"
#include "Shield.h"

Shield::Shield(std::weak_ptr<Armour>follow,VECTOR pos, VECTOR rot)
{
	follow_ = follow;
	model_ = std::make_unique<Model>(ResourceManager::GetInstance().Get("Model_Enemy_Shield04")->GetHandle());
	model_->SetIsAutoDeleteHandle(true);
	transform_.SetModel(model_->GetHandle());

	//transform_.SetModel(MV1LoadModel("Data/Model/Enemy/Shield04.mv1"));

	transform_.pos = pos;
	transform_.quaRot = Quaternion::Euler(rot);
	transform_.quaRotLocal = Quaternion::Euler({0.0f,Utility::Deg2RadF(180.0f),0.0f});

	transform_.scl = {2.0f,2.0f,2.0f};

	transform_.Update();

	hp_ = 200.0f;
	hpMax_ = hp_;
}

Shield::~Shield()
{
	transform_.Release();
}

void Shield::SetPos(VECTOR pos)
{
	transform_.pos = pos;
}

void Shield::SetRot(VECTOR rot)
{
	transform_.quaRot = Quaternion::Euler(rot);
}

void Shield::OnHit(std::weak_ptr<Collider> obj)
{
	float damage = 0.0f;

	if (obj.lock()->objType == Collider::OBJECT_TYPE::PLAYER_BULLET)
	{
		auto bullet = dynamic_pointer_cast<BulletBase>(obj.lock()->parent_.lock());

		damage = bullet->GetDamage();
	}
	else if (obj.lock()->objType == Collider::OBJECT_TYPE::EXPLOSION) {
		auto grenade = dynamic_pointer_cast<GrenadeBase>(obj.lock()->parent_.lock());

		damage = grenade->GetDamage() / 5.0f / follow_.lock()->GetAliveShieldNum();
	}

	Damage(damage);
	auto color = MV1GetDifColorScale(transform_.modelId);
	
	MV1SetDifColorScale(transform_.modelId, {1.0f + 1.0f - (hp_ / hpMax_),color.g,color.b,1.0f });

	AudioManager::GetInstance().Play(
		AudioList::AUDIO_TYPE::GRENADE_HIT,
		transform_.pos,
		2500.0f,
		false,
		0.6f
	);

	auto id = obj.lock()->parent_.lock()->GetID();
	if ((int)NetManager::GetInstance().GetSelf().playerType == (int)id) {
		GameManager::DamageData data;
		data.damage_ = damage;
		data.pos = collider_->pos;
		data.pos.x += (-10 + GetRand(20));
		data.pos.y += (-10 + GetRand(20));
		data.time_ = 1.0f;
		data.color_ = { 255,255,255 };
		GameManager::GetInstance().AddDamageData(data);
	}
}

void Shield::Init_Actor(void)
{
	collider_ = std::make_shared<ColliderCapsule>(GetPtr(),
		Collider::OBJECT_TYPE::SHIELD,
		30.0f, 60.0f);
	// 衝突管理クラスにセット
	CollisionManager::GetInstance().SetCollider(collider_);

	collider_->isCollision = true;
	transform_.isHide = false;

	UpdateCollider();
}

void Shield::Update_Actor(void)
{
	transform_.PreUpdate();
	collider_->PreUpdate();
	transform_.Update();
	UpdateCollider();
}

void Shield::Draw_Actor(void)
{

	transform_.Draw();
	//collider_->DrawDebug();
}

void Shield::Release_Actor(void)
{
	transform_.Release();
}

void Shield::UpdateCollider(void)
{
	VECTOR sendPos = transform_.pos;
	Quaternion sendRot = transform_.quaRot;
	sendPos = VAdd(sendPos, sendRot.PosAxis({ 0.0f,60.0f,0.0f }));
	collider_->Update(sendPos, sendRot);
}

void Shield::ProcessDead(void)
{
	collider_->isCollision = false;
	transform_.isHide = true;
}
