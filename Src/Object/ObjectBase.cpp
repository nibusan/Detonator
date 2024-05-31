#include "ObjectBase.h"

ObjectBase::ObjectBase(void)
{

}

ObjectBase::~ObjectBase(void)
{

}

void ObjectBase::Init(void)
{
    Init_Object();
}

void ObjectBase::Update(void)
{
    Update_Object();
}

void ObjectBase::Draw(void)
{
    Draw_Object();
}

void ObjectBase::Release(void)
{
    Release_Object();
    transform_.Release();
}

void ObjectBase::DrawTrans(void)
{
}

void ObjectBase::UpdateAfterCollision(void)
{
}

const Transform& ObjectBase::GetTransform(void) const
{
    return transform_;
}

void ObjectBase::OnHit()
{
}

void ObjectBase::OnHit(std::weak_ptr<Collider> obj)
{
    // デフォルトでは非表示にする
    transform_.isHide = false;
}

void ObjectBase::OnHit(std::weak_ptr<Collider> objA, std::weak_ptr<Collider> objB)
{
}

void ObjectBase::OnHit(int no)
{
}

void ObjectBase::OnHit(MV1_COLL_RESULT_POLY info)
{
}

void ObjectBase::OnHit(HITRESULT_LINE info)
{
}

void ObjectBase::OnHit(VECTOR normal)
{
}

void ObjectBase::OnHit(VECTOR normal, VECTOR hitPos)
{
}

const ID& ObjectBase::GetID(void) const
{
    return ID_;
}


