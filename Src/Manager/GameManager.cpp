#include <algorithm>
#include "../Net/NetManager.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "ResourceManager.h"
#include "CollisionManager.h"
#include "../UI/Parts/Game/LogMessageArea/UI_LogMessageArea.h"
#include "../Data/Message/MessageTable.h"
#include "../Utility/Utility.h"
#include "../Object/Actor/Enemy/EnemyNeuvio.h"
#include "../Object/Actor/Enemy/EnemyGeorge.h"
#include "../Object/Actor/Enemy/EnemyCreator.h"
#include "../Object/Actor/Player/Player.h"
#include "GameManager.h"

GameManager* GameManager::instance_ = nullptr;

bool GameManager::IsStart(void)
{
	return timeToStart_ <= 0.0f;
}

bool GameManager::IsEnd(void)
{
	return gameTime_ <= 0.0f;
}

GameManager::GameManager(void) : netManager_(NetManager::GetInstance())
{
	//loadOut_.gunType_ = GUN_TYPE::AK47;

	dpi_ = 0.1f;

	// 保存用ホストIPアドレス
	hostIp_ = {};
	name_ = "-";

	isActive_ = true;

	killMessage_.Init(2.0f, 1.0f / 60.0f);
	std::weak_ptr<Font> s = std::dynamic_pointer_cast<Font>(ResourceManager::GetInstance().
		GetResourceFile("Font_GenShinGothic_Bold"));

	fontKill_ = std::make_unique<Font>(s.lock()->GetFontName(), 40);
	fontKill_->SetIsAutoDeleteHandle(true);
	fontDPI_ = std::make_unique<Font>(s.lock()->GetFontName(), 30);
	fontDPI_->SetIsAutoDeleteHandle(true);
	killAlpha_ = 0;

	imgHelp_ = LoadGraph("Data/Image/UI/Icon_Game_HUD_Help.png");
	imgHelpClose_ = LoadGraph("Data/Image/UI/Icon_Game_HUD_Help_close.png");
	helpActive_ = true;
}

GameManager::GameManager(const GameManager&) : netManager_(NetManager::GetInstance())
{

}

void GameManager::DrawDebug(void)
{
	DrawFormatString(900, 0, 0xffffff, "WOLF = %d", tdmScore_[(int)TEAM::WOLF]);
	DrawFormatString(900, GetFontSize(), 0xffffff, "EAGLE = %d", tdmScore_[(int)TEAM::EAGLE]);
	DrawFormatString(900, GetFontSize() * 2, 0xffffff, "TIME = %f", gameTime_);
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		DrawFormatString(900, GetFontSize() * 3 + GetFontSize() * i, 0xffffff, "Alias = %d", playerInfo_[i].alias);
	}
}

void GameManager::SetNormalLogMessage(const std::string& message, unsigned int color) {
	std::shared_ptr<UI_LogMessageArea::MESSAGE_DATA_BASE> messageDataBase
		= std::make_shared<UI_LogMessageArea::MESSAGE_DATA_NORMAL>(message, color);
	logMessageData_ = messageDataBase;
}

void GameManager::SetKillLogMessage(const std::string& by, unsigned int by_color, const std::string& killed, unsigned int killed_color) {
	std::shared_ptr<UI_LogMessageArea::MESSAGE_DATA_BASE> messageDataBase
		= std::make_shared<UI_LogMessageArea::MESSAGE_DATA_KILL>(by, by_color, killed, killed_color);
	logMessageData_ = messageDataBase;
}

const std::shared_ptr<UI_LogMessageArea::MESSAGE_DATA_BASE> GameManager::GetLogMessageData(void) {
	// メッセージデータをコピー
	auto ret_LogMessage = logMessageData_;

	logMessageData_ = nullptr;

	return ret_LogMessage;
}

void GameManager::DrawKillMessage(void)
{
	if (killMessage_.Update())return;


	auto msg = GetDieName() + "をキルした";


	SetDrawBlendMode(DX_BLENDMODE_ALPHA,(int)(killMessage_.cnt_ * 255.0f));
	int width = GetDrawStringWidthToHandle(msg.c_str(),
		strlen(msg.c_str()), fontKill_->GetHandle());

	DrawStringToHandle(960 - width / 2, 700, msg.c_str(), 0xff0000, fontKill_->GetHandle());
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

}

void GameManager::DrawHelpPessage(void)
{
	if (helpActive_) {
		DrawGraph(0, 0, imgHelp_, true);
		DrawFormatStringToHandle(1590, 685, 0xffffff, fontDPI_->GetHandle(), "マウス感度：%.3f",dpi_ * 10.0f);
	}
	else {
		DrawGraph(0, 0, imgHelpClose_, true);
	}
}

void GameManager::SetEnemyFollow(std::weak_ptr<ObjectBase> follow)
{
	enemyFollow_ = follow;
	auto player = std::dynamic_pointer_cast<Player>(enemyFollow_.lock());
	enemyCreator_->AddFollow(player->GetCenterPos());
}

