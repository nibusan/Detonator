#include "ColliderSphere.h"

ColliderSphere::ColliderSphere(std::weak_ptr<ObjectBase> obj, Collider::OBJECT_TYPE type, float rad)
	: Collider(obj)
{
	// �Փ˃^�C�v
	colType = COL_TYPE::SPHERE;

	// �I�u�W�F�N�g�^�C�v
	objType = type;

	// ���a
	radius = rad;
}

void ColliderSphere::DrawDebug(void)
{
	if (!isCollision) return;
	DrawSphere3D(pos, radius, 12, 0xffffff, 0xffffff, false);
}

void ColliderSphere::SetRadius(float rad)
{
	radius = rad;
}

const float ColliderSphere::GetRadius(void) const
{
	return radius;
}
