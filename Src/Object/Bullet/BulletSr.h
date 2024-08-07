#pragma once
#include "BulletBase.h"

class BulletSr final : public BulletBase
{
public:

	BulletSr(int modelId, std::weak_ptr<ActorBase> parent);
	~BulletSr() = default;

	// 各自独自の設定
	void SetParam(void) override;

};