bool GameManager::IsEndGame(void)
{
	return (timeToEnd_ >= 0.0f);
}

IPDATA GameManager::GetHostIP(void)
{
	return hostIp_;
}

void GameManager::SetHostIP(IPDATA ip)
{
	hostIp_ = ip;
}

std::string GameManager::GetName(void)
{
	return name_;
}

void GameManager::SetName(std::string name)
{
	name_ = name;
}

void GameManager::DrawEnemy(void)
{
	for (auto e : enemys_)
	{
		e->Draw();
	}
}

void GameManager::SetLoadOut(int no, LOADOUT loadOut)
{
	playerInfo_[no].loadOut = loadOut;
}

void GameManager::SetTeam(int no, TEAM team)
{
	playerInfo_[no].team = team;
}

void GameManager::UpdateEnemy()
{
	for (auto e : enemys_){
		e->Update();
	}

	if (mode_ == MODE::TIMEATTACK) {
		for (auto e : enemys_) {
			auto en = std::dynamic_pointer_cast<EnemyGeorge>(e);
			auto gun = en->GetGun();
			auto& bullets = gun->GetBullets();
			for (auto& bullet : bullets) {
				bullet->Update();
			}
		}
	}



}

bool GameManager::IsActive(void)
{
	return isActive_;
}

void GameManager::SetActive(bool isActive)
{
	isActive_ = isActive;
}

void GameManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new GameManager();
	}
	instance_->Init();
}

GameManager& GameManager::GetInstance(void)
{
	return *instance_;
}

void GameManager::Init(void)
{

	// シーンタイプ初期化
	state_ = STATE::NONE;

	// ゲームモード初期化
	mode_ = MODE::NONE;

	// 残り時間初期化
	gameTime_ = GAME_TIME_SCOREATTACK;

	// かかった時間初期化
	attackTime_ = 0.0f;

	// ゲーム開始までの時間初期化
	timeToStart_ = TIME_TO_START;

	// 称号複数持ち初期化
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		cnt_[i] = {};
	}
	// TEAMスコア
	tdmScore_[(int)TEAM::WOLF] = 0;
	tdmScore_[(int)TEAM::EAGLE] = 0;

	finishKill_ = 0;
}

void GameManager::Update(void)
{

	// シーンごとの処理
	switch (state_)
	{
	case GameManager::STATE::NONE:
		// 何もしない
		break;
	case GameManager::STATE::sLOBBY:
		sLobbyUpdate();
		break;
	case GameManager::STATE::sGAME:
		sGameUpdate();
		ChangeDPI();
		break;
	case GameManager::STATE::mLOBBY:
		mLobbyUpdate();
		NetUpdate();
		break;
	case GameManager::STATE::mGAME:
		mGameUpdate();
		ChangeDPI();
		SustainableGame();
		break;
	}
}

void GameManager::Destroy(void)
{
	DeleteGraph(imgHelp_);
	DeleteGraph(imgHelpClose_);
	delete instance_;
}

const float GameManager::GetDPI(void) const
{
	return dpi_;
}

void GameManager::SetDPI(float dpi)
{
	dpi_ = dpi;
}

const int GameManager::GetScore(int no)const
{
	return playerInfo_[no].score;
}

void GameManager::AddScore(unsigned int score, int no)
{
	if (IsEnd())return;
	playerInfo_[no].score += score;
}

void GameManager::AddBom(int no)
{
	playerInfo_[no].bom++;
}

void GameManager::AddHeadShot(int no)
{
	if (IsEnd())return;
	playerInfo_[no].headShot++;
}

void GameManager::AddDead(int no)
{
	if (IsEnd())return;
	playerInfo_[no].death++;
}

void GameManager::SetMagazine(int no, int magazine)
{
	playerInfo_[no].magazine = magazine;
}

int GameManager::GetMagazine(int no)
{
	return playerInfo_[no].magazine;
}

const float GameManager::GetGameTime(void) const
{
	return gameTime_;
}

const float GameManager::GetAttackTime(void) const
{
	return attackTime_;
}

const float GameManager::GetTimeToStart(void) const
{
	return timeToStart_;
}

const int GameManager::GetKill(int no)const
{
	return playerInfo_[no].kill;
}

const GRENADE_TYPE GameManager::GetGrenadeType(int no, int slot) const
{
	return playerInfo_[no].loadOut.greType_[slot];
}

int GameManager::GetGrenadeNum(int no) const
{
	return playerInfo_[no].greNum;
}

void GameManager::SetGrenadeNum(int no,int num)
{
	playerInfo_[no].greNum = num;
}

const std::string GameManager::GetPlayerName(int no) const
{
	return playerInfo_[no].playerName;
}

