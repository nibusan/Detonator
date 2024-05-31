#include "../../Object/Common/Transform.h"
#include "../ObjectBase.h"
#include "Collider.h"

Collider::Collider(std::weak_ptr<ObjectBase> obj) : parent_(obj)
{
	// 衝突判定を行うか
	isCollision = true;

	//// 衝突していたか
	//isHit = false;

	// 衝突タイプ
	colType = COL_TYPE::NONE;

	// 相対座標
	relativePos = {};

	// 座標
	pos = {};

	// 回転
	quaRot = {};

	// 大きさ
	scl = { 1.0f,1.0f,1.0f };

}

Collider::~Collider(void)
{
	
}



void Collider::Update(VECTOR followPos, Quaternion rot)
{

	// 回転
	quaRot = rot;

	// 相対座標を加えて更新
	pos = VAdd(followPos,rot.PosAxis(relativePos));
}

void Collider::Update(VECTOR followPos, VECTOR rot)
{
	// 回転
	quaRot = rot;

	// 相対座標を加えて更新
	Quaternion q = Quaternion::Euler(rot);
	pos = VAdd(followPos, q.PosAxis(relativePos));
}


void Collider::PreUpdate(void)
{
	// 大きさ
	preScl = scl;
	// 回転
	preQuaRot = quaRot;
	// 位置
	prePos = pos;
}

void Collider::ChangeObjectType(OBJECT_TYPE type)
{
	


	objType = type;
}

void Collider::ClearHitObject(void)
{
	//hitObjects.clear();
}

void Collider::DrawDebug(void)
{

}
