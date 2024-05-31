#include "ColliderLine.h"


ColliderLine::ColliderLine(std::weak_ptr<ObjectBase> obj, Collider::OBJECT_TYPE type)
	: Collider(obj)
{
	// 衝突タイプ
	colType = COL_TYPE::LINE;

	// オブジェクトタイプ
	objType = type;
}

void ColliderLine::Update(VECTOR followPos, Quaternion rot)
{
	// 回転
	quaRot = rot;

	// 相対座標を加えて更新
	pos = VAdd(followPos, rot.PosAxis(relativePos));

	posA_ = prePos;
	posB_ = pos;
}

void ColliderLine::Update(VECTOR followPos, VECTOR rot)
{
	// 回転
	quaRot = rot;

	// 相対座標を加えて更新
	Quaternion q = Quaternion::Euler(rot);
	pos = VAdd(followPos, q.PosAxis(relativePos));

	posA_ = prePos;
	posB_ = pos;
}

void ColliderLine::Update(VECTOR followPosA, VECTOR followPosB, Quaternion rot)
{
	posA_ = followPosA;
	posB_ = followPosB;

	pos = { (posA_.x + posB_.x) / 2.0f,
			(posA_.y + posB_.y) / 2.0f,
			(posA_.z + posB_.z) / 2.0f };
}

void ColliderLine::Update(VECTOR followPosA, VECTOR followPosB, VECTOR rot)
{
	posA_ = followPosA;
	posB_ = followPosB;

	pos = { (posA_.x + posB_.x) / 2.0f,
			(posA_.y + posB_.y) / 2.0f,
			(posA_.z + posB_.z) / 2.0f };
}

VECTOR ColliderLine::GetPosA(void) const
{
	return posA_;
}

VECTOR ColliderLine::GetPosB(void) const
{
	return posB_;
}

void ColliderLine::DrawDebug(void)
{
	if (!isCollision)return;
	DrawLine3D(posA_, posB_, 0xd9a605);
}
