#pragma once
#include"GrenadeBase.h"

class GrenadeSticky final : public GrenadeBase
{
public:


	GrenadeSticky(const Transform& follow, int frameNo, std::weak_ptr<ActorBase> parent);
	~GrenadeSticky() = default;

	// �Փ˂����Ƃ��̏���
	//void OnHit(MV1_COLL_RESULT_POLY info) override;
	void OnHit(VECTOR normal, VECTOR hitPos) override;


private:
	int soundHandle_;

	// �e���Ǝ��̐ݒ�
	void SetParam(void) override;

	// ����
	void Blast(void) override;

	// �ړ�
	void UpdateNone(void)override;

	void UpdateBlast(void)override;

	void Release_Object(void) override;

	// ���ꂼ��̏�����
	void Reset(void) override;
};