const std::string GameManager::GetName(ID id) const
{
	auto& messageTable = MessageTable::GetInstance();

	switch (id) {
	case ID::PLAYER_1:
	case ID::PLAYER_2:
	case ID::PLAYER_3:
	case ID::PLAYER_4:
		return playerInfo_[(int)id].playerName;
		break;
	case ID::ENEMY_NEUVIO_FOLLOW:
		return Utility::WStringToString(messageTable.GetMessage_(28));
		break;
	case ID::ENEMY_NEUVIO_FIXED:
		return Utility::WStringToString(messageTable.GetMessage_(29));
		break;
	case ID::ENEMY_NEUVIO_THROUGH:
		return Utility::WStringToString(messageTable.GetMessage_(30));
		break;
	case ID::ENEMY_GEORGE:
		return Utility::WStringToString(messageTable.GetMessage_(32));
		break;
	default:
		return "";
		break;
	}
	//if (id > ID::PLAYER_4)return "";
	//return playerInfo_[(int)id].playerName;
}

const GameManager::TEAM GameManager::GetTeam(int no) const
{
	return playerInfo_[no].team;
}

const bool GameManager::IsHide(int no) const
{
	return playerInfo_[no].isHide;
}

void GameManager::SetIsHide(int no, bool isHide)
{
	playerInfo_[no].isHide = isHide;
}

void GameManager::Kill(ID die, ID killed)
{
	if (killed == ID::PLAYER_1 ||
		killed == ID::PLAYER_2 ||
		killed == ID::PLAYER_3 ||
		killed == ID::PLAYER_4) {

		auto killedTeam = playerInfo_[(int)killed].team;
		auto dieTeam = playerInfo_[(int)die].team;
		if (die != killed && killedTeam != dieTeam) {
			playerInfo_[(int)killed].kill++;

			tdmScore_[(int)playerInfo_[(int)killed].team]++;
		}

		int no = (int)netManager_.GetSelf().playerType;
		if ((ID)no == killed) {
			dieID_ = die;
			killMessage_.Set();
		}
	}

	if (die == ID::PLAYER_1 ||
		die == ID::PLAYER_2 ||
		die == ID::PLAYER_3 ||
		die == ID::PLAYER_4) {
		playerInfo_[(int)die].death++;

		int no = (int)netManager_.GetSelf().playerType;
		if ((ID)no == die) {
			killerID_ = killed;
		}

	}

	switch (killed)
	{
	case ID::PLAYER_1:
		finishKill_ = 0;
		break;
	case ID::PLAYER_2:
		finishKill_ = 1;
		break;
	case ID::PLAYER_3:
		finishKill_ = 2;
		break;
	case ID::PLAYER_4:
		finishKill_ = 3;
		break;
	}
	unsigned int dieColor;
	unsigned int killedColor;
	if (mode_ == MODE::TIMEATTACK || 
		mode_ == MODE::SCOREATTACK){
		dieColor = (die == ID::PLAYER_1) ? 0x00FFFF : 0xFF0000;
		killedColor = (killed == ID::PLAYER_1) ? 0x00FFFF : 0xFF0000;
	}
	else {
		int selfTeam = (int)netManager_.GetSelf().team;
		int dieTeam = (int)playerInfo_[(int)die].team;
		int killedTeam = (int)playerInfo_[(int)killed].team;
		dieColor = (dieTeam == selfTeam) ? 0x00FFFF : 0xFF0000;
		killedColor = (killedTeam == selfTeam) ? 0x00FFFF : 0xFF0000;
	}

	SetKillLogMessage(GetName(die), dieColor, GetName(killed), killedColor);
}

std::string GameManager::GetKillerName(void)
{
	auto& messageTable = MessageTable::GetInstance();

	switch (killerID_) {
	case ID::PLAYER_1:
	case ID::PLAYER_2:
	case ID::PLAYER_3:
	case ID::PLAYER_4:
		return playerInfo_[(int)killerID_].playerName;
		break;
	case ID::ENEMY_NEUVIO_FOLLOW:
		return Utility::WStringToString(messageTable.GetMessage_(28));
		break;
	case ID::ENEMY_NEUVIO_FIXED:
		return Utility::WStringToString(messageTable.GetMessage_(29));
		break;
	case ID::ENEMY_NEUVIO_THROUGH:
		return Utility::WStringToString(messageTable.GetMessage_(30));
		break;
	case ID::ENEMY_GEORGE:
		return Utility::WStringToString(messageTable.GetMessage_(32));
		break;
	default:
		return "";
		break;
	}
}

std::string GameManager::GetDieName(void)
{
	auto& messageTable = MessageTable::GetInstance();

	switch (dieID_) {
	case ID::PLAYER_1:
	case ID::PLAYER_2:
	case ID::PLAYER_3:
	case ID::PLAYER_4:
		return playerInfo_[(int)dieID_].playerName;
		break;
	case ID::ENEMY_NEUVIO_FOLLOW:
		return Utility::WStringToString(messageTable.GetMessage_(28));
		break;
	case ID::ENEMY_NEUVIO_FIXED:
		return Utility::WStringToString(messageTable.GetMessage_(29));
		break;
	case ID::ENEMY_NEUVIO_THROUGH:
		return Utility::WStringToString(messageTable.GetMessage_(30));
		break;
	case ID::ENEMY_GEORGE:
		return Utility::WStringToString(messageTable.GetMessage_(32));
		break;
	default:
		return "";
		break;
	}
}

