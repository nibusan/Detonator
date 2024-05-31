#include <DxLib.h>
#include "../Application.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/CollisionManager.h"
#include "../Manager/EffectManager.h"
#include "../Manager/AudioManager.h"
#include "../Manager/Camera/GameCamera.h"
#include "../Utility/Utility.h"
#include "../Net/NetManager.h"
#include "../Object/Map/MapBase.h"
#include "../Object/Actor/Player/Player.h"
#include "../UI/UILayer/UILayer_mGame.h"
#include "../UI/UILayer/UILayer_Pause.h"
#include "../UI/UILayer/UILayer_mGame_Start.h"
#include "../UI/UIScreen/UIScreen.h"
//#include "../Object/Actor/Player/PlayerMulti.h"
#include "MultiGameScene.h"

using namespace std;

MultiGameScene::MultiGameScene(void) : gameManager_(GameManager::GetInstance())
{
}

void MultiGameScene::Init(void)
{
	auto& net = NetManager::GetInstance();
	auto& players = net.GetNetUsers();

	// 自分自身
	keyMain_ = net.GetSelf().playerType;
	// コントローラ初期化
	auto& input = InputManager::GetInstance();
	input.ClearController();

	std::weak_ptr<Font> s = std::dynamic_pointer_cast<Font>(ResourceManager::GetInstance().
		Get("Font_GenShinGothic_Bold"));

	usingFont_ = std::make_shared<Font>("", 48);
	usingFont_->SetIsAutoDeleteHandle(true);
	fontScore_ = std::make_shared<Font>(s.lock()->GetFontName(), 80);
	fontScore_->SetIsAutoDeleteHandle(true);
	fontGoal_ = std::make_shared<Font>(s.lock()->GetFontName(), 25);
	fontGoal_->SetIsAutoDeleteHandle(true);
	fontDamage_ = std::make_shared<Font>(s.lock()->GetFontName(), 15);
	fontDamage_->SetIsAutoDeleteHandle(true);
	fontDeath_ = std::make_shared<Font>(s.lock()->GetFontName(), 70);
	fontDeath_->SetIsAutoDeleteHandle(true);
	fontDeathInfo_ = std::make_shared<Font>(s.lock()->GetFontName(), 50);
	fontDeathInfo_->SetIsAutoDeleteHandle(true);

	auto camera = SceneManager::GetInstance().GetCamera();
	auto gCamera = std::dynamic_pointer_cast<GameCamera>(camera);

	for (auto& player : players)
	{
		input.CreateController((int)player.second.playerType, player.second.key);

		auto info = gameManager_.GetPlayerInfo((int)player.second.playerType);
		VECTOR pos = info.respawnPos;
		VECTOR rot = info.respawnRot;
		float moveConZ = info.moveControlPosZ;
		float actionConZ = info.actionControlPosZ;
		int team = (int)info.team;
		auto p = std::make_shared<Player>(gameManager_.GetLoadOut((int)player.second.playerType), pos, rot,
			player.second.key, (ID)player.second.playerType, team, moveConZ, actionConZ);
		p->Init();
		players_.emplace(player.second.playerType, p);
		//gCamera->SetFollowTarget((int)player.second.playerType, players_.at(player.second.playerType));
		gCamera->ChangeMode((int)player.second.playerType, GameCamera::MODE::FIXED_POINT, players_.at(player.second.playerType));
	}

	map_ = std::make_shared<MapBase>();
	map_->Init();

	gCamera->ChangeMode(GameCamera::MODE::FIXED_POINT, players_.at(keyMain_));

	vignetteScreen_ = std::make_unique<Graphic>(MakeScreen(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, true));
	pixelShader_Vignette_ = std::dynamic_pointer_cast<PixelShader>(ResourceManager::GetInstance().Get("PixelShader_Vignette"));
	pixelShader_Vignette_.lock()->CreateConstantBuffer(sizeof(float) * 4);
	pixelShader_Vignette_.lock()->SetConstantValue(0, 1.0f);
	pixelShader_Vignette_.lock()->SetConstantValue(1, 0.0f);
	pixelShader_Vignette_.lock()->SetConstantValue(2, 0.0f);
	pixelShader_Vignette_.lock()->SetConstantValue(3, 0.0f);

	skyDoom_.SetModel(
		MV1LoadModel("Data/Model/SkyDoom/SkyDome.mv1"));
	float scale = 2.0f;
	skyDoom_.pos = { 0.0f ,0.0f ,0.0f };
	skyDoom_.scl = { scale ,scale ,scale };
	skyDoom_.quaRotLocal =
		Quaternion::Euler({ Utility::Deg2RadF(-90.0f) ,0.0f ,0.0f });
	skyDoom_.Update();


	auto player = std::dynamic_pointer_cast<Player>(players_.at(keyMain_));

	AudioManager::GetInstance().SetListenerInfo(gCamera->GetCameraPos(), player->GetCameraDir());

	std::unique_ptr<UILayerBase> uiLayer = std::make_unique<UILayer_mGame_Start>();
	uiLayer->Init();
	auto uiScreen = SceneManager::GetInstance().GetUIScreen();
	uiScreen.lock()->PushUILayer(std::move(uiLayer));

	isDisplayUI_ = false;
	test_ = false;
	isActive_ = true;
	reloadAlphaRad_ = 0.0f;

	timeFrame_ = LoadGraph("Data/Image/UI/Icon_Game_HUD_Time_BG.png");
	time_ = LoadGraph("Data/Image/UI/Icon_Game_HUD_Time.png");
	timeIcon_ = LoadGraph("Data/Image/UI/Icon_Game_HUD_Time_Info.png");
	blueFrame_ = LoadGraph("Data/Image/UI/Icon_Game_HUD_Team_Kill_Bule_Frame.png");
	blue_ = LoadGraph("Data/Image/UI/Icon_Game_HUD_Team_Kill_Blue.png");
	redFrame_ = LoadGraph("Data/Image/UI/Icon_Game_HUD_Team_Kill_Red_Frame.png");
	red_ = LoadGraph("Data/Image/UI/Icon_Game_HUD_Team_Kill_Red.png");
	wolf_ = LoadGraph("Data/Image/UI/Icon_Game_HUD_Wolf.png");
	eagle_ = LoadGraph("Data/Image/UI/Icon_Game_HUD_Eagle.png");
	/*Sound_3DTest_ =
		AudioManager::GetInstance().Play(AudioList::AUDIO_TYPE::_3DTEST, player->GetTransform().pos, player->GetTransform().GetForward(), { 0.0f,0.0f,0.0f }, 2000.0f);*/
}

