#pragma once
#include "Collider.h"

class ColliderSphere : public Collider
{
public:

	// 球体
	ColliderSphere(std::weak_ptr<ObjectBase> obj, Collider::OBJECT_TYPE type, float rad);
	~ColliderSphere(void) = default;

	// デバッグ用描画
	void DrawDebug(void) override;

	void SetRadius(float rad);
	const float GetRadius(void) const;

private:

	// 半径
	float radius;
};

