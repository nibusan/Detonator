#include "../Application.h"
#include "../Manager/SceneManager.h"
#include "../Manager/GameManager.h"
#include "../Manager/AudioManager.h"
#include "../Data/Message/MessageTable.h"
#include "../Utility/Utility.h"
#include "../Net/NetManager.h"
#include "UILayer/UILayer_InputIPAddress.h"
#include "UILayer/UILayer_InputName.h"
#include "UIScreen/UIScreen.h"
#include "UIBase.h"
#include "Parts/Common/InputTextArea/UI_InputTextArea.h"
#include "../Object/ObjectID.h"
#include "UIFunctionList.h"

void UIFunctionList::Init(void) {
	// 各コールバック関数の登録
	RegisterOnClickFunction();
	RegisterEndInputFunction();
	RegisterGetAddTextFunction();
	RegisterGetLogDataFunction();
}

const std::function<void(void)>& UIFunctionList::GetOnClickFunction(int id) const {
    return onClickFunctionList_.at(id);
}

const std::function<void(std::weak_ptr<UIBase>)>& UIFunctionList::GetEndInputFunction(int id) const {
	return endInputFunctionList_.at(id);
}

const std::function<std::string(void)>& UIFunctionList::GetAddTextFunction(int id) const {
	return getAddTextFunctionList_.at(id);
}

const std::function<std::shared_ptr<UI_LogMessageArea::MESSAGE_DATA_BASE>(void)>& UIFunctionList::GetLogDataFunction(int id) const {
	return getLogDataFunctionList_.at(id);
}

