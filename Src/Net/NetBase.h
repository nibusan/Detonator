#pragma once
#include "NetManager.h"

class NetBase
{
public:

	static constexpr float TIME_SEND_USER = 1.0f;
	static constexpr float TIME_SEND_USERS = 10.0f / 60.0f;
	static constexpr float TIME_SEND_ACTIONS = 1.0f / 60.0f;

	// �R���X�g���N�^
	NetBase(NetManager& manager);

	// �f�X�g���N�^
	virtual ~NetBase(void);

	// �X�V
	//virtual void UpdateConnecting(void) = 0;
	//virtual void UpdateGotoLobby(void) = 0;
	virtual void UpdateLobby(void) = 0;
	virtual void UpdateGotoGame(void) = 0;
	virtual void UpdateGamePlaying(void) = 0;
	virtual void UpdateGotoResult(void) = 0;
	virtual void UpdateResult(void) = 0;

	// ��M����
	//virtual void UdpReceiveThreadConnecting(void) = 0;
	//virtual void UdpReceiveThreadGotoLobby(void) = 0;
	virtual void UdpReceiveThreadLobby(void) = 0;
	virtual void UdpReceiveThreadGotoGame(void) = 0;
	virtual void UdpReceiveThreadGamePlaying(void) = 0;
	virtual void UdpReceiveThreadGotoResult(void) = 0;
	virtual void UdpReceiveThreadResult(void) = 0;

protected:

	// �Ǘ��N���X�̎Q��
	NetManager& manager_;

	// �X�V�X�e�b�v
	float step_;
	float stepAction_;
};