void MultiGameScene::Update(void)
{
	auto player = std::dynamic_pointer_cast<Player>(players_.at(keyMain_));

	if (gameManager_.IsStart() && !isDisplayUI_ && !test_) {
		std::unique_ptr<UILayerBase> uiLayer = std::make_unique<UILayer_mGame>();
		uiLayer->Init();
		auto uiScreen = SceneManager::GetInstance().GetUIScreen();
		uiScreen.lock()->PopUILayer();
		uiScreen.lock()->PushUILayer(std::move(uiLayer));
		isDisplayUI_ = true;
		test_ = true;
	}

	//if (gameManager_.IsStart()) {
	//	if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_ESCAPE))
	//	{
	//		isActive_ = !isActive_;
	//		gameManager_.SetActive(isActive_);
	//		if (!isActive_) {
	//			std::unique_ptr<UILayerBase> uiLayer = std::make_unique<UILayer_Pause>();
	//			uiLayer->Init();
	//			auto uiScreen = SceneManager::GetInstance().GetUIScreen();
	//			uiScreen.lock()->PushUILayer(std::move(uiLayer));
	//			SetMouseDispFlag(true);
	//		}
	//		else {
	//			SetMouseDispFlag(false);
	//		}
	//	}
	//}

	//AudioManager::GetInstance().Update_3DSound(Sound_3DTest_, player->GetTransform().pos, player->GetTransform().GetForward(), { 0.0f,0.0f,0.0f }, 2000.0f);

	//if (!GameManager::GetInstance().IsStart() ||
	//	!GameManager::GetInstance().IsEndGame()) {
	//	Vector2 center = { Application::SCREEN_SIZE_X / 2,Application::SCREEN_SIZE_Y / 2 };
	//	SetMousePoint(center.x, center.y);
	//}


	//// シーン遷移
	//InputManager& ins = InputManager::GetInstance();
	//if (ins.IsTrgDown(KEY_INPUT_Z))
	//{
	//	SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::mRESULT);
	//}



	for (auto& player : players_){
		player.second->Update();
	}

	for (auto& player : players_){
		auto p = std::dynamic_pointer_cast<Player>(player.second);
		auto gun = p->GetGun();
		gun->Update();
	}

	for (auto& player : players_){
		auto p = std::dynamic_pointer_cast<Player>(player.second);
		auto gun = p->GetGun();
		auto& bullets = gun->GetBullets();
		for (auto& bullet : bullets) {
			bullet->Update();
		}
	}

	for (auto& player : players_){
		auto p = std::dynamic_pointer_cast<Player>(player.second);
		auto& gres = p->GetGrenades();
		for (auto& gre : gres) {
			gre->Update();
		}
	}

	map_->Update();

	CollisionManager::GetInstance().CollisionMulti();

	for (auto& player : players_)
	{
		//player.second->UpdateAfterCollision();
	}

	// 通信を送る
	NetManager::GetInstance().Send(NET_DATA_TYPE::ACTION_HIS_ALL);

	//if (NetManager::GetInstance().IsSameGameState(GAME_STATE::GOTO_RESULT))
	//{
	//	SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::mRESULT);
	//}

	if (gameManager_.IsStart()) {
		auto gCamera = std::dynamic_pointer_cast<GameCamera>(SceneManager::GetInstance().GetCamera());
		bool tmpIsDisplayUI = isDisplayUI_;
		if (gCamera->GetMode() == GameCamera::MODE::DEATH) {
			isDisplayUI_ = false;
			if (test_ && tmpIsDisplayUI && !isDisplayUI_) {
				auto uiScreen = SceneManager::GetInstance().GetUIScreen();
				uiScreen.lock()->PopUILayer();
			}
		}
		else {
			isDisplayUI_ = true;
			if (test_ && !tmpIsDisplayUI && isDisplayUI_) {
				std::unique_ptr<UILayerBase> uiLayer = std::make_unique<UILayer_mGame>();
				uiLayer->Init();
				auto uiScreen = SceneManager::GetInstance().GetUIScreen();
				uiScreen.lock()->PushUILayer(std::move(uiLayer));
			}
		}
	}

	auto maxHP = std::dynamic_pointer_cast<ActorBase>(player)->GetHPMAX();
	auto hp = std::dynamic_pointer_cast<ActorBase>(player)->GetHP();
	float rate = 1.0f - (hp / maxHP);
	pixelShader_Vignette_.lock()->SetConstantValue(3, rate * 25.0f);

}

