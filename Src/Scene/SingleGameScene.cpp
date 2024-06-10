#include <DxLib.h>
#include "../Application.h"
#include "../Manager/AudioManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/AudioManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
//#include "../Manager/Camera/CameraBase.h"
#include "../Manager/Camera/GameCamera.h"
#include "../Manager/EffectManager.h"
#include "../Manager/CollisionManager.h"
#include "../Manager/GameManager.h"
#include "../Manager/Camera/GameCamera.h"
#include "../Utility/Utility.h"
#include "../Object/Map/MapBase.h"
#include "../Object/Actor/Player/Player.h"
#include "../UI/UILayer/UILayer_Pause.h"
#include "../UI/UILayer/UILayer_sGame_ScoreAttack_Start.h"
#include "../UI/UILayer/UILayer_sGame_TimeAttack_Start.h"
#include "../UI/UILayer/UILayer_sGame.h"
#include "../UI/UIScreen/UIScreen.h"
#include "SingleGameScene.h"

SingleGameScene::SingleGameScene(void) : gameManager_(GameManager::GetInstance())
{
}

void SingleGameScene::Init(void)
{
	// コントローラ初期化
	auto& input = InputManager::GetInstance();
	input.ClearController();
	input.CreateController(0, -1);

	std::weak_ptr<Font> s = std::dynamic_pointer_cast<Font>(ResourceManager::GetInstance().
		GetResourceFile("Font_GenShinGothic_Bold"));

	fontDeath_ = std::make_unique<Font>(s.lock()->GetFontName(), 70);
	fontDeath_->SetIsAutoDeleteHandle(true);
	fontDamage_ = std::make_unique<Font>(s.lock()->GetFontName(), 15);
	fontDamage_->SetIsAutoDeleteHandle(true);
	fontDeathInfo_ = std::make_unique<Font>(s.lock()->GetFontName(),50);
	fontDeathInfo_->SetIsAutoDeleteHandle(true);

	usingFont_ = std::make_shared<Font>("", 48);
	usingFont_->SetIsAutoDeleteHandle(true);

	auto info = GameManager::GetInstance().GetPlayerInfo(0);
	VECTOR pos = info.respawnPos;
	VECTOR rot = info.respawnRot;
	player_ = std::make_shared<Player>(GameManager::GetInstance().GetLoadOut(0), pos, rot);
	player_->Init();

	gameManager_.SetEnemyFollow(player_);

	map_ = std::make_shared<MapBase>();
	map_->Init();

	auto camera  = SceneManager::GetInstance().GetCamera();
	auto gCamera = std::dynamic_pointer_cast<GameCamera>(camera);
	gCamera->ChangeMode(GameCamera::MODE::FIXED_POINT, player_);
	gCamera->ChangeMode(0, GameCamera::MODE::FIXED_POINT, player_);
	//gCamera->SetFollowTarget(0, player_);

	// シーンタイプ変更

	auto player = std::dynamic_pointer_cast<Player>(player_);


	CollisionManager::GetInstance().CollisionSingle();

	skyDoom_.SetModel(
		MV1LoadModel("Data/Model/SkyDoom/SkyDome.mv1"));
	float scale = 2.0f;
	skyDoom_.pos = { 0.0f ,0.0f ,0.0f };
	skyDoom_.scl = { scale ,scale ,scale };
	skyDoom_.quaRotLocal = 
		Quaternion::Euler({ Utility::Deg2RadF(-90.0f) ,0.0f ,0.0f});
	skyDoom_.Update();

	//// カメラ位置を中心にセット
	//Vector2 center = { Application::SCREEN_SIZE_X / 2,Application::SCREEN_SIZE_Y / 2 };
	//SetMousePoint(center.x, center.y);

	vignetteScreen_ = std::make_unique<Graphic>(MakeScreen(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, true));
	pixelShader_Vignette_ = std::dynamic_pointer_cast<PixelShader>(ResourceManager::GetInstance().GetResourceFile("PixelShader_Vignette"));
	pixelShader_Vignette_.lock()->CreateConstantBuffer(sizeof(float) * 4);
	pixelShader_Vignette_.lock()->SetConstantValue(0, 1.0f);
	pixelShader_Vignette_.lock()->SetConstantValue(1, 0.0f);
	pixelShader_Vignette_.lock()->SetConstantValue(2, 0.0f);
	pixelShader_Vignette_.lock()->SetConstantValue(3, 0.0f);

	AudioManager::GetInstance().SetListenerInfo(gCamera->GetCameraPos(), player->GetCameraDir());
	
	auto gamemode = gameManager_.GetMode();
	switch (gamemode) {
	case GameManager::MODE::TIMEATTACK: {
		std::unique_ptr<UILayerBase> uiLayer = std::make_unique<UILayer_sGame_TimeAttack_Start>();
		uiLayer->Init();
		auto uiScreen = SceneManager::GetInstance().GetUIScreen();
		uiScreen.lock()->PushUILayer(std::move(uiLayer));
	}
		break;

	case GameManager::MODE::SCOREATTACK: {
		std::unique_ptr<UILayerBase> uiLayer = std::make_unique<UILayer_sGame_ScoreAttack_Start>();
		uiLayer->Init();
		auto uiScreen = SceneManager::GetInstance().GetUIScreen();
		uiScreen.lock()->PushUILayer(std::move(uiLayer));
	}
		break;

	case GameManager::MODE::HOST:
	case GameManager::MODE::CLIENT:
	case GameManager::MODE::SETTINGS:
	case GameManager::MODE::NONE:
	default:
		break;
	}
	isDisplayUI_ = false;
	test_ = false;
	isActive_ = true;
	reloadAlphaRad_ = 0.0f;

	g1_ = LoadGraph("Data/Image/UI/Icon_Game_HUD_Time_BG.png");
	g2_ = LoadGraph("Data/Image/UI/Icon_Game_HUD_Time.png");
	g3_ = LoadGraph("Data/Image/UI/Icon_Game_HUD_Time_Info.png");
}

