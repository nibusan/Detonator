#pragma once
#include "BulletBase.h"

class BulletSub final : public BulletBase
{
public:

	BulletSub(int modelId, std::weak_ptr<ActorBase> parent);
	~BulletSub() = default;

	// 各自独自の設定
	void SetParam(void) override;

};