void MultiGameScene::Draw(void)
{

	auto playeraaa = std::dynamic_pointer_cast<Player>(players_.at(keyMain_));

	auto defScreen = DxLib::GetDrawScreen();
	SetDrawScreen(vignetteScreen_->GetHandle());
	ClearDrawScreen();

	auto camera = SceneManager::GetInstance().GetCamera();
	camera->SetBeforeDraw();

	SetUseLighting(false);
	skyDoom_.Draw();
	SetUseLighting(true);

	map_->Draw();

	for (auto& player : players_)
	{
		player.second->Draw();
	}

	EffectManager::GetInstance().Draw();

	for (auto& player : players_)
	{
		player.second->DrawTrans();
	}

	auto& damageData = GameManager::GetInstance().GetDamageData();
	for (int i = 0; i < damageData.size(); i++) {
		damageData[i].time_ -= 0.016666666f;

		if (damageData[i].time_ < 0.0f) {
			//damageData.erase(damageData.begin() + i);
		}

		auto damageDrawPos = ConvWorldPosToScreenPos(damageData[i].pos);
		if (damageDrawPos.z > 1.0f) continue;
		SetDrawBlendMode(DX_BLENDGRAPHTYPE_ALPHA, static_cast<int>((damageData[i].time_) * 255));
		int color = GetColor(damageData[i].color_.x, damageData[i].color_.y, damageData[i].color_.z);
		DrawStringToHandle(damageDrawPos.x, damageDrawPos.y, std::to_string(damageData[i].damage_).c_str(), color, fontDamage_->GetHandle());
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	std::erase_if(damageData, [](GameManager::DamageData x) {return x.time_ < 0.0f; });

	auto player = std::dynamic_pointer_cast<Player>(playeraaa);
	auto magazine = GameManager::GetInstance().GetMagazine((int)NetManager::GetInstance().GetSelf().playerType);
	if (GameManager::GetInstance().IsStart()) {
		if (magazine <= 0) {
			reloadAlphaRad_ += Utility::Deg2RadF(2.0f);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(fabsf(sinf(reloadAlphaRad_)) * 255));
			int width = GetDrawStringWidthToHandle("弾切れ", 3, usingFont_->GetHandle());
			DrawStringToHandle(Application::SCREEN_SIZE_X / 2 - width / 2 - 50, 492, "弾切れ", 0xFFFFFF, usingFont_->GetHandle());
			int width2 = GetDrawStringWidthToHandle("Rでリロード", 6, usingFont_->GetHandle());
			DrawStringToHandle(Application::SCREEN_SIZE_X / 2 - width / 2 - 110, 540, "Rでリロード", 0xFFFFFF, usingFont_->GetHandle());
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
		else {
			reloadAlphaRad_ = 0.0f;
		}
	}

	auto gameCamera = std::dynamic_pointer_cast<GameCamera>(camera);
	if (gameManager_.IsStart() && gameCamera->GetMode() != GameCamera::MODE::DEATH) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
		DrawGraph(Application::SCREEN_SIZE_X / 2 - 160, 0, timeFrame_, true);
		DrawGraph(Application::SCREEN_SIZE_X / 2 - 427, 0, blue_, true);
		DrawGraph(Application::SCREEN_SIZE_X / 2 + 157, 0, red_, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		DrawGraph(Application::SCREEN_SIZE_X / 2 - 160, 0, time_, true);
		DrawGraph(Application::SCREEN_SIZE_X / 2 + 157, 0, redFrame_, true);
		DrawGraph(Application::SCREEN_SIZE_X / 2 - 427, 0, blueFrame_, true);

		DrawGraph(Application::SCREEN_SIZE_X / 2 - 160, 10, timeIcon_, true);
		DrawFormatStringToHandle(Application::SCREEN_SIZE_X / 2 - 70, 80, 0xffffff, fontGoal_->GetHandle(),
			"目標スコア：%d", gameManager_.GetGoalTDMScore());

		int no = (int)NetManager::GetInstance().GetSelf().playerType;
		auto team = gameManager_.GetTeam(no);
		if (team == GameManager::TEAM::WOLF) {
			DrawGraph(Application::SCREEN_SIZE_X / 2 - 420, 10, wolf_, true);
			DrawGraph(Application::SCREEN_SIZE_X / 2 + 330, 10, eagle_, true);
			DrawFormatStringToHandle(Application::SCREEN_SIZE_X / 2 - 280, 10, 0xffffff, fontScore_->GetHandle(),
				"%d", gameManager_.GetTDMScore(GameManager::TEAM::WOLF));
			DrawFormatStringToHandle(Application::SCREEN_SIZE_X / 2 + 240, 10, 0xffffff, fontScore_->GetHandle(),
				"%d", gameManager_.GetTDMScore(GameManager::TEAM::EAGLE));
		}
		else {
			DrawGraph(Application::SCREEN_SIZE_X / 2 - 420, 10, eagle_, true);
			DrawGraph(Application::SCREEN_SIZE_X / 2 + 330, 10, wolf_, true);
			DrawFormatStringToHandle(Application::SCREEN_SIZE_X / 2 - 280, 10, 0xffffff, fontScore_->GetHandle(),
				"%d", gameManager_.GetTDMScore(GameManager::TEAM::EAGLE));
			DrawFormatStringToHandle(Application::SCREEN_SIZE_X / 2 + 240, 10, 0xffffff, fontScore_->GetHandle(),
				"%d", gameManager_.GetTDMScore(GameManager::TEAM::WOLF));
		}

		gameManager_.DrawHelpPessage();
		gameManager_.DrawKillMessage();
	}

	if (gameCamera->GetMode() == GameCamera::MODE::DEATH) {
		int width = GetDrawStringWidthToHandle(gameManager_.GetKillerName().c_str(),
			strlen(gameManager_.GetKillerName().c_str()), fontDeath_->GetHandle());
		DrawStringToHandle(Application::SCREEN_SIZE_X / 2 - width / 2, 700, gameManager_.GetKillerName().c_str(), 0xff0000, fontDeath_->GetHandle());
		DrawStringToHandle(Application::SCREEN_SIZE_X / 2 - 165, 800, "に圧倒された", 0xff0000, fontDeathInfo_->GetHandle());
	}

	SetDrawScreen(defScreen);
	vignetteScreen_->Draw({ 0.0f,0.0f }, false, pixelShader_Vignette_.lock());

}

void MultiGameScene::Release(void)
{
	for (auto& player : players_)
	{
		player.second->Release();
	}

	map_->Release();

	auto uiScreen = SceneManager::GetInstance().GetUIScreen();
	for (int i = 0; i < uiScreen.lock()->GetUILayerCount(); i++) {
		uiScreen.lock()->PopUILayer();
	}

	DeleteGraph(timeFrame_);
	DeleteGraph(time_);
	DeleteGraph(blueFrame_);
	DeleteGraph(blue_);
	DeleteGraph(redFrame_);
	DeleteGraph(red_);
	DeleteGraph(wolf_);
	DeleteGraph(eagle_);

	skyDoom_.Release();
}

