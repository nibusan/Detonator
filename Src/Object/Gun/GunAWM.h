#pragma once
#include "GunBase.h"

class GunAWM : public GunBase
{
public:



	static constexpr VECTOR RELATIVE_POS = { 0.0f,-3.0f,5.0f };

	GunAWM(const Transform& follow, int frameNo, std::shared_ptr<ActorBase> parent);
	~GunAWM(void) = default;



	// �e����
	void CreateBullet(void) override;

	// �X�R�[�v�̈ʒu�擾
	const VECTOR GetScopePos()const override;

	void PlayReloadSound(void) override;

private:

	// �摜
	int imgH_;

	// �e���Ǝ��̐ݒ�
	void SetParam(void) override;
	void PlayBurstSound(void) override;

	void Draw_Object(void) override;
	void DrawTrans(void) override;
	void Release_Object(void) override;

};

