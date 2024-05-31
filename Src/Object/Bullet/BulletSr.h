#pragma once
#include "BulletBase.h"

class BulletSr final : public BulletBase
{
public:

	BulletSr(int modelId, std::weak_ptr<ActorBase> parent);
	~BulletSr() = default;

	// äeé©ì∆é©ÇÃê›íË
	void SetParam(void) override;

};

