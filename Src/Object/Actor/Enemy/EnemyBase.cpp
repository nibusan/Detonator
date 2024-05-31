#include "EnemyBase.h"

EnemyBase::EnemyBase(void)
{

}

EnemyBase::~EnemyBase(void)
{

}

void EnemyBase::Init_Actor(void)
{
	Init_Enemy();
}

void EnemyBase::Update_Actor(void)
{
	Update_Enemy();
}

void EnemyBase::Draw_Actor(void)
{
	Draw_Enemy();
}

void EnemyBase::Release_Actor(void)
{
	Release_Enemy();
}

void EnemyBase::Update_EnemyAfterCollision(void)
{
}