void UIFunctionList::RegisterOnClickFunction(void) {
	onClickFunctionList_[0] = [&](void)->void {
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::sLOBBY);
		GameManager::GetInstance().ChangeState(GameManager::STATE::sLOBBY);
		GameManager::GetInstance().ChangeMode(GameManager::MODE::TIMEATTACK);
		AudioManager::GetInstance().Play(AudioList::AUDIO_TYPE::SYSTEM_CLICKBUTTON, false, 1.0f, 44100);
	};

	onClickFunctionList_[1] = [&](void)->void {
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::sLOBBY);
		GameManager::GetInstance().ChangeState(GameManager::STATE::sLOBBY);
		GameManager::GetInstance().ChangeMode(GameManager::MODE::SCOREATTACK);
		AudioManager::GetInstance().Play(AudioList::AUDIO_TYPE::SYSTEM_CLICKBUTTON, false, 1.0f, 44100);
	};

	onClickFunctionList_[2] = [&](void)->void {
		std::unique_ptr<UILayerBase> uiLayer = std::make_unique<UILayer_InputName>();
		uiLayer->Init();
		auto uiScreen = SceneManager::GetInstance().GetUIScreen();
		uiScreen.lock()->SetIsActive_FrontUILayer(false);
		uiScreen.lock()->PushUILayer(std::move(uiLayer));
	};

	onClickFunctionList_[3] = [](void)->void {
		std::unique_ptr<UILayerBase> uiLayer = std::make_unique<UILayer_InputIPAddress>();
		uiLayer->Init();
		auto uiScreen = SceneManager::GetInstance().GetUIScreen();
		uiScreen.lock()->SetIsActive_FrontUILayer(false);
		uiScreen.lock()->PushUILayer(std::move(uiLayer));
	};

	onClickFunctionList_[4] = [&](void)->void {
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::sLOBBY);
		AudioManager::GetInstance().Play(AudioList::AUDIO_TYPE::SYSTEM_CLICKBUTTON, false, 1.0f, 44100);
	};

	onClickFunctionList_[5] = [&](void)->void {
		Application::GetInstance().Shutdown();
	};

	onClickFunctionList_[6] = [&](void)->void {
		auto uiScreen = SceneManager::GetInstance().GetUIScreen();
		uiScreen.lock()->PopUILayer();
	};

	onClickFunctionList_[7] = [&](void)->void {
		NetManager::GetInstance().Run(NET_MODE::CLIENT);
		NetManager::GetInstance().SetPlayerName(GameManager::GetInstance().GetName());
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::mLOBBY);
		GameManager::GetInstance().ChangeMode(GameManager::MODE::CLIENT);
		AudioManager::GetInstance().Play(AudioList::AUDIO_TYPE::SYSTEM_CLICKBUTTON, false, 1.0f, 44100);
	};

	onClickFunctionList_[8] = [&](void)->void {
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::sGAME);
		AudioManager::GetInstance().Play(AudioList::AUDIO_TYPE::SYSTEM_CHANGESCENE_SINGLE, false, 1.0f, 44100);
	};

	onClickFunctionList_[9] = [&](void)->void {
		auto& playerInfo = GameManager::GetInstance().GetPlayerInfo(static_cast<int>(ID::PLAYER_1));
		playerInfo.loadOut.gunType_ = (GUN_TYPE)((int)playerInfo.loadOut.gunType_ + 1);
		if (playerInfo.loadOut.gunType_ >= GUN_TYPE::MAX)
		{
			playerInfo.loadOut.gunType_ = GUN_TYPE::AK47;
		}
		AudioManager::GetInstance().Play(AudioList::AUDIO_TYPE::SYSTEM_WEAPON_SELECT, false, 0.8f, 44100);
	};

	onClickFunctionList_[10] = [&](void)->void {
		auto& playerInfo = GameManager::GetInstance().GetPlayerInfo(static_cast<int>(ID::PLAYER_1));
		playerInfo.loadOut.gunType_ = (GUN_TYPE)((int)playerInfo.loadOut.gunType_ - 1);
		if (playerInfo.loadOut.gunType_ < GUN_TYPE::AK47)
		{
			playerInfo.loadOut.gunType_ = GUN_TYPE::M24R;
		}
		AudioManager::GetInstance().Play(AudioList::AUDIO_TYPE::SYSTEM_WEAPON_SELECT, false, 0.8f, 44100);
	};

	onClickFunctionList_[11] = [&](void)->void {
		auto& gameManager = GameManager::GetInstance();
		auto& playerInfo = gameManager.GetPlayerInfo(static_cast<int>(ID::PLAYER_1));
		auto nextGrenadeType = gameManager.GetGrenadeType(static_cast<int>(ID::PLAYER_1), 1);
		playerInfo.loadOut.greType_[0] = (GRENADE_TYPE)((int)playerInfo.loadOut.greType_[0] + 1);
		if (nextGrenadeType == playerInfo.loadOut.greType_[0]) {
			//playerInfo.loadOut.greType_[0] = (GRENADE_TYPE)(((int)(playerInfo.loadOut.greType_[0]) + 2));
		}
		if (playerInfo.loadOut.greType_[0] >= GRENADE_TYPE::MAX)
		{
			playerInfo.loadOut.greType_[0] = (GRENADE_TYPE)((int)(playerInfo.loadOut.greType_[0]) - (int)(GRENADE_TYPE::MAX));
		}
		AudioManager::GetInstance().Play(AudioList::AUDIO_TYPE::SYSTEM_WEAPON_SELECT, false, 0.8f, 44100);
	};

	onClickFunctionList_[12] = [&](void)->void {
		auto& gameManager = GameManager::GetInstance();
		auto& playerInfo = gameManager.GetPlayerInfo(static_cast<int>(ID::PLAYER_1));
		auto nextGrenadeType = gameManager.GetGrenadeType(static_cast<int>(ID::PLAYER_1), 1);
		playerInfo.loadOut.greType_[0] = (GRENADE_TYPE)((int)playerInfo.loadOut.greType_[0] - 1);
		if (nextGrenadeType == playerInfo.loadOut.greType_[0]) {
			//playerInfo.loadOut.greType_[0] = (GRENADE_TYPE)(((int)(playerInfo.loadOut.greType_[0]) - 2));
		}
		if (playerInfo.loadOut.greType_[0] < GRENADE_TYPE::HAND)
		{
			playerInfo.loadOut.greType_[0] = (GRENADE_TYPE)((int)(playerInfo.loadOut.greType_[0]) + (int)(GRENADE_TYPE::MAX));
		}
		AudioManager::GetInstance().Play(AudioList::AUDIO_TYPE::SYSTEM_WEAPON_SELECT, false, 0.8f, 44100);
	};

	onClickFunctionList_[13] = [&](void)->void {
		auto& gameManager = GameManager::GetInstance();
		auto& playerInfo = gameManager.GetPlayerInfo(static_cast<int>(ID::PLAYER_1));
		auto nextGrenadeType = gameManager.GetGrenadeType(static_cast<int>(ID::PLAYER_1), 0);
		playerInfo.loadOut.greType_[1] = (GRENADE_TYPE)((int)playerInfo.loadOut.greType_[1] + 1);
		if (nextGrenadeType == playerInfo.loadOut.greType_[1]) {
			//playerInfo.loadOut.greType_[1] = (GRENADE_TYPE)(((int)(playerInfo.loadOut.greType_[1]) + 2));
		}
		if (playerInfo.loadOut.greType_[1] >= GRENADE_TYPE::MAX)
		{
			playerInfo.loadOut.greType_[1] = (GRENADE_TYPE)((int)(playerInfo.loadOut.greType_[1]) - (int)(GRENADE_TYPE::MAX));
		}
		AudioManager::GetInstance().Play(AudioList::AUDIO_TYPE::SYSTEM_WEAPON_SELECT, false, 0.8f, 44100);
	};

	onClickFunctionList_[14] = [&](void)->void {
		auto& gameManager = GameManager::GetInstance();
		auto& playerInfo = gameManager.GetPlayerInfo(static_cast<int>(ID::PLAYER_1));
		auto nextGrenadeType = gameManager.GetGrenadeType(static_cast<int>(ID::PLAYER_1), 0);
		playerInfo.loadOut.greType_[1] = (GRENADE_TYPE)((int)playerInfo.loadOut.greType_[1] - 1);
		if (nextGrenadeType == playerInfo.loadOut.greType_[1]) {
			//playerInfo.loadOut.greType_[1] = (GRENADE_TYPE)(((int)(playerInfo.loadOut.greType_[1]) + 1ZZz));
		}
		if (playerInfo.loadOut.greType_[1] < GRENADE_TYPE::HAND)
		{
			playerInfo.loadOut.greType_[1] = (GRENADE_TYPE)((int)(playerInfo.loadOut.greType_[1]) + (int)(GRENADE_TYPE::MAX));
		}
		AudioManager::GetInstance().Play(AudioList::AUDIO_TYPE::SYSTEM_WEAPON_SELECT, false, 0.8f, 44100);
	};

	onClickFunctionList_[15] = [&](void)->void {
		AudioManager::GetInstance().Play(AudioList::AUDIO_TYPE::PLAYER_DEATH3, false, 0.8f, 44100);
	};

	onClickFunctionList_[16] = [&](void)->void {
		auto uiScreen = SceneManager::GetInstance().GetUIScreen();
		uiScreen.lock()->PopUILayer();
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::MENU);
	};

	onClickFunctionList_[17] = [&](void)->void {

	};

	onClickFunctionList_[18] = [&](void)->void {

	};

	onClickFunctionList_[19] = [&](void)->void {
		auto uiScreen = SceneManager::GetInstance().GetUIScreen();
		uiScreen.lock()->PopUILayer();
		GameManager::GetInstance().SetActive(true);
		SetMouseDispFlag(false);
	};

	onClickFunctionList_[20] = [&](void)->void {
		GameManager::GetInstance().Back();
	};

	onClickFunctionList_[21] = [&](void)->void {
		auto& self = NetManager::GetInstance().GetSelf();
		auto& info = GameManager::GetInstance().GetPlayerInfo((int)(self.playerType));
		info.loadOut.gunType_ = (GUN_TYPE)((int)info.loadOut.gunType_ + 1);
		if (info.loadOut.gunType_ >= GUN_TYPE::MAX)
		{
			info.loadOut.gunType_ = GUN_TYPE::AK47;
		}
		NetManager::GetInstance().SetLoadOut((int)info.loadOut.gunType_
			, (int)info.loadOut.greType_[0], (int)info.loadOut.greType_[1]);
		AudioManager::GetInstance().Play(AudioList::AUDIO_TYPE::SYSTEM_WEAPON_SELECT, false, 0.8f, 44100);
	};

	onClickFunctionList_[22] = [&](void)->void {
		auto& self = NetManager::GetInstance().GetSelf();
		auto& info = GameManager::GetInstance().GetPlayerInfo((int)(self.playerType));
		info.loadOut.gunType_ = (GUN_TYPE)((int)info.loadOut.gunType_ - 1);
		if (info.loadOut.gunType_ < GUN_TYPE::AK47)
		{
			info.loadOut.gunType_ = GUN_TYPE::M24R;
		}
		NetManager::GetInstance().SetLoadOut((int)info.loadOut.gunType_
			, (int)info.loadOut.greType_[0], (int)info.loadOut.greType_[1]);
		AudioManager::GetInstance().Play(AudioList::AUDIO_TYPE::SYSTEM_WEAPON_SELECT, false, 0.8f, 44100);
	};

	onClickFunctionList_[23] = [&](void)->void {
		auto& self = NetManager::GetInstance().GetSelf();
		auto& info = GameManager::GetInstance().GetPlayerInfo((int)(self.playerType));
		info.loadOut.greType_[0] = (GRENADE_TYPE)((int)info.loadOut.greType_[0] + 1);
		if (info.loadOut.greType_[0] >= GRENADE_TYPE::MAX)
		{
			info.loadOut.greType_[0] = (GRENADE_TYPE)((int)(info.loadOut.greType_[0]) - (int)(GRENADE_TYPE::MAX));
		}
		NetManager::GetInstance().SetLoadOut((int)info.loadOut.gunType_
			, (int)info.loadOut.greType_[0], (int)info.loadOut.greType_[1]);
		AudioManager::GetInstance().Play(AudioList::AUDIO_TYPE::SYSTEM_WEAPON_SELECT, false, 0.8f, 44100);
	};

	onClickFunctionList_[24] = [&](void)->void {
		auto& self = NetManager::GetInstance().GetSelf();
		auto& info = GameManager::GetInstance().GetPlayerInfo((int)(self.playerType));
		info.loadOut.greType_[0] = (GRENADE_TYPE)((int)info.loadOut.greType_[0] - 1);
		if (info.loadOut.greType_[0] < GRENADE_TYPE::HAND)
		{
			info.loadOut.greType_[0] = (GRENADE_TYPE)((int)(info.loadOut.greType_[0]) + (int)(GRENADE_TYPE::MAX));
		}
		NetManager::GetInstance().SetLoadOut((int)info.loadOut.gunType_
			, (int)info.loadOut.greType_[0], (int)info.loadOut.greType_[1]);
		AudioManager::GetInstance().Play(AudioList::AUDIO_TYPE::SYSTEM_WEAPON_SELECT, false, 0.8f, 44100);
	};

	onClickFunctionList_[25] = [&](void)->void {
		auto& self = NetManager::GetInstance().GetSelf();
		auto& info = GameManager::GetInstance().GetPlayerInfo((int)(self.playerType));
		info.loadOut.greType_[1] = (GRENADE_TYPE)((int)info.loadOut.greType_[1] + 1);
		if (info.loadOut.greType_[1] >= GRENADE_TYPE::MAX)
		{
			info.loadOut.greType_[1] = (GRENADE_TYPE)((int)(info.loadOut.greType_[1]) - (int)(GRENADE_TYPE::MAX));
		}
		NetManager::GetInstance().SetLoadOut((int)info.loadOut.gunType_
			, (int)info.loadOut.greType_[0], (int)info.loadOut.greType_[1]);
		AudioManager::GetInstance().Play(AudioList::AUDIO_TYPE::SYSTEM_WEAPON_SELECT, false, 0.8f, 44100);
	};

	onClickFunctionList_[26] = [&](void)->void {
		auto& self = NetManager::GetInstance().GetSelf();
		auto& info = GameManager::GetInstance().GetPlayerInfo((int)(self.playerType));
		info.loadOut.greType_[1] = (GRENADE_TYPE)((int)info.loadOut.greType_[1] - 1);
		if (info.loadOut.greType_[1] < GRENADE_TYPE::HAND)
		{
			info.loadOut.greType_[1] = (GRENADE_TYPE)((int)(info.loadOut.greType_[1]) + (int)(GRENADE_TYPE::MAX));
		}
		NetManager::GetInstance().SetLoadOut((int)info.loadOut.gunType_
			, (int)info.loadOut.greType_[0], (int)info.loadOut.greType_[1]);
		AudioManager::GetInstance().Play(AudioList::AUDIO_TYPE::SYSTEM_WEAPON_SELECT, false, 0.8f, 44100);
	};

	onClickFunctionList_[27] = [&](void)->void {
		// そのままコネクトシーンへ
		// 通信開始(待機)
		NetManager::GetInstance().Run(NET_MODE::HOST);
		NetManager::GetInstance().SetPlayerName(GameManager::GetInstance().GetName());
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::mLOBBY);
		GameManager::GetInstance().ChangeMode(GameManager::MODE::HOST);
		AudioManager::GetInstance().Play(AudioList::AUDIO_TYPE::SYSTEM_CLICKBUTTON, false, 1.0f, 44100);
	};

	onClickFunctionList_[28] = [&](void)->void {
		auto& netmanager = NetManager::GetInstance();
		auto& gamemanager = GameManager::GetInstance();
		if (netmanager.GetMode() == NET_MODE::HOST)
		{

			auto& players = netmanager.GetNetUsers();
			if (players.size() > 1 && gamemanager.IsDivideTeam() &&
				netmanager.IsSameGameState(GAME_STATE::LOBBY))
			{
				NetManager::GetInstance().ChangeGameState(GAME_STATE::GOTO_GAME);
				AudioManager::GetInstance().Play(AudioList::AUDIO_TYPE::SYSTEM_CHANGESCENE_MULTI, false, 1.0f, 44100);
			}
			else {
				AudioManager::GetInstance().Play(AudioList::AUDIO_TYPE::SYSTEM_CANCEL, false, 1.0f, 44100);
			}
		}
		else {
			AudioManager::GetInstance().Play(AudioList::AUDIO_TYPE::SYSTEM_CANCEL, false, 1.0f, 44100);
		}
	};

}

