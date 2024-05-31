#pragma once
#include "NetBase.h"

class NetHost : public NetBase
{
public:

	// �R���X�g���N�^
	NetHost(NetManager& manager);

	// �f�X�g���N�^
	~NetHost(void);

	// �X�V
	//void UpdateConnecting(void) override;
	//void UpdateGotoLobby(void) override;
	void UpdateLobby(void) override;
	void UpdateGotoGame(void) override;
	void UpdateGamePlaying(void) override;
	void UpdateGotoResult(void) override;
	void UpdateResult(void) override;

protected:


	//void UdpReceiveThreadConnecting(void) override;
	//void UdpReceiveThreadGotoLobby(void) override;
	void UdpReceiveThreadLobby(void) override;
	void UdpReceiveThreadGotoGame(void) override;
	void UdpReceiveThreadGamePlaying(void) override;
	void UdpReceiveThreadGotoResult(void) override;
	void UdpReceiveThreadResult(void) override;
};