int GameManager::GetEliminationNum(ID id)
{
	return eliminationNum_[(int)id];
}

void GameManager::AddElimination(ID id)
{
	if (IsEnd())return;
	eliminationNum_[(int)id]++;
}

void GameManager::NetUpdate(void)
{
	auto& players = netManager_.GetNetUsers();
	auto& self = netManager_.GetSelf();


	for (auto& player : players) {
		if (player.second.key == self.key) {
			continue;
		}

		int i = (int)player.second.playerType;
		playerInfo_[i].team = (TEAM)player.second.team;
		playerInfo_[i].loadOut.gunType_ = (GUN_TYPE)player.second.gun;
		playerInfo_[i].loadOut.greType_[0] = (GRENADE_TYPE)player.second.grenade[0];
		playerInfo_[i].loadOut.greType_[1] = (GRENADE_TYPE)player.second.grenade[1];

	}
}

GameManager::STATE GameManager::GetState(void)
{
	return state_;
}

int GameManager::GetDead(int no)
{
	return playerInfo_[no].death;
}

void GameManager::ChangeState(STATE state)
{
	state_ = state;

	// 初期化
	switch (state_)
	{
	case GameManager::STATE::NONE:
		break;
	case GameManager::STATE::sLOBBY:
		sLobbyInit();
		break;
	case GameManager::STATE::mLOBBY:
		mLobbyInit();
		break;
	case GameManager::STATE::sGAME:
		sGameInit();
		break;
	case GameManager::STATE::mGAME:
		mGameInit();
		break;
	default:
		break;
	}
}

GameManager::MODE GameManager::GetMode(void)
{
	return mode_;
}

void GameManager::ChangeMode(MODE mode)
{
	mode_ = mode;
}

void GameManager::SetName(int no, std::string name)
{

	playerInfo_[no].playerName = name;
}

LOADOUT GameManager::GetLoadOut(int no)
{
	return playerInfo_[no].loadOut;
}

GameManager::PLAYER_INFO& GameManager::GetPlayerInfo(int no)
{
	return playerInfo_[no];
}

const GRENADE_TYPE GameManager::GetNowGreType(int no) const
{
	return playerInfo_[no].greType;
}

void GameManager::ChangePlayerInfo(PLAYER_INFO info, int no)
{
	playerInfo_[no] = info;
}

void GameManager::SetGreType(int no, GRENADE_TYPE type)
{
	playerInfo_[no].greType = type;
}

const GameManager::TEAM& GameManager::GetWinner(void) const
{
	return winTeam_;
}

const bool GameManager::WinPlayer(void) const
{
	auto& self = netManager_.GetSelf().team;
	return ((TEAM)self == winTeam_);
}

const int& GameManager::GetTDMScore(TEAM team) const
{
	return tdmScore_[(int)team];
}

int GameManager::GetGoalTDMScore(void)const
{
	return goalScore_;
}

void GameManager::sLobbyInit(void)
{
	playerInfo_[(int)ID::PLAYER_1].Reset();
}

void GameManager::mLobbyInit(void)
{
	for (auto& info : playerInfo_) {
		info.Reset();
	}
	if(SceneManager::GetInstance().GetPreSceneID() == SceneManager::SCENE_ID::MENU)
	{
		auto& info = playerInfo_[0];
		netManager_.SetTeam((int)info.team);
		netManager_.SetLoadOut((int)info.loadOut.gunType_,
			(int)info.loadOut.greType_[0], (int)info.loadOut.greType_[1]);
	}
	else {
		auto& self = netManager_.GetSelf();
		auto& info = playerInfo_[(int)self.playerType];
		netManager_.SetTeam((int)info.team);
		netManager_.SetLoadOut((int)info.loadOut.gunType_,
			(int)info.loadOut.greType_[0], (int)info.loadOut.greType_[1]);
	}
}

void GameManager::ResetPlayerInfo(int no)
{
	playerInfo_[no].loadOut = { GUN_TYPE::AK47,GRENADE_TYPE::HAND,GRENADE_TYPE::SMOKE };
	playerInfo_[no].kill = 0;
	playerInfo_[no].death = 0;
	playerInfo_[no].bom = 0;
	playerInfo_[no].headShot = 0;
	playerInfo_[no].score = 0;
	playerInfo_[no].magazine = 0;
	playerInfo_[no].team = TEAM::WOLF;
	playerInfo_[no].greType = GRENADE_TYPE::HAND;
	playerInfo_[no].greNum = 0;
	playerInfo_[no].moveControlPosZ = 0.0f;
	playerInfo_[no].actionControlPosZ = 0.0f;
	playerInfo_[no].playerName = "プレイヤー";
	playerInfo_[no].alias = ALIAS::NONE;
	playerInfo_[no].respawnPos = {};
	playerInfo_[no].respawnRot = {};
	playerInfo_[no].isHide = false;
}

