#pragma once
#include "Collider.h"

class ColliderLine : public Collider
{
public:

	// モデル
	ColliderLine(std::weak_ptr<ObjectBase> obj, Collider::OBJECT_TYPE type);
	~ColliderLine(void) = default;

	// 位置更新等
	void Update(VECTOR followPos, Quaternion rot) override;
	void Update(VECTOR followPos, VECTOR rot) override;
	void Update(VECTOR followPosA, VECTOR followPosB, Quaternion rot);
	void Update(VECTOR followPosA, VECTOR followPosB, VECTOR rot);

	// 点A取得
	VECTOR GetPosA(void) const;

	// 点B取得
	VECTOR GetPosB(void) const;

	// デバッグ用描画
	void DrawDebug(void) override;

private:

	VECTOR posA_;
	VECTOR posB_;
};

