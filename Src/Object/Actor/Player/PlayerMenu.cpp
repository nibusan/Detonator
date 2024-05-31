#include "../../../Manager/InputManager.h"
#include "../../../Manager/GameManager.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Utility/Utility.h"
#include "../../Gun/GunMenu.h"
#include "PlayerMenu.h"

PlayerMenu::PlayerMenu(ID id)
{
	ID_ = id;
}

void PlayerMenu::Init_Actor(void)
{
	//ID_ = ID::PLAYER_1;

	// 仮読み込み
	model_ = std::make_unique<Model>(ResourceManager::GetInstance().Get("Model_Player_Player1")->GetHandle());
	model_->SetIsAutoDeleteHandle(true);
	transform_.SetModel(model_->GetHandle());

	// ローカル回転
	VECTOR localRot = { Utility::Deg2RadF(0.0f),Utility::Deg2RadF(180.0f),Utility::Deg2RadF(0.0f) };
	transform_.quaRotLocal = Quaternion::Euler(localRot);

	// 初期座標
	transform_.pos = { -190.0f,-80.0f,270.0f };

	transform_.quaRot = Quaternion::Euler({ 0.0f,Utility::Deg2RadF(150.0f),0.0f });

	AnimationSetting();

	// アニメーション遷移
	animCon_->ChangeAnimation((int)ACTION::IDLE);
	//animCon_->SetValidAnimMove(true);

	// 選択している銃
	//gunType_ = GUN_TYPE::AK47;



	//for (auto& gun : guns_)
	//{
	//	gun->Init();
	//}

	// 表示用銃
	gun_ = std::make_shared<GunMenu>(ID_, &transform_, 35);

	Update_Actor();

}

void PlayerMenu::Update_Actor(void)
{
	transform_.PreUpdate();

	animCon_->Update();

	transform_.Update();

	gun_->ChangeType(GameManager::GetInstance().GetLoadOut((int)ID_).gunType_);
	gun_->Update();
}

void PlayerMenu::Draw_Actor(void)
{
	transform_.Draw();

	gun_->Draw();
}

void PlayerMenu::Release_Actor(void)
{
	transform_.Release();

	gun_->Release();

	animCon_->Release();
}

//void PlayerMenu::SetGunType(GUN_TYPE type)
//{
//	gunType_ = type;
//}
//
//const GUN_TYPE& PlayerMenu::GetGunType(void) const
//{
//	return gunType_;
//}



void PlayerMenu::SetGunType(GUN_TYPE type)
{
	gun_->ChangeType(type);
}

void PlayerMenu::SetID(ID id)
{
	ID_ = id;
}

void PlayerMenu::AnimNotice(int id)
{
	if (id == (int)ACTION::IDLE)
	{
		animCon_->SetIsStop((int)ACTION::IDLE, true);
	}
}

void PlayerMenu::AnimationSetting(void)
{
	// アニメーターコントローラ
	animCon_ = std::make_unique<AnimatorController>(std::dynamic_pointer_cast<ActorBase>(GetPtr()), transform_);

	// アニメーションによる座標移動無効化用座標
	VECTOR defPos = { 0.0f, 95.0f, 0.0f };

	// アニメーション
	int anim = -1;
	// 待機
	anim = MV1LoadModel("Data/Model/Player/Animation/Rifle Idle.mv1");
	animCon_->SetAnimData((int)ACTION::IDLE, anim, defPos);
}
