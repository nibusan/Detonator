#include <DxLib.h>
#include "../Application.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Utility/Utility.h"
#include "../Object/Actor/Player/PlayerMenu.h"
#include "../Object/Gun/GunMenu.h"
#include "../Object/Grenade/GrenadeMenu.h"
#include "../UI/UILayer/UILayer_sLobby.h"
#include "../UI/UIScreen/UIScreen.h"
#include "SingleLobbyScene.h"

SingleLobbyScene::SingleLobbyScene(void) : gameManager_(GameManager::GetInstance())
{
}

SingleLobbyScene::~SingleLobbyScene(void)
{
	int a = 1;
	a = 0;
}

void SingleLobbyScene::Init(void)
{
	UISetting();

	std::unique_ptr<UILayerBase> uiLayer = std::make_unique<UILayer_sLobby>();
	uiLayer->Init();
	auto uiScreen = SceneManager::GetInstance().GetUIScreen();
	uiScreen.lock()->PushUILayer(std::move(uiLayer));
}

void SingleLobbyScene::Update(void)
{
	player_->Update();
	gun_->Update();
	grenades_[0]->Update();
	grenades_[1]->Update();
}

void SingleLobbyScene::Draw(void)
{
	DrawUI();

	grenades_[0]->Draw();
	grenades_[1]->Draw();
}

void SingleLobbyScene::Release(void)
{
	DeleteGraph(back_);
	player_->Release();
	gun_->Release();
	grenades_[0]->Release();
	grenades_[1]->Release();
	box_.scl = { 1.0f,1.0f,1.0f };
	box_.Update();
	box_.Release();

	auto uiScreen = SceneManager::GetInstance().GetUIScreen();
	uiScreen.lock()->PopUILayer();
}

void SingleLobbyScene::UISetting(void)
{
	back_ = LoadGraph("Data/Image/Menu_Background.png");

	// 表示用プレイヤー
	player_ = std::make_shared<PlayerMenu>(ID::PLAYER_1);
	player_->Init();

	VECTOR pos = { 35.0f,0.0f,120.0f };
	gun_ = std::make_shared<GunMenu>(ID::PLAYER_1, pos);

	pos = { -3.0f,-70.0f,200.0f };
	grenades_[0] = std::make_shared<GrenadeMenu>(ID::PLAYER_1, 0, pos);
	pos = { 125.0f,-70.0f,200.0f };
	grenades_[1] = std::make_shared<GrenadeMenu>(ID::PLAYER_1, 1, pos);

	box_.SetModel(MV1LoadModel("Data/Model/Box/box.mv1"));
	box_.pos = { 100.0f,-100.0f,500.0f };
	box_.quaRot = Quaternion::Euler({ Utility::Deg2RadF(22.0f),Utility::Deg2RadF(110.0f),Utility::Deg2RadF(-50.0f) });
	box_.scl = { 2.8f,2.8f,2.8f };
	box_.Update();

	std::weak_ptr<Font> s = std::dynamic_pointer_cast<Font>(ResourceManager::GetInstance().
		GetResourceFile("Font_GenShinGothic_Bold"));

	fontGunName_ = std::make_unique<Font>(s.lock()->GetFontName(), 50);
	fontGunName_->SetIsAutoDeleteHandle(true);
	fontInfo_ = std::make_unique<Font>(s.lock()->GetFontName(), 30);
	fontInfo_->SetIsAutoDeleteHandle(true);
	fontMode_ = std::make_unique<Font>(s.lock()->GetFontName(), 40);
	fontMode_->SetIsAutoDeleteHandle(true);
}

