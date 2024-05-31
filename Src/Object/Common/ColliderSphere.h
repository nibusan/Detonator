#pragma once
#include "Collider.h"

class ColliderSphere : public Collider
{
public:

	// ����
	ColliderSphere(std::weak_ptr<ObjectBase> obj, Collider::OBJECT_TYPE type, float rad);
	~ColliderSphere(void) = default;

	// �f�o�b�O�p�`��
	void DrawDebug(void) override;

	void SetRadius(float rad);
	const float GetRadius(void) const;

private:

	// ���a
	float radius;
};

