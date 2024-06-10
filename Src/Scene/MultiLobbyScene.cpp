#include <DxLib.h>
#include "../Application.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Utility/Utility.h"
#include "../Object/Gun/GunMenu.h"
#include "../Object/Grenade/GrenadeMenu.h"
#include "../UI/UILayer/UILayer_mLobby.h"
#include "../UI/UIScreen/UIScreen.h"
#include "MultiLobbyScene.h"

MultiLobbyScene::MultiLobbyScene(void) : gameManager_(GameManager::GetInstance()) , netManager_(NetManager::GetInstance())
{
}

void MultiLobbyScene::Init(void)
{
	UISetting();
}

void MultiLobbyScene::Update(void)
{

	if (netManager_.IsSameGameState(GAME_STATE::GOTO_GAME)){
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::mGAME);
	}
	else if (netManager_.HostIsNotFound()) {
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::MENU);
		netManager_.Stop();
	}
	
	int id = (int)netManager_.GetSelf().playerType;
	gun_->SetID((ID)id);
	gun_->Update();
	grenades_[0]->SetID((ID)id);
	grenades_[0]->Update();
	grenades_[1]->SetID((ID)id);
	grenades_[1]->Update();
}

void MultiLobbyScene::Draw(void)
{
	DrawUI();
}

void MultiLobbyScene::Release(void)
{
	auto uiScreen = SceneManager::GetInstance().GetUIScreen();
	for (int i = 0; i < uiScreen.lock()->GetUILayerCount(); i++) {
		uiScreen.lock()->PopUILayer();
	}

	gun_->Release();
	grenades_[0]->Release();
	grenades_[1]->Release();
	box_.Release();

	DeleteGraph(back_);
	DeleteGraph(menu_);
	DeleteGraph(wolf_);
	DeleteGraph(eagle_);
}

void MultiLobbyScene::UISetting(void)
{
	back_ = LoadGraph("Data/Image/Menu_Background.png");
	menu_ = LoadGraph("Data/Image/UI/Menu_Multi.png");
	wolf_ = LoadGraph("Data/Image/UI/Icon_Game_HUD_Wolf.png");
	eagle_ = LoadGraph("Data/Image/UI/Icon_Game_HUD_Eagle.png");

	int id = (int)netManager_.GetSelf().playerType;

	VECTOR pos = { 35.0f,0.0f,120.0f };
	gun_ = std::make_shared<GunMenu>((ID)id, pos);

	pos = { -3.0f,-70.0f,200.0f };
	grenades_[0] = std::make_shared<GrenadeMenu>((ID)id, 0, pos);
	pos = { 125.0f,-70.0f,200.0f };
	grenades_[1] = std::make_shared<GrenadeMenu>((ID)id, 1, pos);



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
	fontGun_ = std::make_unique<Font>(s.lock()->GetFontName(), 35);
	fontGun_->SetIsAutoDeleteHandle(true);
	fontSmoll_ = std::make_unique<Font>(s.lock()->GetFontName(), 20);
	fontSmoll_->SetIsAutoDeleteHandle(true);

	std::unique_ptr<UILayerBase> uiLayer = std::make_unique<UILayer_mLobby>();
	uiLayer->Init();
	auto uiScreen = SceneManager::GetInstance().GetUIScreen();
	uiScreen.lock()->PushUILayer(std::move(uiLayer));
}

void MultiLobbyScene::DrawUI(void)
{
	DrawGraph(0, 0, back_, true);
	DrawGraph(0, 0, menu_, true);

	box_.Draw();

	int id = (int)netManager_.GetSelf().playerType;
	if (!netManager_.IsRunning())id = 0;
	auto info = gameManager_.GetLoadOut(id);
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

	DrawStringToHandle(33, 40,
		"TEAMDEATHMATCH", 0xffffff, fontMode_->GetHandle());
	DrawStringToHandle(460, 49,
		"-チームデスマッチ", 0xffffff, fontInfo_->GetHandle());
	DrawStringToHandle(33, 90,
		"武器選択", 0xffffff, fontGun_->GetHandle());

	// マルチ描画
	auto ip = netManager_.GetHostIp();
	DrawFormatStringToHandle(1350, 20, 0xffffff, fontInfo_->GetHandle(),"ホストのアドレス：%d.%d.%d.%d",
		ip.d1, ip.d2, ip.d3, ip.d4);

	auto& players = netManager_.GetNetUsers();
	int i = 0;
	for (auto& p : players) {
		std::string name;
		for (auto s : p.second.playerName) {
			if (s == 0)continue;
			name.push_back(s);
		}
		gameManager_.SetName((int)p.second.playerType, name);
		DrawStringToHandle(160, 325 + 75.0f * i, name.c_str(), 0xffffff, fontInfo_->GetHandle());
		DrawStringToHandle(440, 325 + 75.0f * i, "TEAM", 0xffffff, fontSmoll_->GetHandle());
		switch (p.second.team)
		{
		case 0:
			DrawRotaGraph(400, 340 + 75.0f * i, 0.8f, 0.0f, wolf_, true);
			DrawStringToHandle(440, 345 + 75.0f * i, "WOLF", 0xffffff, fontSmoll_->GetHandle());
			break;
		case 1:
			DrawRotaGraph(400, 340 + 75.0f * i, 0.8f, 0.0f, eagle_, true);
			DrawStringToHandle(440, 345 + 75.0f * i, "EAGLE", 0xffffff, fontSmoll_->GetHandle());
			break;
		}
		if (p.second.key == netManager_.GetHostJoinUser().key) {
			DrawStringToHandle(35, 325 + 75.0f * i, "ホスト", 0xffffff, fontInfo_->GetHandle());
		}
		i++;
	}

	DrawStringToHandle(185, 820, "ホストのみ開始可能", 0xffffff, fontInfo_->GetHandle());
	if (!gameManager_.IsDivideTeam()) {
		DrawStringToHandle(210, 1000, "チームを分けてください", 0xffffff, fontSmoll_->GetHandle());
		DrawStringToHandle(179, 1020, "人数差は1人まで認められます", 0xffffff, fontSmoll_->GetHandle());
	}

	gun_->Draw();
	grenades_[0]->Draw();
	grenades_[1]->Draw();
}
