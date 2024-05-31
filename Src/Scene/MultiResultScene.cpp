#include <DxLib.h>
#include "../Application.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/GameManager.h"
#include "../Net/NetManager.h"
#include "../UI/UILayer/UILayer_mResult.h"
#include "../UI/UIScreen/UIScreen.h"
#include "MultiResultScene.h"

MultiResultScene::MultiResultScene(void) : gameManager_(GameManager::GetInstance()), netManager_(NetManager::GetInstance())
{
}

void MultiResultScene::Init(void)
{
	gameManager_.ChangeState(GameManager::STATE::NONE);
	UISetting();
}

void MultiResultScene::Update(void)
{
	// シーン遷移
	InputManager& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_SPACE))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::mLOBBY);
		netManager_.GetInstance().ChangeGameState(GAME_STATE::LOBBY);
	}
}

void MultiResultScene::Draw(void)
{
	DrawUI();
}

void MultiResultScene::Release(void)
{
	auto uiScreen = SceneManager::GetInstance().GetUIScreen();
	for (int i = 0; i < uiScreen.lock()->GetUILayerCount(); i++) {
		uiScreen.lock()->PopUILayer();
	}

	DeleteGraph(back_);
	for (auto& alias : alias_) {
		DeleteGraph(alias);
	}
}

void MultiResultScene::UISetting(void)
{
	back_ = LoadGraph("Data/Image/UI/Result_Window_Multi.png");
	alias_[(int)GameManager::ALIAS::NONE] = LoadGraph("Data/Image/UI/Icon_Result_Alias_None.png");
	alias_[(int)GameManager::ALIAS::MVP] = LoadGraph("Data/Image/UI/Icon_Result_Alias_MVP.png");
	alias_[(int)GameManager::ALIAS::KILLER] = LoadGraph("Data/Image/UI/Icon_Result_Alias_Killer.png");
	alias_[(int)GameManager::ALIAS::LASTKILL] = LoadGraph("Data/Image/UI/Icon_Result_Alias_LastKill.png");
	alias_[(int)GameManager::ALIAS::BOMMER] = LoadGraph("Data/Image/UI/Icon_Result_Alias_Bommer.png");
	alias_[(int)GameManager::ALIAS::UNDEAD] = LoadGraph("Data/Image/UI/Icon_Result_Alias_Undead.png");
	alias_[(int)GameManager::ALIAS::HITMAN] = LoadGraph("Data/Image/UI/Icon_Result_Alias_Hitman.png");
	alias_[(int)GameManager::ALIAS::COMEDIAN] = LoadGraph("Data/Image/UI/Icon_Result_Alias_Comedian.png");

	std::weak_ptr<Font> s = std::dynamic_pointer_cast<Font>(ResourceManager::GetInstance().
		Get("Font_GenShinGothic_Bold"));

	fontWinner_ = std::make_unique<Font>(s.lock()->GetFontName(), 60);
	fontWinner_->SetIsAutoDeleteHandle(true);
	fontScore_ = std::make_unique<Font>(s.lock()->GetFontName(), 80);
	fontScore_->SetIsAutoDeleteHandle(true);
	fontTeam_ = std::make_unique<Font>(s.lock()->GetFontName(), 70);
	fontTeam_->SetIsAutoDeleteHandle(true);
	fontName_ = std::make_unique<Font>(s.lock()->GetFontName(), 35);
	fontName_->SetIsAutoDeleteHandle(true);
	fontAlias_ = std::make_unique<Font>(s.lock()->GetFontName(), 30);
	fontAlias_->SetIsAutoDeleteHandle(true);
	fontMode_ = std::make_unique<Font>(s.lock()->GetFontName(), 50);
	fontMode_->SetIsAutoDeleteHandle(true);

	std::unique_ptr<UILayerBase> uiLayer = std::make_unique<UILayer_mResult>();
	uiLayer->Init();
	auto uiScreen = SceneManager::GetInstance().GetUIScreen();
	uiScreen.lock()->PushUILayer(std::move(uiLayer));
}

