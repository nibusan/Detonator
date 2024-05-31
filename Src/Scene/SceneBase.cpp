#include "SceneBase.h"

SceneBase::SceneBase(void)
{
}

SceneBase::~SceneBase(void)
{
}

void SceneBase::Init(void)
{
}

void SceneBase::Update(void)
{
}

void SceneBase::Draw(void)
{
}

void SceneBase::Release(void)
{
}

Shared_Graphic SceneBase::GetDrawScreen(void) const
{
	return sceneScreen_;
}
