#include <DxLib.h>
#include "../Application.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../UI/UILayer/UILayer_sResult.h"
#include "../UI/UIScreen/UIScreen.h"
#include "SingleResultScene.h"

SingleResultScene::SingleResultScene(void) : gameManager_(GameManager::GetInstance())
{
}

void SingleResultScene::Init(void)
{
	std::unique_ptr<UILayerBase> uiLayer = std::make_unique<UILayer_sResult>();
	uiLayer->Init();
	auto uiScreen = SceneManager::GetInstance().GetUIScreen();
	uiScreen.lock()->PushUILayer(std::move(uiLayer));

	UISetting();
}

void SingleResultScene::Update(void)
{

}

void SingleResultScene::Draw(void)
{
	DrawUI();
}

void SingleResultScene::Release(void)
{
	auto uiScreen = SceneManager::GetInstance().GetUIScreen();
	uiScreen.lock()->PopUILayer();

	DeleteGraph(back_);
}

void SingleResultScene::UISetting(void)
{
	back_ = LoadGraph("Data/Image/UI/Result_Window_Single.png");

	std::weak_ptr<Font> s = std::dynamic_pointer_cast<Font>(ResourceManager::GetInstance().
		GetResourceFile("Font_GenShinGothic_Bold"));

	fontHeading_ = std::make_unique<Font>(s.lock()->GetFontName(), 65);
	fontHeading_->SetIsAutoDeleteHandle(true);
	fontSubheading_ = std::make_unique<Font>(s.lock()->GetFontName(), 45);
	fontSubheading_->SetIsAutoDeleteHandle(true);
	fontInfo_ = std::make_unique<Font>(s.lock()->GetFontName(), 35);
	fontInfo_->SetIsAutoDeleteHandle(true);
	fontMode_ = std::make_unique<Font>(s.lock()->GetFontName(), 50);
	fontMode_->SetIsAutoDeleteHandle(true);
}

void SingleResultScene::DrawUI(void)
{
	// UIとして扱いたくないモノの描画

	DrawGraph(0, 0, back_, true);

	if (gameManager_.GetMode() == GameManager::MODE::TIMEATTACK) {
		DrawStringToHandle(60, 50, "TIMEATTACK", 0xffffff, fontMode_->GetHandle());
		DrawStringToHandle(410, 63, "-タイムアタック", 0xffffff, fontInfo_->GetHandle());
		float time = gameManager_.GetGameTime();
		int min = time / 60.0f;
		int sec = (int)(time) % 60;
		int ms = (time - (float)(min * 60 + sec)) * 100.0f;
		std::string str_min = (min < 10) ? "0" + std::to_string(min) : std::to_string(min);
		std::string str_sec = (sec < 10) ? "0" + std::to_string(sec) : std::to_string(sec);
		std::string str_ms = (ms < 10) ? "0" + std::to_string(ms) : std::to_string(ms);
		DrawStringToHandle(350, 240, ("TIME   " + str_min + ":" + str_sec + ":" + str_ms).c_str(),
			0xffffff, fontHeading_->GetHandle());


		DrawStringToHandle(400, 350,  "Detail", 0xffffff, fontSubheading_->GetHandle());
		DrawFormatStringToHandle(470, 440, 0xffffff, fontInfo_->GetHandle(), "ENAKEUS   Score 10000 x %d",
			gameManager_.GetEliminationNum(ID::ENEMY_GEORGE));
		DrawFormatStringToHandle(1050, 440, 0xffffff, fontInfo_->GetHandle(), "%d",
			gameManager_.GetEliminationNum(ID::ENEMY_GEORGE) * 10000);

		DrawStringToHandle(400, 640,  "Bonus", 0xffffff, fontSubheading_->GetHandle());
		if (gameManager_.GetDead(0) == 0) {
			DrawStringToHandle(470, 730, "NoDeath Score 1000 x 1", 0xffffff, fontInfo_->GetHandle());
			DrawStringToHandle(1050, 730, "1000", 0xffffff, fontInfo_->GetHandle());
		}
		
	}
	else if (gameManager_.GetMode() == GameManager::MODE::SCOREATTACK) {
		DrawStringToHandle(60, 50, "SCOREATTACK", 0xffffff, fontMode_->GetHandle());
		DrawStringToHandle(460, 63, "-スコアアタック", 0xffffff, fontInfo_->GetHandle());

		int bonus = 0;
		if (gameManager_.GetDead(0) == 0)bonus += 1000;

		DrawFormatStringToHandle(350, 240,
			0xffffff, fontHeading_->GetHandle(), "SCORE   %d", 
			gameManager_.GetEliminationNum(ID::ENEMY_NEUVIO_FOLLOW) * 3000 +
			gameManager_.GetEliminationNum(ID::ENEMY_NEUVIO_FIXED) * 2000 +
			gameManager_.GetEliminationNum(ID::ENEMY_NEUVIO_THROUGH) * 500 + bonus);


		DrawStringToHandle(400, 350, "Detail", 0xffffff, fontSubheading_->GetHandle());
		DrawFormatStringToHandle(470, 440, 0xffffff, fontInfo_->GetHandle(), "UAV_MA   Score 3000 x %d",
			gameManager_.GetEliminationNum(ID::ENEMY_NEUVIO_FOLLOW));
		DrawFormatStringToHandle(1050, 440, 0xffffff, fontInfo_->GetHandle(), "%d",
			gameManager_.GetEliminationNum(ID::ENEMY_NEUVIO_FOLLOW) * 3000);
		DrawFormatStringToHandle(470, 500, 0xffffff, fontInfo_->GetHandle(), "UAV_B2   Score 2000 x %d",
			gameManager_.GetEliminationNum(ID::ENEMY_NEUVIO_FIXED));
		DrawFormatStringToHandle(1050, 500, 0xffffff, fontInfo_->GetHandle(), "%d",
			gameManager_.GetEliminationNum(ID::ENEMY_NEUVIO_FIXED) * 2000);
		DrawFormatStringToHandle(470, 560, 0xffffff, fontInfo_->GetHandle(), "UAV_SS   Score 500 x %d",
			gameManager_.GetEliminationNum(ID::ENEMY_NEUVIO_THROUGH));
		DrawFormatStringToHandle(1050, 560, 0xffffff, fontInfo_->GetHandle(), "%d",
			gameManager_.GetEliminationNum(ID::ENEMY_NEUVIO_THROUGH) * 500);

		DrawStringToHandle(400, 640, "Bonus", 0xffffff, fontSubheading_->GetHandle());
		if (gameManager_.GetDead(0) == 0 && gameManager_.GetState() == GameManager::STATE::NONE) {
			DrawStringToHandle(470, 730, "NoDeath Score 1000 x 1", 0xffffff, fontInfo_->GetHandle());
			DrawStringToHandle(1050, 730, "1000", 0xffffff, fontInfo_->GetHandle());
		}
	}

}
