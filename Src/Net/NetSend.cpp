#include "../Manager/SceneManager.h"
#include "NetManager.h"
#include "NetSend.h"

NetSend::NetSend(NetManager& manager, int& sendSocketId) : manager_(manager) , sendSocketId_(sendSocketId)
{
	type_ = NET_DATA_TYPE::NONE;
}

NetSend::~NetSend(void)
{
}

void NetSend::Send(NET_DATA_TYPE type)
{
	// 通信種別によって送るものを変える
	type_ = type;

	switch (type_)
	{
	case NET_DATA_TYPE::USER:
		SendSelfJoinPlayer();
		break;
	case NET_DATA_TYPE::USERS:
		SendJoinPlayers();
		break;
	case NET_DATA_TYPE::ACTION_HIS_ALL:
		SendActionHisAll();
		break;
	case NET_DATA_TYPE::DEAD:
		SendDead();
		break;
	}
}

NET_BASIC_DATA NetSend::MakeBasicData(NET_DATA_TYPE type)
{
	NET_BASIC_DATA ret;
	ret.type = type;
	ret.key = manager_.GetSelf().key;
	//ret.gameTime = 0.0f;
	//ret.crc = crc;
	return ret;
}

void NetSend::SendUDP_Host(const char* bufPtr, int bufSize)
{
	// ホストへ送信
	NetWorkSendUDP(sendSocketId_, manager_.GetHostIp(), manager_.GetHostPort(), bufPtr, bufSize);
}

void NetSend::SendUDP_Clients(const char* bufPtr, int bufSize)
{
	// 自分以外へ送信
	NET_JOIN_USER user;

	auto& users = manager_.GetNetUsers();
	for (const auto& userPair : users)
	{

		user = userPair.second;
		if (user.key == manager_.GetSelf().key)
		{
			// 自分宛てには送信しない
			continue;
		}

		// 自分以外へ送信
		NetWorkSendUDP(sendSocketId_, user.ip, user.port, bufPtr, bufSize);

	}
}

void NetSend::SendJoinPlayer(const NET_JOIN_USER& entity)
{
	const size_t SIZE_BASIC = sizeof(NET_BASIC_DATA);
	const size_t SIZE_ENTITY = sizeof(NET_JOIN_USER);
	const size_t SIZE_ALL = SIZE_BASIC + SIZE_ENTITY;

	// 送信データのCRC
	//std::uint32_t crc = CRC::Calculate(&entity, SIZE_ENTITY, CRC::CRC_32());

	// バッファ確保
	char buf[SIZE_ALL] = { 0 };
	char* bufPtr = buf;

	// 基本データを頭に付けるため作成
	NET_BASIC_DATA basicData = MakeBasicData(type_);

	// 基本データをメモリコピー
	memcpy(bufPtr, &basicData, SIZE_BASIC);
	bufPtr += SIZE_BASIC;

	// 送信データをメモリコピー
	memcpy(bufPtr, &entity, SIZE_ENTITY);

	// 送信
	SendUDP_Host(buf, SIZE_ALL);
}

void NetSend::SendSelfJoinPlayer(void)
{
	SendJoinPlayer(manager_.GetSelf());
}

void NetSend::SendJoinPlayers(void)
{
	const size_t SIZE_BASIC = sizeof(NET_BASIC_DATA);
	const size_t SIZE_ENTITY = sizeof(NET_JOIN_USERS);
	const size_t SIZE_ALL = SIZE_BASIC + SIZE_ENTITY;

	// 送信データの作成
	NET_JOIN_USERS entity;
	auto& users = manager_.GetNetUsers();
	int cnt = 0;
	for (const auto& userPair : users)
	{
		entity.users[cnt] = userPair.second;
		cnt++;
	}

	// 送信データのCRC
	//std::uint32_t crc = CRC::Calculate(&entity, SIZE_ENTITY, CRC::CRC_32());

	// バッファ確保
	char buf[SIZE_ALL] = { 0 };
	char* bufPtr = buf;

	// 基本データを頭に付けるため作成
	NET_BASIC_DATA basicData = MakeBasicData(type_);

	// 基本データをメモリコピー
	memcpy(bufPtr, &basicData, SIZE_BASIC);
	bufPtr += SIZE_BASIC;

	// 送信データをメモリコピー
	memcpy(bufPtr, &entity, SIZE_ENTITY);

	// 送信
	SendUDP_Clients(buf, SIZE_ALL);
}

void NetSend::SendActionHisAll(void)
{
	const size_t SIZE_BASIC = sizeof(NET_BASIC_DATA);
	const size_t SIZE_ENTITY = sizeof(NET_ACTION_HIS);
	const size_t SIZE_ALL = SIZE_BASIC + SIZE_ENTITY;

	// 送信データの作成
	auto self = manager_.GetSelfAction();
	manager_.MakeActionHis(self);
	NET_ACTION_HIS entity = manager_.GetSelfActionHis();

	// 送信データのCRC
	//std::uint32_t crc = CRC::Calculate(&entity, SIZE_ENTITY, CRC::CRC_32());

	// バッファ確保
	char buf[SIZE_ALL] = { 0 };
	char* bufPtr = buf;

	// 基本データを頭に付けるため作成
	NET_BASIC_DATA basicData = MakeBasicData(type_);

	// 基本データをメモリコピー
	memcpy(bufPtr, &basicData, SIZE_BASIC);
	bufPtr += SIZE_BASIC;

	// 送信データをメモリコピー
	memcpy(bufPtr, &entity, SIZE_ENTITY);

	// 送信
	SendUDP_Clients(buf, SIZE_ALL);
}

void NetSend::SendDead(void)
{
	const size_t SIZE_BASIC = sizeof(NET_BASIC_DATA);
	const size_t SIZE_ENTITY = sizeof(NET_ACTION_HIS);
	const size_t SIZE_ALL = SIZE_BASIC + SIZE_ENTITY;

	// 送信データの作成
	auto entity = manager_.GetSelfDead();

	// 送信データのCRC
	//std::uint32_t crc = CRC::Calculate(&entity, SIZE_ENTITY, CRC::CRC_32());

	// バッファ確保
	char buf[SIZE_ALL] = { 0 };
	char* bufPtr = buf;

	// 基本データを頭に付けるため作成
	NET_BASIC_DATA basicData = MakeBasicData(type_);

	// 基本データをメモリコピー
	memcpy(bufPtr, &basicData, SIZE_BASIC);
	bufPtr += SIZE_BASIC;

	// 送信データをメモリコピー
	memcpy(bufPtr, &entity, SIZE_ENTITY);

	// 送信
	SendUDP_Clients(buf, SIZE_ALL);
}