void SingleGameScene::Update(void)
{
	if (GameManager::GetInstance().IsStart() && !isDisplayUI_ && !test_) {
		std::unique_ptr<UILayerBase> uiLayer = std::make_unique<UILayer_sGame>();
		uiLayer->Init();
		auto uiScreen = SceneManager::GetInstance().GetUIScreen();
		uiScreen.lock()->PopUILayer();
		uiScreen.lock()->PushUILayer(std::move(uiLayer));
		isDisplayUI_ = true;
		test_ = true;
	}

	//AudioManager::GetInstance().Update_3DSound(Sound_3DTest_, player_->GetTransform().pos, player_->GetTransform().GetForward(), { 0.0f,0.0f,0.0f }, 2000.0f);


	// シーン遷移
	InputManager& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_RETURN))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::sRESULT);
	}

	if (gameManager_.IsStart()) {
		if (ins.IsTrgDown(KEY_INPUT_ESCAPE))
		{
			auto isA = gameManager_.IsActive();
			gameManager_.SetActive(!isA);
			if (!gameManager_.IsActive()) {
				std::unique_ptr<UILayerBase> uiLayer = std::make_unique<UILayer_Pause>();
				uiLayer->Init();
				auto uiScreen = SceneManager::GetInstance().GetUIScreen();
				uiScreen.lock()->PushUILayer(std::move(uiLayer));
				SetMouseDispFlag(true);
			}
			else {
				auto uiScreen = SceneManager::GetInstance().GetUIScreen();
				uiScreen.lock()->PopUILayer();
				SetMouseDispFlag(false);
			}
		}
	}

	if (!gameManager_.IsActive()) return;

	//if (ins.IsTrgDown(KEY_INPUT_B))
	//{
	//	/*EffectManager::EFFECT bubble = {
	//		EffectManager::EFFECT_TYPE::UTSUHO,
	//		{0.0f,0.0f,0.0f},
	//		{10.0f,10.0f,10.0f},
	//		{0.0f,0.0f,1.0f},
	//		1.0f
	//	};

	//	EffectManager::GetInstance().AddEffect(bubble);*/
	//}

	player_->Update();

	auto camera = SceneManager::GetInstance().GetCamera();
	camera->Update();

	if (GameManager::GetInstance().IsStart()) {
		auto gCamera = std::dynamic_pointer_cast<GameCamera>(camera);
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
				std::unique_ptr<UILayerBase> uiLayer = std::make_unique<UILayer_sGame>();
				uiLayer->Init();
				auto uiScreen = SceneManager::GetInstance().GetUIScreen();
				uiScreen.lock()->PushUILayer(std::move(uiLayer));
			}
		}
	}


	// お試しコードーーーーーーーーーーーーーーーーーーー

	auto player = std::dynamic_pointer_cast<Player>(player_);

	auto gun = player->GetGun();
	gun->Update();

	auto& bullets = gun->GetBullets();
	for (auto& bullet : bullets) {
		bullet->Update();
	}

	auto& gres = player->GetGrenades();
	for (auto& gre : gres) {
		gre->Update();
	}

	//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー

	gameManager_.UpdateEnemy();


	map_->Update();

	CollisionManager::GetInstance().CollisionSingle();


	//// カメラ位置を中心にセット
	//Vector2 center = { Application::SCREEN_SIZE_X / 2,Application::SCREEN_SIZE_Y / 2 };
	//SetMousePoint(center.x, center.y);

	auto maxHP = std::dynamic_pointer_cast<ActorBase>(player_)->GetHPMAX();
	auto hp = std::dynamic_pointer_cast<ActorBase>(player_)->GetHP();
	float rate = 1.0f - (hp / maxHP);
	pixelShader_Vignette_.lock()->SetConstantValue(3, rate * 25.0f);
}

