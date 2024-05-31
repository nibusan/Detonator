#pragma once
#include "BulletBase.h"

class BulletEnemy final : public BulletBase
{

public:

	BulletEnemy(int modelId, std::weak_ptr<ActorBase> parent);
	~BulletEnemy() = default;

	void Init_Object(void) override;

	// �e���Ǝ��̐ݒ�
	void SetParam(void) override;

};

