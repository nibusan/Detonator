#pragma once
#include "../../Gun/GunType.h"
#include "../../ObjectBase.h"
#include "../ActorBase.h"
#include "../AnimatorController.h"

//class AnimatorController;
class GunMenu;


class PlayerMenu : public ActorBase
{
public:

	enum class ACTION
	{
		IDLE		// �ҋ@
	};

	PlayerMenu(ID id);
	~PlayerMenu() = default;

	void SetGunType(GUN_TYPE type);

	void SetID(ID id);
	//const GUN_TYPE& GetGunType(void) const;

	// �A�j���[�V�����ʒm
	void AnimNotice(int id)override;

private:

	ID ID_;

	// �A�j���[�^�[�R���g���[��
	std::unique_ptr<AnimatorController> animCon_;

	// �\���p�e
	std::shared_ptr<GunMenu> gun_;

	// �I�����Ă���e
	//GUN_TYPE gunType_;

	void Init_Actor(void) override;
	void Update_Actor(void) override;
	void Draw_Actor(void) override;
	void Release_Actor(void) override;

	// �A�j���[�V�����ݒ�
	void AnimationSetting(void);
};

