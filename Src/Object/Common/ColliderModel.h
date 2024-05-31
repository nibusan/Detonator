#pragma once
#include "Collider.h"

class ColliderModel : public Collider
{
public:

	// モデル
	ColliderModel(std::weak_ptr<ObjectBase> obj, Collider::OBJECT_TYPE type, int model);
	~ColliderModel(void) = default;

	// 位置更新等
	void Update(VECTOR followPos, Quaternion rot) override;
	void Update(VECTOR followPos, VECTOR rot) override;
	void Update(void);

	// モデルID取得
	int GetModelId(void) const;

	// 指定のフレームを衝突判定から消す(大きさを0にする)
	void HideFrame(int frameNo);

	// 指定のフレームの衝突判定を復活(大きさを1にする)
	void AppearFrame(int frameNo);

	VECTOR GetScaleFrame(int frameNo) const;

private:

	int modelId_;
};