void GameManager::sGameInit(void)
{
	isActive_ = true;
	attackTime_ = 0.0f;
	timeToStart_ = TIME_TO_START;
	timeToEnd_ = TIME_TO_END;
	playerInfo_[(int)ID::PLAYER_1].GameReset();

	playerInfo_[(int)ID::PLAYER_1].respawnPos = SINGLE_RESPAWN_POS;
	playerInfo_[(int)ID::PLAYER_1].respawnRot = SINGLE_RESPAWN_ROT;

	killMessage_.cnt_ = 0.0f;

	// TEAMスコア
	tdmScore_[(int)TEAM::WOLF] = 0;
	tdmScore_[(int)TEAM::EAGLE] = 0;

	// 撃破数
	for (auto& num : eliminationNum_) {
		num = 0;
	}

	// エネミー生成
	enemyCreator_ = nullptr;
	enemyCreator_ = std::make_shared<EnemyCreator>(enemys_);
	if (mode_ == MODE::SCOREATTACK) {
		gameTime_ = GAME_TIME_SCOREATTACK;
		enemyCreator_->Init(EnemyCreator::ENEMY_ID::NEUVIO, 10, 5.0f, 10);
	}else {
		gameTime_ = 0.0f;
		enemyCreator_->Init(EnemyCreator::ENEMY_ID::GEORGE, 1, 300.0f, 1);
	}
}

void GameManager::mGameInit(void)
{
	damages_.clear();
	isActive_ = true;
	gameTime_ = GAME_TIME_TDM;
	attackTime_ = 0.0f;
	timeToStart_ = TIME_TO_START;
	timeToEnd_ = TIME_TO_END;

	for (auto& info : playerInfo_) {
		info.GameReset();
	}

	killMessage_.cnt_ = 0.0f;

	int num = 0;
	int wolf = 0;
	int eagle = 0;

	auto& players = netManager_.GetNetUsers();
	for (auto& player : players) {

		auto& info = playerInfo_[(int)player.second.playerType];
		info.team = (TEAM)player.second.team;
		info.loadOut.gunType_ = (GUN_TYPE)player.second.gun;
		info.loadOut.greType_[0] = (GRENADE_TYPE)player.second.grenade[0];
		info.loadOut.greType_[1] = (GRENADE_TYPE)player.second.grenade[1];

		switch (info.team)
		{
		case TEAM::WOLF:
			info.respawnPos = MULTI_RESPAWN_POS_WOLF[wolf];
			info.respawnRot = MULTI_RESPAWN_ROT_WOLF[wolf];
			info.moveControlPosZ = WOLF_CONTROL_POS_Z;
			info.actionControlPosZ = EAGLE_CONTROL_POS_Z;
			wolf++;
			break;
		case TEAM::EAGLE:
			info.respawnPos = MULTI_RESPAWN_POS_EAGLE[eagle];
			info.respawnRot = MULTI_RESPAWN_ROT_EAGLE[eagle];
			info.moveControlPosZ = EAGLE_CONTROL_POS_Z;
			info.actionControlPosZ = WOLF_CONTROL_POS_Z;
			eagle++;
			break;
		}

		num++;
	}


	// TDM目標スコア
	if (num - 2 < 0)num = 2;
	goalScore_ = TDM_GOAL_SCORE[num - 2];

	// TEAMスコア
	tdmScore_[(int)TEAM::WOLF] = 0;
	tdmScore_[(int)TEAM::EAGLE] = 0;

	// 撃破数
	for (auto& num : eliminationNum_) {
		num = 0;
	}
}

