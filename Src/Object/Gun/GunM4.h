#pragma once
#include "GunBase.h"

class GunM4 : public GunBase
{
public:

	static constexpr VECTOR RELATIVE_POS = { 0.0f,4.0f,25.0f };

	GunM4(const Transform& follow, int frameNo, std::weak_ptr<ActorBase> parent);
	~GunM4(void) = default;



	// �e����
	void CreateBullet(void) override;

	// �X�R�[�v�̈ʒu�擾
	const VECTOR GetScopePos()const override;

	void PlayReloadSound(void) override;

private:

	// �e���Ǝ��̐ݒ�
	void SetParam(void) override;
	void PlayBurstSound(void) override;

	void Draw_Object(void) override;
};