void UIFunctionList::RegisterEndInputFunction(void) {
	endInputFunctionList_[0] = [&](std::weak_ptr<UIBase> ui)->void {
		auto inputTextArea = std::dynamic_pointer_cast<UI_InputTextArea>(ui.lock());
		auto ip = NetManager::GetInstance().GetHostIp();
		ip.d1 = std::stoi(inputTextArea->GetInputString());
		NetManager::GetInstance().SetHostIp(ip);
	};

	endInputFunctionList_[1] = [&](std::weak_ptr<UIBase> ui)->void {
		auto inputTextArea = std::dynamic_pointer_cast<UI_InputTextArea>(ui.lock());
		auto ip = NetManager::GetInstance().GetHostIp();
		ip.d2 = std::stoi(inputTextArea->GetInputString());
		NetManager::GetInstance().SetHostIp(ip);
	};

	endInputFunctionList_[2] = [&](std::weak_ptr<UIBase> ui)->void {
		auto inputTextArea = std::dynamic_pointer_cast<UI_InputTextArea>(ui.lock());
		auto ip = NetManager::GetInstance().GetHostIp();
		ip.d3 = std::stoi(inputTextArea->GetInputString());
		NetManager::GetInstance().SetHostIp(ip);
	};

	endInputFunctionList_[3] = [&](std::weak_ptr<UIBase> ui)->void {
		auto inputTextArea = std::dynamic_pointer_cast<UI_InputTextArea>(ui.lock());
		auto ip = NetManager::GetInstance().GetHostIp();
		ip.d4 = std::stoi(inputTextArea->GetInputString());
		NetManager::GetInstance().SetHostIp(ip);
	};

	endInputFunctionList_[4] = [&](std::weak_ptr<UIBase> ui)->void {
		auto inputTextArea = std::dynamic_pointer_cast<UI_InputTextArea>(ui.lock());
		auto& netManager = NetManager::GetInstance();
		GameManager::GetInstance().SetName(inputTextArea->GetInputString());
	};
}