//void GameManager::Alias(void)
//{
//
//	int j = 0;
//	int idx[INFO_MAX] = {};
//	int preIdx[INFO_MAX] = {};
//	PLAYER_INFO info;
//	PLAYER_INFO preInfo;
//	info.kill = preInfo.kill = playerInfo_[0].kill;
//	info.death = preInfo.death = playerInfo_[0].death;
//	info.bom = preInfo.bom = playerInfo_[0].bom;
//	info.score = preInfo.score = playerInfo_[0].score;
//	info.headShot = preInfo.headShot = playerInfo_[0].headShot;
//
//	while (j < INFO_MAX)
//	{
//		for (int i = 1; i < MAX_PLAYER; i++)
//		{
//
//			j = 0;
//			// キル
//			if (info.kill < playerInfo_[i].kill)
//			{
//				preInfo.kill = info.kill;
//				info.kill = playerInfo_[i].kill;
//				preIdx[j] = idx[j];
//				idx[j] = i;
//			}
//			else
//			{
//				if (preInfo.kill < playerInfo_[i].kill)
//				{
//					preInfo.kill = playerInfo_[i].kill;
//					preIdx[j] = i;
//				}
//			}
//
//			j++;
//			// 死亡
//			if (info.death < playerInfo_[i].death)
//			{
//				preInfo.death = info.death;
//				info.death = playerInfo_[i].death;
//				preIdx[j] = idx[j];
//				idx[j] = i;
//			}
//			else
//			{
//				if (preInfo.death < playerInfo_[i].death)
//				{
//					preInfo.death = playerInfo_[i].death;
//					preIdx[j] = i;
//				}
//			}
//
//			j++;
//			// ボム
//			if (info.bom < playerInfo_[i].bom)
//			{
//				preInfo.bom = info.bom;
//				info.bom = playerInfo_[i].bom;
//				preIdx[j] = idx[j];
//				idx[j] = i;
//			}
//			else
//			{
//				if (preInfo.bom < playerInfo_[i].bom)
//				{
//					preInfo.bom = playerInfo_[i].bom;
//					preIdx[j] = i;
//				}
//			}
//
//			j++;
//			// スコア
//			if (info.score < playerInfo_[i].score)
//			{
//				preInfo.score = info.score;
//				info.score = playerInfo_[i].score;
//				preIdx[j] = idx[j];
//				idx[j] = i;
//			}
//			else
//			{
//				if (preInfo.score < playerInfo_[i].score)
//				{
//					preInfo.score = playerInfo_[i].score;
//					preIdx[j] = i;
//				}
//			}
//
//			j++;
//			// ヘッドショット
//			if (info.headShot < playerInfo_[i].headShot)
//			{
//				preInfo.headShot = info.headShot;
//				info.headShot = playerInfo_[i].headShot;
//				preIdx[j] = idx[j];
//				idx[j] = i;
//			}
//			else
//			{
//				if (preInfo.headShot < playerInfo_[i].headShot)
//				{
//					preInfo.headShot = playerInfo_[i].headShot;
//					preIdx[j] = i;
//				}
//			}
//
//		}
//		j = INFO_MAX;
//	}
//
//	for (int n = 0; n < INFO_MAX; n++)
//	{
//
//		int index = idx[n];
//		switch (index)
//		{
//		case 0:
//			SetAlias(index, n);
//			break;
//		case 1:
//			SetAlias(index, n);
//			break;
//		case 2:
//			SetAlias(index, n);
//			break;
//		case 3:
//			SetAlias(index, n);
//			break;
//		}
//
//	}
//
//}

void GameManager::SetAlias(int idx, int no)
{


	if (playerInfo_[0].alias == ALIAS::NONE &&
		playerInfo_[1].alias == ALIAS::NONE &&
		playerInfo_[2].alias == ALIAS::NONE &&
		playerInfo_[3].alias == ALIAS::NONE
		)
	{
		idx_ = idx;
		//playerInfo_[idx_].alias = ALIAS::MVP;
	}

	cnt_[idx]++;

	if (playerInfo_[idx].alias == ALIAS::NONE && no == 0)
	{
		playerInfo_[idx].alias = ALIAS::KILLER;
	}
	else if (playerInfo_[idx].alias == ALIAS::NONE && no == 4)
	{
		playerInfo_[idx].alias = ALIAS::HITMAN;
	}
	else if (playerInfo_[finishKill_].alias == ALIAS::NONE)
	{
		playerInfo_[finishKill_].alias = ALIAS::LASTKILL;
	}
	else if (playerInfo_[idx].alias == ALIAS::NONE && no == 2)
	{
		playerInfo_[idx].alias = ALIAS::BOMMER;
	}
	else if (playerInfo_[idx].alias == ALIAS::NONE && no == 1)
	{
		playerInfo_[idx].alias = ALIAS::UNDEAD;
	}
	//else if (playerInfo_[idx].alias == ALIAS::NONE && no == 3)
	//{
	//	playerInfo_[idx].alias = ALIAS::SCORER;
	//}

}

void GameManager::SetMVP(void)
{
	ALIAS preAlias[MAX_PLAYER];
	for (int idx = 1; idx < MAX_PLAYER; idx++)
	{
		idx_ = idx;
		idx_--;
		if (playerInfo_[idx_].alias == ALIAS::NONE && idx_ == 0 && cnt_[idx_] > 1)
		{
			playerInfo_[idx_].alias = ALIAS::MVP;
		}
		if (cnt_[idx_] < cnt_[idx])
		{
			preAlias[idx_] = playerInfo_[idx_].alias;
			playerInfo_[idx_].alias = ALIAS::NONE;
			idx_++;
			playerInfo_[idx_].alias = ALIAS::MVP;
		}
	}

	for (int i = 0; i < MAX_PLAYER; i++)
	{
		if (playerInfo_[i].alias == ALIAS::NONE)
		{
			playerInfo_[i].alias = preAlias[i];
		}
	}
}

