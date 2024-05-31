#pragma once
#include <DxLib.h>
#include <DxLib.h>
#include "../Common/Vector2.h"
#include "../Object/Common/Transform.h"

static constexpr int MAX_PLAYERS = 4;
static constexpr int MAX_SEND_BYTES = 1024;
static constexpr int NUM_FRAME = 10;
static constexpr int MAX_NAME_NUM = 20;

// �ʐM���[�h
enum class NET_MODE
{
	NONE,
	HOST,
	CLIENT,
};

// �Q�[���i�s���
enum class GAME_STATE
{
	NONE,
	//CONNECTING,
	//GOTO_LOBBY,
	LOBBY,
	GOTO_GAME,
	GAME_PLAYING,
	GOTO_RESULT,
	RESULT,
};

// �Q�[���v���C���[���
enum class PLAYER_TYPE
{
	PLAYER_1,
	PLAYER_2,
	PLAYER_3,
	PLAYER_4,
};

// �ʐM���
enum class NET_DATA_TYPE
{
	NONE,
	USER,
	USERS,
	ACTION_HIS_ALL,
	DEAD,
};

// ��{�ʐM�f�[�^
struct NET_BASIC_DATA
{
	// �ʐM���
	NET_DATA_TYPE type = NET_DATA_TYPE::NONE;
	// ���[�U�̔C�ӃL�[
	int key = -1;
	// �Q�[���̐i�s����
	//float gameTime = 0.0f;
	//uint32_t crc;
};

//// �J�������
//struct CAMERA_INFO
//{
//	VECTOR pos = {};
//	VECTOR rot = {};
//	VECTOR targetPos = {};
//};

// ���[�U�f�[�^
struct NET_JOIN_USER
{
	// �ʐM���[�h(�z�X�g or �N���C�A���g)
	NET_MODE mode = NET_MODE::NONE;
	// ���[�U�̔C�ӃL�[
	int key = -1;
	// ���[�U��IP�A�h���X
	IPDATA ip = { 0, 0, 0, 0 };
	// ���[�U�̃|�[�g
	int port = -1;
	// ���[�U1�A2�A3�A4...
	PLAYER_TYPE playerType = PLAYER_TYPE::PLAYER_1;
	// �S�̂̃Q�[���i�s���
	GAME_STATE gameState = GAME_STATE::LOBBY;
	// �Q�[�����[�htest
	int gameMode = 0;
	// �`�[��
	int team = 0;
	// ���O
	char playerName[MAX_NAME_NUM] = "";
	// �̍�
	int alias = 0;
	// ����
	int gun = 0;
	int grenade[2] = { 0,0 };
	void Reset()
	{
		mode = NET_MODE::NONE;
		key = -1;
		ip = { 0, 0, 0, 0 };
		port = -1;
		playerType = PLAYER_TYPE::PLAYER_1;
		gameState = GAME_STATE::LOBBY;
		gameMode = 0;
		team = 0;
		for (auto& name : playerName) {
			name = 0;
		}
		alias = 0;
		gun = 0;
		grenade[0] = 0;
		grenade[1] = 0;
	}
};

struct NET_JOIN_USERS
{
	NET_JOIN_USER users[MAX_PLAYERS];
};

enum class PLAYER_ACTION
{
	NONE = 0,
	MOVE_FORWARD = 1,
	MOVE_BACK = 2,
	MOVE_LEFT = 4,
	MOVE_RIGHT = 8,
	MOVE_RUN = 16,
	CROUCH = 32,
	JUMP = 64,
	SHOT = 128,
	ADS = 256,
	RELOAD = 512,
	GRENADE = 1024,
	GRENADE_CHANGE = 2048,
	GRENADE_INITIATION = 4096,
	ROTATE = 8192,
};

struct NET_ACTION
{
	// ���[�U�̔C�ӃL�[
	int key = -1;
	// �Q�[���i�s�t���[��
	unsigned int frameNo = 0;
	// �A�N�V�����L�[
	unsigned int actBits = 0;
	// ��]��
	Vector2<float> rotPow_ = {};
	// �����_��
	int randSeed = 0;
	// �J�������
	//CAMERA_INFO cameraInfo_ = {};

	void Reset(void) {
		// �Q�[���i�s�t���[��
		frameNo = 0;
		// �A�N�V�����L�[
		actBits = 0;
		// ��]��
		rotPow_ = {};
		// �����_��
		randSeed = 0;
		// �J�������
		//cameraInfo_ = {};
	}
};

struct DEAD
{
	// ���[�U�̔C�ӃL�[
	int key = -1;
	// �Q�[���i�s�t���[��
	unsigned int frameNo = 0;
	// ���S���
	bool isDead = false;
};

struct NET_ACTION_HIS
{
	// ���[�U�̔C�ӃL�[
	int key = -1;
	// �ߋ��t���[�����̃A�N�V�������(NUM_FRAME(10)�t���[����)
	NET_ACTION actions[NUM_FRAME];

	bool isReceive = false;
};

struct NET_USERS_ACTION_HIS
{
	// �S�����̉ߋ��t���[�����̃A�N�V�������
	NET_ACTION_HIS actionsHistories[MAX_PLAYERS];
};
