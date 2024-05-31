#include "../ObjectBase.h"
#include "ColliderCapsule.h"


ColliderCapsule::ColliderCapsule(std::weak_ptr<ObjectBase> obj, Collider::OBJECT_TYPE type, float radius, float length)
	: Collider(obj)
{
	// �Փ˃^�C�v
	colType = COL_TYPE::CAPSULE;
	
	// �I�u�W�F�N�g�^�C�v
	objType = type;

	// ���a
	radius_ = radius;

	// ����
	length_ = length;
}

void ColliderCapsule::Update(VECTOR followPos, Quaternion rot)
{

	// ��]
	quaRot = rot;

	// ���΍��W�������čX�V
	pos = VAdd(followPos, rot.PosAxis(relativePos));

	// �J�v�Z���p
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
	// ��]
	quaRot = rot;

	// ���΍��W�������čX�V
	Quaternion q = Quaternion::Euler(rot);
	pos = VAdd(followPos, q.PosAxis(relativePos));


	// �J�v�Z���p
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
	// �傫��
	preScl = scl;
	// ��]
	preQuaRot = quaRot;
	// �ʒu
	prePos = pos;

	// ���a
	preRadius_ = radius_;

	// ����
	preLength_ = length_;

	// �J�v�Z���p���W
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
