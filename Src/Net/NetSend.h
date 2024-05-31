#pragma once
#include "NetStructures.h"

class NetManager;

class NetSend
{
public:

	NetSend(NetManager& manager, int& sendSocketId);
	~NetSend(void);

	// ���M����
	void Send(NET_DATA_TYPE type);

private:

	NetManager& manager_;

	// ���M�p�\�P�b�g�n���h��
	int& sendSocketId_;

	// ���M���
	NET_DATA_TYPE type_;

	// ����M�p�̊�{�f�[�^���쐬����
	NET_BASIC_DATA MakeBasicData(NET_DATA_TYPE type);
	//NET_BASIC_DATA MakeBasicData(NET_DATA_TYPE type, std::uint32_t crc);

	// �z�X�g�֑��M
	void SendUDP_Host(const char* bufPtr, int bufSize);

	// �N���C�A���g�֑��M
	void SendUDP_Clients(const char* bufPtr, int bufSize);

	// �e�푗�M����
	// �v���C���[�ǉ�
	void SendJoinPlayer(const NET_JOIN_USER& entity);
	// �������g�𑗐M
	void SendSelfJoinPlayer(void);
	// �v���C���[�B�ɑ��M
	void SendJoinPlayers(void);
	// �A�N�V�������𑗐M
	void SendActionHisAll(void);
	// ���S��Ԃ𑗐M
	void SendDead(void);
};

