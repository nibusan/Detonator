#pragma once
#include "BulletBase.h"

class BulletSr final : public BulletBase
{
public:

	BulletSr(int modelId, std::weak_ptr<ActorBase> parent);
	~BulletSr() = default;

	// �e���Ǝ��̐ݒ�
	void SetParam(void) override;

};