void MultiResultScene::DrawUI(void)
{


	DrawGraph(0, 0, back_, true);

	GameManager::PLAYER_INFO info[GameManager::MAX_PLAYER];
	for (int i = 0; i < GameManager::MAX_PLAYER; i++) {
		info[i] = gameManager_.GetPlayerInfo(i);
	}

	std::string alias[] = {
		"NONE",
		"MVP",
		"KILLER",
		"LASTKILL",
		"BOMMER",
		"UNDEAD",
		"HITMAN",
		"COMEDIAN"
	};

	auto team = gameManager_.GetWinner();

	std::string teamName[] = {
	"WOLF",
	"EAGLE"
	};

	std::string w[] = {
	"WINNER  ",
	"LOSER  ",
	"DRAW  "
	};


	if (team != GameManager::TEAM::MAX) {
		DrawStringToHandle(700, 160, (w[0] + teamName[(int)team]).c_str(), 0xffffff, fontWinner_->GetHandle());
		DrawStringToHandle(730, 860, (w[1] + teamName[((int)team + 1) % 2]).c_str(), 0xffffff, fontWinner_->GetHandle());
		DrawStringToHandle(490, 510, teamName[(int)team].c_str(), 0x000000, fontTeam_->GetHandle());
		DrawStringToHandle(1210, 510, teamName[((int)team + 1) % 2].c_str(), 0x000000, fontTeam_->GetHandle());
		DrawFormatStringToHandle(850, 500, 0x000000, fontScore_->GetHandle(), "%d  -  %d",
			gameManager_.GetTDMScore(team), gameManager_.GetTDMScore((GameManager::TEAM)(((int)team + 1) % 2)));
	}
	else {
		DrawStringToHandle(740, 160, (w[2] + teamName[0]).c_str(), 0xffffff, fontWinner_->GetHandle());
		DrawStringToHandle(740, 860, (w[2] + teamName[1]).c_str(), 0xffffff, fontWinner_->GetHandle());
		DrawStringToHandle(490, 510, teamName[0].c_str(), 0x000000, fontTeam_->GetHandle());
		DrawStringToHandle(1210, 510, teamName[1].c_str(), 0x000000, fontTeam_->GetHandle());
		DrawFormatStringToHandle(850, 500, 0x000000, fontScore_->GetHandle(), "%d  -  %d",
			gameManager_.GetTDMScore((GameManager::TEAM)0), gameManager_.GetTDMScore((GameManager::TEAM)1));
	}





	int win = 0;
	int loser = 0;
	if (team == GameManager::TEAM::MAX)team = GameManager::TEAM::WOLF;

	for (auto& inf : info) {
		if (team == inf.team && inf.alias != GameManager::ALIAS::NONE) {
			DrawStringToHandle(390 + 840 * win, 250, inf.playerName.c_str(), 0xffffff, fontName_->GetHandle());
			DrawStringToHandle(520 + 840 * win, 300, alias[(int)inf.alias].c_str(), 0xffffff, fontAlias_->GetHandle());
			DrawFormatStringToHandle(520 + 840 * win,340,0xffffff, fontAlias_->GetHandle(),
				"KILL %d",inf.kill);
			DrawFormatStringToHandle(520 + 840 * win, 380, 0xffffff, fontAlias_->GetHandle(),
				"DEATH %d", inf.death);
			DrawGraph(390 + 840 * win, 300, alias_[(int)inf.alias], true);
			win++;
		}else if(team != inf.team && inf.alias != GameManager::ALIAS::NONE){
			DrawStringToHandle(390 + 840 * loser, 675, inf.playerName.c_str(), 0xffffff, fontName_->GetHandle());
			DrawStringToHandle(520 + 840 * loser, 725, alias[(int)inf.alias].c_str(), 0xffffff, fontAlias_->GetHandle());
			DrawFormatStringToHandle(520 + 840 * loser, 765, 0xffffff, fontAlias_->GetHandle(),
				"KILL %d", inf.kill);
			DrawFormatStringToHandle(520 + 840 * loser, 805, 0xffffff, fontAlias_->GetHandle(),
				"DEATH %d", inf.death);
			DrawGraph(390 + 840 * loser, 725, alias_[(int)inf.alias], true);
			loser++;
		}
		else if (win < 2) {
			DrawStringToHandle(390 + 840 * win, 250, inf.playerName.c_str(), 0xffffff, fontName_->GetHandle());
			DrawStringToHandle(520 + 840 * win, 300, alias[(int)inf.alias].c_str(), 0xffffff, fontAlias_->GetHandle());
			DrawFormatStringToHandle(520 + 840 * win, 340, 0xffffff, fontAlias_->GetHandle(),
				"KILL %d", inf.kill);
			DrawFormatStringToHandle(520 + 840 * win, 380, 0xffffff, fontAlias_->GetHandle(),
				"DEATH %d", inf.death);
			DrawGraph(390 + 840 * win, 300, alias_[(int)inf.alias], true);
			win++;
		}
		else if (loser < 2) {
			DrawStringToHandle(390 + 840 * loser, 675, inf.playerName.c_str(), 0xffffff, fontName_->GetHandle());
			DrawStringToHandle(520 + 840 * loser, 725, alias[(int)inf.alias].c_str(), 0xffffff, fontAlias_->GetHandle());
			DrawFormatStringToHandle(520 + 840 * loser, 765, 0xffffff, fontAlias_->GetHandle(),
				"KILL %d", inf.kill);
			DrawFormatStringToHandle(520 + 840 * loser, 805, 0xffffff, fontAlias_->GetHandle(),
				"DEATH %d", inf.death);
			DrawGraph(390 + 840 * loser, 725, alias_[(int)inf.alias], true);
			loser++;
		}
	}


	DrawStringToHandle(60, 50, "TEAMDEATHMATCH", 0xffffff, fontMode_->GetHandle());
	DrawStringToHandle(600, 63, "-チームデスマッチ", 0xffffff, fontName_->GetHandle());


}
