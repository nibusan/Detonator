#include "../Manager/SceneManager.h"
#include "NetManager.h"
#include "NetHost.h"

NetHost::NetHost(NetManager& manager) : NetBase(manager)
{
}

NetHost::~NetHost(void)
{
}

//void NetHost::UpdateConnecting(void)
//{
//	step_ += SceneManager::GetInstance().GetDeltaTime();
//	if (step_ >= TIME_SEND_USERS)
//	{
//		step_ = 0.0f;
//		manager_.Send(NET_DATA_TYPE::USERS);
//	}
//}
//
//void NetHost::UpdateGotoLobby(void)
//{
//	step_ += SceneManager::GetInstance().GetDeltaTime();
//	if (step_ >= TIME_SEND_USERS)
//	{
//		step_ = 0.0f;
//		manager_.Send(NET_DATA_TYPE::USERS);
//	}
//}

void NetHost::UpdateLobby(void)
{
	//step_ += SceneManager::GetInstance().GetDeltaTime();
	//if (step_ >= TIME_SEND_USERS)
	//{
	//	step_ = 0.0f;
		manager_.Send(NET_DATA_TYPE::USERS);
	//}
}

void NetHost::UpdateGotoGame(void)
{
	//step_ += SceneManager::GetInstance().GetDeltaTime();
	//if (step_ >= TIME_SEND_USERS)
	//{
	//	step_ = 0.0f;
		manager_.Send(NET_DATA_TYPE::USERS);
	//}
}

void NetHost::UpdateGamePlaying(void)
{
	// ñàÉtÉåÅ[ÉÄëóêM
	manager_.Send(NET_DATA_TYPE::ACTION_HIS_ALL);
}

void NetHost::UpdateGotoResult(void)
{
	manager_.Send(NET_DATA_TYPE::USERS);
}

void NetHost::UpdateResult(void)
{
	manager_.Send(NET_DATA_TYPE::USERS);
}
//
//void NetHost::UdpReceiveThreadConnecting(void)
//{
//}
//
//void NetHost::UdpReceiveThreadGotoLobby(void)
//{
//}

void NetHost::UdpReceiveThreadLobby(void)
{
}

void NetHost::UdpReceiveThreadGotoGame(void)
{
}

void NetHost::UdpReceiveThreadGamePlaying(void)
{
}

void NetHost::UdpReceiveThreadGotoResult(void)
{
}

void NetHost::UdpReceiveThreadResult(void)
{
}


