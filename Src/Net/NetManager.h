#pragma once
#include <map>
#include <mutex>
#include "NetStructures.h"

class NetSend;
class NetBase;

class NetManager
{
public:

	// �ő��M��
	static constexpr int MAX_RECEIVE_NUM = (10);

	// ��M�ҋ@����(�~���b)
	static constexpr float RECEIVE_TERM_MSEC = (1.0f / 120.0f * 1000.0f);

	// �����ҋ@����(�~���b)
	static constexpr float SYNC_TERM_MSEC = (1.0f / 120.0f * 1000.0f);

	// 
	static constexpr float RECEIVE_TIME = (5.0f);

	struct NET_POOL
	{

		// �z�X�g�̃l�b�g���[�U���
		NET_JOIN_USER hostJoinUser_;

		// �������g�̃l�b�g���[�U���
		NET_JOIN_USER selfJoinUser_;

		// �������g�̃A�N�V�������
		NET_ACTION selfAction_;

		// �������g�̃A�N�V������񗚗�
		NET_ACTION_HIS selfActionHis_;

		// �������g�̎��S���
		DEAD selfDead_;

		// �ڑ�����(�������g���܂�)(�C�ӂ̃L�[�Ŕ���)
		std::map<int, NET_JOIN_USER> joinUsers_;

		// �ڑ�����(�������g���܂�)�̃A�N�V������񗚗�(�C�ӂ̃L�[�Ŕ���)
		std::map<int, NET_ACTION_HIS> joinUserActionHis_;

		// �ڑ�����(�������g���܂�)�̎��S���(�C�ӂ̃L�[�Ŕ���)
		std::map<int, DEAD> joinUserDeath_;



	};

	// �C���X�^���X�̐���
	static void CreateInstance(void);

	// �C���X�^���X�̎擾
	static NetManager& GetInstance(void);

	void Init(void);

	// ���\�[�X�̔j��
	void Destroy(void);

	// �z�X�g��IP�A�h���X
	IPDATA GetHostIp(void) const;
	void SetHostIp(IPDATA hostIp);

	int GetPlayerNum(void);

	std::string& GetPlayerName(void);
	int GetAlias(void);
	int GetTeam(void);
	void SetPlayerName(std::string name);
	void SetAlias(int alias);
	void SetTeam(int team);
	void SetLoadOut(int gun, int gre1, int gre2);

	bool IsDead(int key);
	void SetDead(bool isDead);

	// ���[�h�擾
	NET_MODE GetMode(void) const;

	// �z�X�g���[�h�����f
	bool IsHost(void) const;
	bool IsRunning(void) const;

	// �ʐM�J�n
	void Run(NET_MODE mode);

	// �X�V����
	void Update(void);
	void DataReflection(void);
	void UpdateEndOfFrame(void);

	// ���Ԍo��
	void UpdateTime(void);

	// ���̃t���[����
	void NextFrame(void);

	// �ʐM�I��
	void Stop(void);

	// �ʐM�𑗂�
	void Send(NET_DATA_TYPE type);

	// ��ԑJ��
	void ChangeGameState(GAME_STATE state);

#pragma region ��p�v�[��

	// �����̃l�b�g�����擾
	const NET_JOIN_USER GetSelf(void) const;

	// �ڑ�������擾
	const std::map<int, NET_JOIN_USER> GetNetUsers(void) const;
	const std::map<int, NET_ACTION_HIS> GetNetUserActionHis(void) const;

	// �z�X�g�̃Q�[���i�s��
	GAME_STATE GetGameStateHost(void) const;

	// �������g�̃Q�[���i�s��
	GAME_STATE GetGameStateSelf(void) const;

	// �S�Ă̐ڑ����肪�w�肳�ꂽ�Q�[���i�s��Ԃ����f
	bool IsSameGameState(GAME_STATE state);

	// �����̃A�N�V���������擾
	const NET_ACTION GetSelfAction(void) const;

	const DEAD GetSelfDead(void) const;

	// �ڑ�����̃A�N�V���������擾
	const std::map<int, NET_ACTION_HIS> GetActionHis(void) const;

	const NET_JOIN_USER& GetHostJoinUser(void)const;

	// �A�N�V��������
	bool IsAction(int key, PLAYER_ACTION action);