void GameManager::Alias(void)
{
	//NONE,
	//COMEDIAN
	//KILLER,
	//LASTKILL,
	//BOMMER,
	//UNDEAD,
	//HITMAN
	//MVP

	float max = 0.0f;
	int no = 0;

	bool isAllZero = true;

	auto& players = netManager_.GetNetUsers();

	// COMEDIAN
	for (auto& player : players) {
		int i = (int)player.second.playerType;
		playerInfo_[i].alias = ALIAS::COMEDIAN;
		if (playerInfo_[i].kill != 0)isAllZero = false;
	}

	if (isAllZero)return;

	// MVP
	if (playerInfo_[0].death == 0) {
		max = (float)playerInfo_[0].kill;
	}
	else {
		max = (float)playerInfo_[0].kill / (float)playerInfo_[0].death;
	}
	for (auto& player : players) {
		int i = (int)player.second.playerType;
		float rate = 0.0f;
		if (playerInfo_[i].death == 0) {
			rate = (float)playerInfo_[i].kill;
		}
		else {
			rate = (float)playerInfo_[i].kill / (float)playerInfo_[i].death;
		}
		if (max > rate)continue;
		max = rate;
		no = i;
	}
	playerInfo_[no].alias = ALIAS::MVP;

	// KILLER
	no = 0;
	max = playerInfo_[0].kill;
	for (auto& player : players) {
		int i = (int)player.second.playerType;
		float kill = playerInfo_[i].kill;
		if (max > kill)continue;
		max = kill;
		no = i;
	}
	if (playerInfo_[no].alias == ALIAS::COMEDIAN) {
		playerInfo_[no].alias = ALIAS::KILLER;
	}

	// BOMMER
	no = 0;
	max = playerInfo_[0].bom;
	for (auto& player : players) {
		int i = (int)player.second.playerType;
		float bom = playerInfo_[i].bom;
		if (max > bom)continue;
		max = bom;
		no = i;
	}
	if (playerInfo_[no].alias == ALIAS::COMEDIAN) {
		playerInfo_[no].alias = ALIAS::BOMMER;
	}

	// UNDEAD
	no = 0;
	max = playerInfo_[0].death;
	for (auto& player : players) {
		int i = (int)player.second.playerType;
		float death = playerInfo_[i].death;
		if (max > death)continue;
		max = death;
		no = i;
	}
	if (playerInfo_[no].alias == ALIAS::COMEDIAN) {
		playerInfo_[no].alias = ALIAS::UNDEAD;
	}

	// HITMAN
	no = 0;
	max = playerInfo_[0].headShot;
	for (auto& player : players) {
		int i = (int)player.second.playerType;
		float head = playerInfo_[i].headShot;
		if (max > head)continue;
		max = head;
		no = i;
	}
	if (playerInfo_[no].alias == ALIAS::COMEDIAN) {
		playerInfo_[no].alias = ALIAS::HITMAN;
	}
	
	// LASTKILL
	if (playerInfo_[finishKill_].alias == ALIAS::COMEDIAN) {
		playerInfo_[finishKill_].alias = ALIAS::LASTKILL;
	}
}

GameManager::~GameManager(void)
{

}

void GameManager::ChangeDPI(void)
{
	auto& input = InputManager::GetInstance(); 
	if(input.IsTrgDown(KEY_INPUT_UP)) {
		dpi_ += 0.0125f;
		dpi_ = std::clamp(dpi_, 0.025f, 0.15f);
	}
	else if(input.IsTrgDown(KEY_INPUT_DOWN)) {
		dpi_ -= 0.0125f;
		dpi_ = std::clamp(dpi_, 0.025f, 0.15f);
	}
}

void GameManager::sLobbyUpdate(void)
{

}

void GameManager::mLobbyUpdate(void)
{
	auto& input = InputManager::GetInstance();
	if (input.IsTrgDown(KEY_INPUT_S))
	{
		auto& self = netManager_.GetSelf();
		auto& info = playerInfo_[(int)self.playerType];
		info.team = (TEAM)((int)info.team + 1);
		if (info.team >= TEAM::MAX)
		{
			info.team = TEAM::WOLF;
		}

		netManager_.SetTeam((int)info.team);
	}
}

void GameManager::sGameUpdate(void)
{
	if (!isActive_) return;

	auto& ins = SceneManager::GetInstance();

	if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_H)) {
		helpActive_ = !helpActive_;
	}

	switch (mode_)
	{
	case GameManager::MODE::NONE:
		break;
	case GameManager::MODE::TIMEATTACK:
		// ゲーム開始までの時間
		timeToStart_ -= ins.GetDeltaTime();

		// エネミー生成
		enemyCreator_->Update();


		// ゲーム開始まで残り時間を減らさない
		if (timeToStart_ >= 0.0f) return;

		// かかった時間
		attackTime_ += ins.GetDeltaTime();

		// 残り時間
		{
			float preGameTime = gameTime_;
			gameTime_ += ins.GetDeltaTime();

			// キル数が一定に達したらリザルトに移行
			if (playerInfo_[(int)ID::PLAYER_1].score > 0)
			{
				gameTime_ = preGameTime;

				// ゲーム終了までの時間
				timeToEnd_ -= ins.GetDeltaTime();
				if (timeToEnd_ >= 0.0f) return;

				ins.ChangeScene(SceneManager::SCENE_ID::sRESULT);
				//state_ = STATE::NONE;
			}
		}
		break;
	case GameManager::MODE::SCOREATTACK:
		// ゲーム開始までの時間
		timeToStart_ -= ins.GetDeltaTime();

		// エネミー生成
		enemyCreator_->Update();

		// ゲーム開始まで残り時間を減らさない
		if (timeToStart_ >= 0.0f) return;

		// 残り時間
		gameTime_ -= ins.GetDeltaTime();

		// 残り時間が0になったらリザルトに移行
		if (gameTime_ <= 0.0f)
		{
			gameTime_ = 0.0f;

			// ゲーム終了までの時間
			timeToEnd_ -= ins.GetDeltaTime();
			if (timeToEnd_ >= 0.0f) return;

			ins.ChangeScene(SceneManager::SCENE_ID::sRESULT);
			//state_ = STATE::NONE;
		}
		break;
	case GameManager::MODE::HOST:
		break;
	case GameManager::MODE::CLIENT:
		break;
	case GameManager::MODE::SETTINGS:
		break;
	}

}

