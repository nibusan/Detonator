#include "../ObjectBase.h"
#include "ActorBase.h"

ActorBase::ActorBase(void)
{

}

ActorBase::~ActorBase(void)
{

}

void ActorBase::Init_Object(void)
{
	Init_Actor();
}

void ActorBase::Update_Object(void)
{
	Update_Actor();
}

void ActorBase::Draw_Object(void)
{
	Draw_Actor();
}

void ActorBase::Release_Object(void)
{
	Release_Actor();

	// アニメーションハンドル
	//MV1DeleteModel();

}

void ActorBase::AnimEnd(int id)
{
}

void ActorBase::AnimNotice(int id)
{
}

void ActorBase::Damage(float damage)
{
	if (hp_ <= 0.0f) return;

	hp_ -= damage;
	if (hp_ <= 0.0f)
	{
		hp_ = 0.0f;
		ProcessDead();
	}
}

void ActorBase::ProcessDead(void)
{
}

const ID& ActorBase::GetLastAtkID(void) const
{
	return lastAtkID_;
}

const float& ActorBase::GetHP(void) const
{
	return hp_;
}

const float& ActorBase::GetHPMAX(void) const
{
	return hpMax_;
}