void SingleLobbyScene::DrawUI(void)
{
	// UIとして扱いたくないモノの描画

	DrawGraph(0, 0, back_, true);

	box_.Draw();

	auto info = gameManager_.GetLoadOut(0);
	switch (info.gunType_)
	{
	case GUN_TYPE::AK47:
		DrawStringToHandle(750, 200, "AK-47", 0xffffff, fontGunName_->GetHandle());
		DrawStringToHandle(750, 260,
			"1949年にソビエト連邦軍が制式採用した自動小銃", 0xffffff, fontInfo_->GetHandle());
		DrawStringToHandle(750, 300,
			"火力は高いが反動が大きい", 0xffffff, fontInfo_->GetHandle());
		break;
	case GUN_TYPE::AWM:
		DrawStringToHandle(750, 200, "AWM", 0xffffff, fontGunName_->GetHandle());
		DrawStringToHandle(750, 260,
			"イギリス軍で制式採用されているボルトアクション方式の狙撃銃", 0xffffff, fontInfo_->GetHandle());
		DrawStringToHandle(750, 300,
			"長距離に強く、近距離に弱い", 0xffffff, fontInfo_->GetHandle());
		break;
	case GUN_TYPE::M4:
		DrawStringToHandle(750, 200, "M4", 0xffffff, fontGunName_->GetHandle());
		DrawStringToHandle(750, 260,
			"現在アメリカ軍兵士の大半が装備しているアサルトライフル", 0xffffff, fontInfo_->GetHandle());
		DrawStringToHandle(750, 300,
			"反動は小さいが火力が少し落ちる", 0xffffff, fontInfo_->GetHandle());
		break;
	case GUN_TYPE::M24R:
		DrawStringToHandle(750, 200, "M24R", 0xffffff, fontGunName_->GetHandle());
		DrawStringToHandle(750, 260,
			"連射性能が高く近距離戦闘が強いサブマシンガン", 0xffffff, fontInfo_->GetHandle());
		DrawStringToHandle(750, 300,
			"M24というスナイパーライフルとは別物", 0xffffff, fontInfo_->GetHandle());
		break;
	}

	DrawStringToHandle(750, 710,
		"スロット1", 0xffffff, fontInfo_->GetHandle());
	switch (info.greType_[0])
	{
	case GRENADE_TYPE::HAND:
		DrawStringToHandle(900, 910,
			"手榴弾", 0xffffff, fontInfo_->GetHandle());
		break;
	case GRENADE_TYPE::SMOKE:
		DrawStringToHandle(870, 910,
			"発煙手榴弾", 0xffffff, fontInfo_->GetHandle());
		break;
	case GRENADE_TYPE::STICKY:
		DrawStringToHandle(885, 910,
			"粘着榴弾", 0xffffff, fontInfo_->GetHandle());
		break;
	}



	DrawStringToHandle(1350, 710,
		"スロット2", 0xffffff, fontInfo_->GetHandle());
	switch (info.greType_[1])
	{
	case GRENADE_TYPE::HAND:
		DrawStringToHandle(1500, 910,
			"手榴弾", 0xffffff, fontInfo_->GetHandle());
		break;
	case GRENADE_TYPE::SMOKE:
		DrawStringToHandle(1470, 910,
			"発煙手榴弾", 0xffffff, fontInfo_->GetHandle());
		break;
	case GRENADE_TYPE::STICKY:
		DrawStringToHandle(1485, 910,
			"粘着榴弾", 0xffffff, fontInfo_->GetHandle());
		break;
	}

	if (gameManager_.GetMode() == GameManager::MODE::TIMEATTACK) {
		DrawStringToHandle(33, 40,
			"TIMEATTACK", 0xffffff, fontMode_->GetHandle());
		DrawStringToHandle(320, 49,
			"-タイムアタック", 0xffffff, fontInfo_->GetHandle());
	}
	else if (gameManager_.GetMode() == GameManager::MODE::SCOREATTACK) {
		DrawStringToHandle(33, 40,
			"SCOREATTACK", 0xffffff, fontMode_->GetHandle());
		DrawStringToHandle(355, 49,
			"-スコアアタック", 0xffffff, fontInfo_->GetHandle());
	}


	player_->Draw();
	gun_->Draw();
	grenades_[0]->Draw();
	grenades_[1]->Draw();
}