void GameManager::mGameUpdate(void)
{
	// ゲーム開始までの時間
	auto& ins = SceneManager::GetInstance();
	timeToStart_ -= ins.GetDeltaTime();

	// ゲーム開始まで残り時間を減らさない
	if (timeToStart_ >= 0.0f) return;

	if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_H)) {
		helpActive_ = !helpActive_;
	}

	// 残り時間
	gameTime_ -= ins.GetDeltaTime();

	// 残り時間が0になったらリザルトに移行
	if (gameTime_ <= 0.0f ||
		tdmScore_[(int)TEAM::WOLF] >= goalScore_ || tdmScore_[(int)TEAM::EAGLE] >= goalScore_)
	{
		state_ = STATE::NONE;

		NetManager::GetInstance().ChangeGameState(GAME_STATE::RESULT);
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::mRESULT);

		// 勝者
		if (tdmScore_[(int)TEAM::WOLF] > tdmScore_[(int)TEAM::EAGLE]) winTeam_ = TEAM::WOLF;
		else if (tdmScore_[(int)TEAM::WOLF] < tdmScore_[(int)TEAM::EAGLE])winTeam_ = TEAM::EAGLE;
		else winTeam_ = TEAM::MAX;

		Alias();
	}

}

void GameManager::ReleseEnemy(void)
{
	enemyCreator_->Relese();
	for (auto e : enemys_) {
		e->Release();
	}
	enemys_.clear();
}

void GameManager::Back(void)
{
	auto& sMng = SceneManager::GetInstance();
	auto scene = sMng.GetSceneID();

	switch (scene)
	{
	case SceneManager::SCENE_ID::MENU:
		sMng.ChangeScene(SceneManager::SCENE_ID::TITLE);
		break;
	case SceneManager::SCENE_ID::sLOBBY:
		sMng.ChangeScene(SceneManager::SCENE_ID::MENU);
		break;
	case SceneManager::SCENE_ID::sGAME:
		sMng.ChangeScene(SceneManager::SCENE_ID::sLOBBY);
		break;
	case SceneManager::SCENE_ID::sRESULT:
		sMng.ChangeScene(SceneManager::SCENE_ID::sLOBBY);
		break;
	case SceneManager::SCENE_ID::mLOBBY:
		sMng.ChangeScene(SceneManager::SCENE_ID::MENU);
		netManager_.Stop();
		break;
	case SceneManager::SCENE_ID::mGAME:
		sMng.ChangeScene(SceneManager::SCENE_ID::MENU);
		netManager_.Stop();
		break;
	case SceneManager::SCENE_ID::mRESULT:
		sMng.ChangeScene(SceneManager::SCENE_ID::mLOBBY);
		netManager_.ChangeGameState(GAME_STATE::LOBBY);
		break;
	}
}

void GameManager::AddDamageData(DamageData data)
{
	damages_.emplace_back(data);
}

std::vector<GameManager::DamageData>& GameManager::GetDamageData(void)
{
	return damages_;
}

bool GameManager::IsDivideTeam(void)
{
	bool ret = false;

	int wolf = 0;
	int eagle = 0;

	auto& players = netManager_.GetNetUsers();
	for (auto& player : players) {
		(TEAM)player.second.team == TEAM::WOLF ? wolf++ : eagle++;
	}

	ret = (wolf == eagle || abs(wolf - eagle) == 1);
	ret = !(wolf == 0 || eagle == 0);

	return ret;
}

void GameManager::SustainableGame(void)
{
	if (!IsDivideTeam())
	{
		// ゲーム続行不可
		// クライアントの場合メニューに戻ります
		// ホストの場合ロビーに戻ります
		if (netManager_.IsHost()) {
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::mLOBBY);
			netManager_.ChangeGameState(GAME_STATE::LOBBY);
			ChangeState(STATE::mLOBBY);
		}
		else {
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::MENU);
			netManager_.Stop();
			ChangeState(STATE::NONE);
		}
	}
}
