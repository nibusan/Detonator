#pragma once
#include "BulletBase.h"

class BulletGeorge final : public BulletBase
{
public:

	BulletGeorge(int modelId, std::weak_ptr<ActorBase> parent);
	~BulletGeorge() = default;

	void Init_Object(void) override;

	// �e���Ǝ��̐ݒ�
	void SetParam(void) override;
};

