#pragma once
#include "BulletBase.h"

class BulletAR final : public BulletBase
{
public:

	BulletAR(int modelId, std::weak_ptr<ActorBase> parent);
	~BulletAR() = default;

	// �e���Ǝ��̐ݒ�
	void SetParam(void) override;

};