	// �A�N�V��������
	bool IsAction(int key, PLAYER_ACTION action, bool isAction);
	Vector2<float> GetRotatePow(int key, Vector2<float> pow);
	int GetRandSeed(int key, int seed);
	//CAMERA_INFO GetCameraInfo(int key, CAMERA_INFO info);
	//VECTOR GetTargetPos(int key, VECTOR pos);
	//VECTOR GetBulletDir(int key, VECTOR vec);
	//Transform GetTransform(int key, Transform transform);
	//VECTOR GetFrameRot(int key, VECTOR rot);
	//VECTOR GetCameraRot(int key, VECTOR rot);

	// �A�N�V�����ݒ�
	void SetAction(PLAYER_ACTION act);
	void SetRotPow(Vector2<float> pow);
	void SetRandSeed(int seed);
	//void SetCameraInfo(CAMERA_INFO info);
	//void SetTargetPos(VECTOR pos);
	//void SetBulletDir(VECTOR vec);
	//void SetTransform(Transform transform);
	//void SetFrameRot(VECTOR rot);
	//void SetCameraRot(VECTOR rot);

	// �A�N�V�����ݒ�
	void SetGameMode(int mode);

	// �S�Ă̐ڑ����肪�w�肳�ꂽ�Q�[���J�E���g�����f
	bool IsSameFrameNo(void);

	// �S���[�U����f�[�^���󂯎������
	bool IsReceiveAll(void);
	void ResetReceiveAll(void);

	// �����̃A�N�V������񗚗�
	const NET_ACTION_HIS GetSelfActionHis(void) const;



#pragma endregion

	// �z�X�g�̃|�[�g�ԍ��擾
	int GetHostPort(void) const;

	// �������A�N�V�����ݒ�
	void ResetAction(void);

	unsigned int GetFrameNo(void) const;

	//void MakeActionHis(int frameNo, NET_ACTION action);
	void MakeActionHis(NET_ACTION action);

	bool IsSync(void);

	bool HostIsNotFound(void);

private:

	const int RECEIVE_PORT_HOST = 65000;
	const int RECEIVE_PORT_CLIENT_RANGE = RECEIVE_PORT_HOST + 100;

	// �ʐM������܂ł̎���
	// �����ԒʐM���Ȃ���΃��[�U�[�������
	std::map<int, float> joinUserTime_;
	float hostTime_;

	// �z�X�g��������Ȃ������ꍇtrue
	bool hostIsNotFound_;

	// �ÓI�C���X�^���X
	static NetManager* instance_;

	// �f�t�H���g�R���X�g���N�^��private�ɂ��āA
	// �O�����琶���ł��Ȃ��l�ɂ���
	NetManager(void);

	// �R�s�[�R���X�g���N�^�����l
	NetManager(const NetManager&);

	// �f�X�g���N�^�����l
	~NetManager(void) = default;

	// �ʐM�J�n�t���O
	bool isRunning_;

	// �����t���O(�Q�[�����̂�true)
	bool isSync_;

	// �\�P�b�g�n���h��
	int sendSocketId_;
	int recvSocketId_;

	std::unique_ptr<NetBase> net_;
	std::unique_ptr<NetSend> netSend_;

	// ����(�z�X�g�A�N���C�A���g)
	NET_MODE mode_;

	// �z�X�g��IP�A�h���X
	IPDATA hostIp_;

	// �X���b�h
	std::thread recvThread_;

	// ����M�f�[�^�̔r������
	// �r���I�˕��O�҂�r�˂��đނ���
	// poolShare_�p
	std::mutex poolMutex_;

	// ����M���̃f�[�^�����p
	// �r������O�ł͐�΂ɓǂݏ������Ȃ�����
	NET_POOL poolShare_;

	// �r������O�Ŏg�p
	NET_POOL pool_;

	int frameNo_;

	bool isFirstReceive_;

	// �ʐM��ʂ��Ƃ̍X�V����
	void UpdateState(void);

	// ��M�X���b�h
	void UdpReceiveThread(void);

	// �f�[�^��M����
	void UdpReceiveData(void);

	// ����M�p�̊�{�f�[�^���o�b�t�@����擾����
	NET_BASIC_DATA GetBasicData(const char* bufptr);

#pragma region ����M�f�[�^��������

	// �ڑ������ǉ��E�ύX
	void ReplaceUser(NET_JOIN_USER entity);

	// �ڑ������ݒ�(��{�I�ɃN���C�A���g���̏���)
	// ���Ă킯�ł��Ȃ�����
	void ReplaceUsers(NET_JOIN_USERS entities);

	// �A�N�V��������ǉ��E�ύX
	void ReplaceActionHis(NET_ACTION_HIS entity);

#pragma endregion

};