void UIFunctionList::RegisterGetAddTextFunction(void) {
	getAddTextFunctionList_[0] = [](void)->std::string {
		auto& messageTable = MessageTable::GetInstance();
		auto& gameManager = GameManager::GetInstance();
		std::string missionText;
		if (gameManager.GetMode() == GameManager::MODE::SCOREATTACK) {
			missionText = Utility::WStringToString(messageTable.GetMessage_(23));
		}
		else if (gameManager.GetMode() == GameManager::MODE::TIMEATTACK) {
			missionText = Utility::WStringToString(messageTable.GetMessage_(31));
		}
		return missionText;
	};

	getAddTextFunctionList_[1] = [](void)->std::string {
		auto& messageTable = MessageTable::GetInstance();
		auto& gameManager = GameManager::GetInstance();
		std::string scoreText = std::to_string(gameManager.GetScore(0));
		return scoreText;
	};

	getAddTextFunctionList_[2] = [](void)->std::string {
		auto& messageTable = MessageTable::GetInstance();
		auto& gameManager = GameManager::GetInstance();
		const auto& playerInfo = gameManager.GetPlayerInfo(0);
		int calcTotalScore = playerInfo.score;
		calcTotalScore += (playerInfo.death == 0) ? 1000 : 0;
		std::string scoreText = std::to_string(calcTotalScore);
		return scoreText;
	};

	getAddTextFunctionList_[3] = [](void)->std::string {
		auto& messageTable = MessageTable::GetInstance();
		auto& gameManager = GameManager::GetInstance();
		std::string scoreText = std::to_string(gameManager.GetScore(0));
		return scoreText;
	};

	getAddTextFunctionList_[4] = [](void)->std::string {
		auto& messageTable = MessageTable::GetInstance();
		auto& gameManager = GameManager::GetInstance();
		const auto& playerInfo = gameManager.GetPlayerInfo(0);
		std::string scoreText = std::to_string(playerInfo.kill);
		return scoreText;
	};

	getAddTextFunctionList_[5] = [](void)->std::string {
		auto& messageTable = MessageTable::GetInstance();
		auto& gameManager = GameManager::GetInstance();
		const auto& playerInfo = gameManager.GetPlayerInfo(0);
		std::string scoreText = "+" + std::to_string((playerInfo.death == 0) ? 1000 : 0);
		return scoreText;
	};

	getAddTextFunctionList_[6] = [](void)->std::string {
		auto& messageTable = MessageTable::GetInstance();
		auto& gameManager = GameManager::GetInstance();
		const auto& playerInfo = gameManager.GetPlayerInfo(0);
		std::string scoreText = "+" + std::to_string((playerInfo.death == 0) ? 1000 : 0);
		return scoreText;
	};

	getAddTextFunctionList_[7] = [](void)->std::string {
		return "";
	};
}

void UIFunctionList::RegisterGetLogDataFunction(void) {
	getLogDataFunctionList_[0] = [](void)->std::shared_ptr<UI_LogMessageArea::MESSAGE_DATA_BASE> {
		return GameManager::GetInstance().GetLogMessageData();
	};
}
