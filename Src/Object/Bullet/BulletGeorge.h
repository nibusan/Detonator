#pragma once
#include "BulletBase.h"

class BulletGeorge final : public BulletBase
{
public:

	BulletGeorge(int modelId, std::weak_ptr<ActorBase> parent);
	~BulletGeorge() = default;

	void Init_Object(void) override;

	// 各自独自の設定
	void SetParam(void) override;
};

