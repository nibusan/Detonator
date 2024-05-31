#include "../Manager/SceneManager.h"
#include "../Manager/AudioManager.h"
#include "NetManager.h"
#include "NetClient.h"

NetClient::NetClient(NetManager& manager) : NetBase(manager)
{

}

NetClient::~NetClient(void)
{
}

//void NetClient::UpdateConnecting(void)
//{
//	step_ += SceneManager::GetInstance().GetDeltaTime();
//	if (step_ >= TIME_SEND_USER)
//	{
//		step_ = 0.0f;
//		manager_.Send(NET_DATA_TYPE::USER);
//	}
//}
//
//void NetClient::UpdateGotoLobby(void)
//{
//	step_ += SceneManager::GetInstance().GetDeltaTime();
//	if (step_ >= TIME_SEND_USER)
//	{
//		step_ = 0.0f;
//		manager_.Send(NET_DATA_TYPE::USER);
//	}
//}

void NetClient::UpdateLobby(void)
{
	//step_ += SceneManager::GetInstance().GetDeltaTime();
	//if (step_ >= TIME_SEND_USER)
	//{
	//	step_ = 0.0f;
		manager_.Send(NET_DATA_TYPE::USER);
	//}
}

void NetClient::UpdateGotoGame(void)
{
	//step_ += SceneManager::GetInstance().GetDeltaTime();
	//if (step_ >= TIME_SEND_USER)
	//{
	//	step_ = 0.0f;
		manager_.Send(NET_DATA_TYPE::USER);
	//}
}

void NetClient::UpdateGamePlaying(void)
{
	// 毎フレーム送信
	manager_.Send(NET_DATA_TYPE::ACTION_HIS_ALL);
}

void NetClient::UpdateGotoResult(void)
{
	manager_.Send(NET_DATA_TYPE::USER);
}

void NetClient::UpdateResult(void)
{
	manager_.Send(NET_DATA_TYPE::USER);
}

//void NetClient::UdpReceiveThreadConnecting(void)
//{
//	// ホストがロビー画面へ遷移したら、
//	// クライアントも一斉に遷移する
//	if (manager_.GetGameStateHost() >= GAME_STATE::GOTO_LOBBY)
//	{
//		manager_.ChangeGameState(GAME_STATE::GOTO_LOBBY);
//	}
//}
//
//void NetClient::UdpReceiveThreadGotoLobby(void)
//{
//}

void NetClient::UdpReceiveThreadLobby(void)
{
	// ホストがゲーム画面へ遷移したら、
	// クライアントも一斉に遷移する
	if (manager_.GetGameStateHost() == GAME_STATE::GOTO_GAME)
	{
		AudioManager::GetInstance().Play(AudioList::AUDIO_TYPE::SYSTEM_CHANGESCENE_MULTI, false, 1.0f, 44100);
		manager_.ChangeGameState(GAME_STATE::GOTO_GAME);
	}
}

void NetClient::UdpReceiveThreadGotoGame(void)
{
}

void NetClient::UdpReceiveThreadGamePlaying(void)
{

}

void NetClient::UdpReceiveThreadGotoResult(void)
{
}

void NetClient::UdpReceiveThreadResult(void)
{
}

