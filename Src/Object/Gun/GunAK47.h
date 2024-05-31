#pragma once
#include "GunBase.h"

class GunAK47 : public GunBase
{
public:

	static constexpr VECTOR RELATIVE_POS = { 0.0f,-2.0f,22.0f };

	//// ���˕������΂炯������ۂ̎n�_����I�_�܂ł̒���
	//static constexpr float StoE_LENGTH = (30.0f);

	//// ���˕������΂炯������ۂ̉~�̔��a
	//static constexpr float RADIUS = (10.0f);

	GunAK47(const Transform& follow, int frameNo, std::weak_ptr<ActorBase> parent);
	~GunAK47(void) = default;

	// �e����
	void CreateBullet(void) override;

	// �X�R�[�v�̈ʒu�擾
	const VECTOR GetScopePos()const override;

	void PlayReloadSound(void) override;

private:

	// �e���Ǝ��̐ݒ�
	void SetParam(void) override;
	void PlayBurstSound(void) override;

	//// �e�𔭎ˏ�Ԃɐݒ�
	//void SetBullet(std::shared_ptr<BulletBase> bullet)override;

};

