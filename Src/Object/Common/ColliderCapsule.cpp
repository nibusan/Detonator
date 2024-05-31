#include "../ObjectBase.h"
#include "ColliderCapsule.h"


ColliderCapsule::ColliderCapsule(std::weak_ptr<ObjectBase> obj, Collider::OBJECT_TYPE type, float radius, float length)
	: Collider(obj)
{
	// 衝突タイプ
	colType = COL_TYPE::CAPSULE;
	
	// オブジェクトタイプ
	objType = type;

	// 半径
	radius_ = radius;

	// 長さ
	length_ = length;
}

void ColliderCapsule::Update(VECTOR followPos, Quaternion rot)
{

	// 回転
	quaRot = rot;

	// 相対座標を加えて更新
	pos = VAdd(followPos, rot.PosAxis(relativePos));

	// カプセル用
	float len = length_ / 2.0f;
	VECTOR tempPos1 = { 0.0f,len,0.0f };
	VECTOR tempPos2 = { 0.0f,-len,0.0f };
	VECTOR rotPos1 = quaRot.PosAxis(tempPos1);
	VECTOR rotPos2 = quaRot.PosAxis(tempPos2);
	capPosA_ = VAdd(pos, rotPos1);
	capPosB_ = VAdd(pos, rotPos2);
}

void ColliderCapsule::Update(VECTOR followPos, VECTOR rot)
{
	// 回転
	quaRot = rot;

	// 相対座標を加えて更新
	Quaternion q = Quaternion::Euler(rot);
	pos = VAdd(followPos, q.PosAxis(relativePos));


	// カプセル用
	float len = length_ / 2.0f;
	VECTOR tempPos1 = { 0.0f,len,0.0f };
	VECTOR tempPos2 = { 0.0f,-len,0.0f };
	VECTOR rotPos1 = quaRot.PosAxis(tempPos1);
	VECTOR rotPos2 = quaRot.PosAxis(tempPos2);
	capPosA_ = VAdd(pos, rotPos1);
	capPosB_ = VAdd(pos, rotPos2);
}

void ColliderCapsule::PreUpdate(void)
{
	// 大きさ
	preScl = scl;
	// 回転
	preQuaRot = quaRot;
	// 位置
	prePos = pos;

	// 半径
	preRadius_ = radius_;

	// 長さ
	preLength_ = length_;

	// カプセル用座標
	preCapPosA_ = capPosA_;
	preCapPosB_ = capPosB_;
}

void ColliderCapsule::DrawDebug(void)
{
	if (!isCollision)return;
	DrawCapsule3D(capPosA_, capPosB_, radius_, 12, 0xff00ff, 0xff00ff, false);
}

float ColliderCapsule::GetRadius(void) const
{
	return radius_;
}

VECTOR ColliderCapsule::GetPosA(void) const
{
	return capPosA_;
}

VECTOR ColliderCapsule::GetPosB(void) const
{
	return capPosB_;
}

float ColliderCapsule::GetPreRadius(void) const
{
	return preRadius_;
}

VECTOR ColliderCapsule::GetPrePosA(void) const
{
	return preCapPosA_;
}

VECTOR ColliderCapsule::GetPrePosB(void) const
{
	return preCapPosB_;
}
