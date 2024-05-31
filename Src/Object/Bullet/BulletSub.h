#pragma once
#include "BulletBase.h"

class BulletSub final : public BulletBase
{
public:

	BulletSub(int modelId, std::weak_ptr<ActorBase> parent);
	~BulletSub() = default;

	// äeé©ì∆é©ÇÃê›íË
	void SetParam(void) override;

};