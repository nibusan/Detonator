#pragma once
#include "GunBase.h"

class GunEnemy : public GunBase
{
public:

	static constexpr VECTOR RELATIVE_POS = { 0.0f,1.0f,18.0f };

	//// ���˕������΂炯������ۂ̎n�_����I�_�܂ł̒���
	//static constexpr float StoE_LENGTH = (30.0f);

	//// ���˕������΂炯������ۂ̉~�̔��a
	//static constexpr float RADIUS = (10.0f);

	GunEnemy(const Transform& follow, int frameNo, std::weak_ptr<ActorBase> parent);
	~GunEnemy(void) = default;

	// �e����
	void CreateBullet(void) override;

	void PlayReloadSound(void) override;

private:

	// �e���Ǝ��̐ݒ�
	void SetParam(void) override;
	void PlayBurstSound(void) override;

	// �ʏ펞�̒Ǐ](������̃t���[��������)
	void FollowNone(void) override;

	// ���C���̒Ǐ](�����_������)
	void FollowShot(void) override;

	// �����[�h���̒Ǐ]
	void FollowReload(void) override;

	// �e�𔭎ˏ�Ԃɐݒ�
	void SetBullet(std::shared_ptr<BulletBase> bullet) override;

};

