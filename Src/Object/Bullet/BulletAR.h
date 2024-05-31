#pragma once
#include "BulletBase.h"

class BulletAR final : public BulletBase
{
public:

	BulletAR(int modelId, std::weak_ptr<ActorBase> parent);
	~BulletAR() = default;

	// äeé©ì∆é©ÇÃê›íË
	void SetParam(void) override;

};

