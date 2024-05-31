#pragma once
#include "GunBase.h"

class GunM24R : public GunBase
{
public:



	static constexpr VECTOR RELATIVE_POS = { 0.0f,8.0f,13.0f };

	GunM24R(const Transform& follow, int frameNo, std::weak_ptr<ActorBase> parent);
	~GunM24R(void) = default;

	// �e����
	void CreateBullet(void) override;

	// �X�R�[�v�̈ʒu�擾
	const VECTOR GetScopePos()const override;

	void PlayReloadSound(void) override;

private:

	// �e���Ǝ��̐ݒ�
	void SetParam(void) override;
	void PlayBurstSound(void) override;


};