void SingleGameScene::Draw(void)
{

	// デバッグ
	auto camera = std::dynamic_pointer_cast<GameCamera>(SceneManager::GetInstance().GetCamera());

	auto defScreen = DxLib::GetDrawScreen();
	SetDrawScreen(vignetteScreen_->GetHandle());
	ClearDrawScreen();

	//auto camera = SceneManager::GetInstance().GetCamera();
	camera->SetBeforeDraw();

	SetUseLighting(false);
	skyDoom_.Draw();
	SetUseLighting(true);

	gameManager_.DrawEnemy();

	map_->Draw();

	player_->Draw();

	EffectManager::GetInstance().Draw();

	player_->DrawTrans();

	//DrawLine(960, 0, 960, 1080, 0xffffff, true);

	auto& damageData = gameManager_.GetDamageData();
	for (int i = 0; i < damageData.size(); i++) {
		damageData[i].time_ -= 0.016666666f;
		
		if (damageData[i].time_ < 0.0f) {
			//damageData.erase(damageData.begin() + i);
		}

		auto damageDrawPos = ConvWorldPosToScreenPos(damageData[i].pos);
		if (damageDrawPos.z > 1.0f) continue;
		SetDrawBlendMode(DX_BLENDGRAPHTYPE_ALPHA, static_cast<int>((damageData[i].time_) * 255));
		int color = GetColor(damageData[i].color_.x, damageData[i].color_.y, damageData[i].color_.z);
		DrawStringToHandle(damageDrawPos.x,damageDrawPos.y,std::to_string(damageData[i].damage_).c_str(), color, fontDamage_->GetHandle());
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	std::erase_if(damageData, [](GameManager::DamageData x) {return x.time_ < 0.0f; });

	auto player = std::dynamic_pointer_cast<Player>(player_);
	auto magazine = GameManager::GetInstance().GetMagazine(0);
	if (gameManager_.IsStart()) {
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

	if (gameManager_.IsStart() && camera->GetMode() != GameCamera::MODE::DEATH) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
		DrawGraph(Application::SCREEN_SIZE_X / 2 - 160, 0, g1_, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		DrawGraph(Application::SCREEN_SIZE_X / 2 - 160, 10, g3_, true);
		DrawGraph(Application::SCREEN_SIZE_X / 2 - 160, 0, g2_, true);
		gameManager_.DrawHelpPessage();
		gameManager_.DrawKillMessage();
	}

	if (camera->GetMode() == GameCamera::MODE::DEATH) {
		int width = GetDrawStringWidthToHandle(gameManager_.GetKillerName().c_str(), 
			strlen(gameManager_.GetKillerName().c_str()), fontDeath_->GetHandle());
		DrawStringToHandle(Application::SCREEN_SIZE_X / 2 - width / 2, 700, gameManager_.GetKillerName().c_str(), 0xff0000, fontDeath_->GetHandle());
		DrawStringToHandle(Application::SCREEN_SIZE_X / 2 - 165, 800, "に圧倒された", 0xff0000, fontDeathInfo_->GetHandle());
	}


	SetDrawScreen(defScreen);
	vignetteScreen_->Draw({ 0.0f,0.0f }, false, pixelShader_Vignette_.lock());
}

void SingleGameScene::Release(void)
{
	player_->Release();
	skyDoom_.Release();

	map_->Release();

	gameManager_.ReleseEnemy();

	DeleteGraph(g1_);
	DeleteGraph(g2_);
	DeleteGraph(g3_);

	auto uiScreen = SceneManager::GetInstance().GetUIScreen();
	for (int i = 0; i < uiScreen.lock()->GetUILayerCount(); i++) {
		uiScreen.lock()->PopUILayer();
	}
}
