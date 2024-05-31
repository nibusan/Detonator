#pragma once
#include"../../Common/Count.h"
#include"GrenadeBase.h"

class GrenadeHand final : public GrenadeBase
{
public:

	// �����܂ł̎���
	static constexpr float BLAST_TIME = (3.0f);

	GrenadeHand(const Transform& follow, int frameNo, std::weak_ptr<ActorBase> parent);
	~GrenadeHand() = default;

	// �Փ˂����Ƃ��̏���
	void OnHit(MV1_COLL_RESULT_POLY info) override;
	void OnHit(VECTOR normal, VECTOR hitPos) override;



private:

	// �����J�E���g
	Count blast_;

	// �e���Ǝ��̐ݒ�
	void SetParam(void) override;

	// ����
	void Blast(void) override;

	// �ړ�
	void UpdateNone(void)override;

	void UpdateBlast(void)override;

	// ���ꂼ��̏�����
	void Reset(void) override;
};

