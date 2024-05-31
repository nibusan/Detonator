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
		IDLE		// 待機
	};

	PlayerMenu(ID id);
	~PlayerMenu() = default;

	void SetGunType(GUN_TYPE type);

	void SetID(ID id);
	//const GUN_TYPE& GetGunType(void) const;

	// アニメーション通知
	void AnimNotice(int id)override;

private:

	ID ID_;

	// アニメーターコントローラ
	std::unique_ptr<AnimatorController> animCon_;

	// 表示用銃
	std::shared_ptr<GunMenu> gun_;

	// 選択している銃
	//GUN_TYPE gunType_;

	void Init_Actor(void) override;
	void Update_Actor(void) override;
	void Draw_Actor(void) override;
	void Release_Actor(void) override;

	// アニメーション設定
	void AnimationSetting(void);
};

