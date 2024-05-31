#pragma once
#include"GrenadeBase.h"

class GrenadeSmoke final : public GrenadeBase
{
public:

	GrenadeSmoke(const Transform& follow, int frameNo, std::weak_ptr<ActorBase> parent);
	~GrenadeSmoke() = default;

	// �Փ˂����Ƃ��̏���
	void OnHit(MV1_COLL_RESULT_POLY info) override;

private:

	// �e���Ǝ��̐ݒ�
	void SetParam(void) override;

	// ����
	void Blast(void) override;

	// ���ꂼ��̏�����
	void Reset(void) override;
};

