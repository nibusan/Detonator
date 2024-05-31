#pragma once
#include <DxLib.h>
#include <DxLib.h>
#include "../Common/Vector2.h"
#include "../Object/Common/Transform.h"

static constexpr int MAX_PLAYERS = 4;
static constexpr int MAX_SEND_BYTES = 1024;
static constexpr int NUM_FRAME = 10;
static constexpr int MAX_NAME_NUM = 20;

// 通信モード
enum class NET_MODE
{
	NONE,
	HOST,
	CLIENT,
};

// ゲーム進行状態
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

// ゲームプレイヤー種別
enum class PLAYER_TYPE
{
	PLAYER_1,
	PLAYER_2,
	PLAYER_3,
	PLAYER_4,
};

// 通信種別
enum class NET_DATA_TYPE
{
	NONE,
	USER,
	USERS,
	ACTION_HIS_ALL,
	DEAD,
};

// 基本通信データ
struct NET_BASIC_DATA
{
	// 通信種別
	NET_DATA_TYPE type = NET_DATA_TYPE::NONE;
	// ユーザの任意キー
	int key = -1;
	// ゲームの進行時間
	//float gameTime = 0.0f;
	//uint32_t crc;
};

//// カメラ情報
//struct CAMERA_INFO
//{
//	VECTOR pos = {};
//	VECTOR rot = {};
//	VECTOR targetPos = {};
//};

// ユーザデータ
struct NET_JOIN_USER
{
	// 通信モード(ホスト or クライアント)
	NET_MODE mode = NET_MODE::NONE;
	// ユーザの任意キー
	int key = -1;
	// ユーザのIPアドレス
	IPDATA ip = { 0, 0, 0, 0 };
	// ユーザのポート
	int port = -1;
	// ユーザ1、2、3、4...
	PLAYER_TYPE playerType = PLAYER_TYPE::PLAYER_1;
	// 全体のゲーム進行状態
	GAME_STATE gameState = GAME_STATE::LOBBY;
	// ゲームモードtest
	int gameMode = 0;
	// チーム
	int team = 0;
	// 名前
	char playerName[MAX_NAME_NUM] = "";
	// 称号
	int alias = 0;
	// 武器
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
	// ユーザの任意キー
	int key = -1;
	// ゲーム進行フレーム
	unsigned int frameNo = 0;
	// アクションキー
	unsigned int actBits = 0;
	// 回転量
	Vector2<float> rotPow_ = {};
	// ランダム
	int randSeed = 0;
	// カメラ情報
	//CAMERA_INFO cameraInfo_ = {};

	void Reset(void) {
		// ゲーム進行フレーム
		frameNo = 0;
		// アクションキー
		actBits = 0;
		// 回転量
		rotPow_ = {};
		// ランダム
		randSeed = 0;
		// カメラ情報
		//cameraInfo_ = {};
	}
};

struct DEAD
{
	// ユーザの任意キー
	int key = -1;
	// ゲーム進行フレーム
	unsigned int frameNo = 0;
	// 死亡状態
	bool isDead = false;
};

struct NET_ACTION_HIS
{
	// ユーザの任意キー
	int key = -1;
	// 過去フレーム分のアクション情報(NUM_FRAME(10)フレーム分)
	NET_ACTION actions[NUM_FRAME];

	bool isReceive = false;
};

struct NET_USERS_ACTION_HIS
{
	// 全員分の過去フレーム分のアクション情報
	NET_ACTION_HIS actionsHistories[MAX_PLAYERS];
};
