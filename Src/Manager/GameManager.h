#pragma once
#include <string>
#include "LoadOut.h"
#include "../Object/ObjectID.h"
#include "../UI/Parts/Game/LogMessageArea/UI_LogMessageArea.h"
#include "../Common/Handle/Font/Font.h"
#include "../Common/Count.h"

class NetManager;
class ObjectBase;
class EnemyCreator;

class GameManager
{

public:

	// シーンごとのタイプ
	enum class STATE
	{
		NONE,
		sLOBBY,
		mLOBBY,
		sGAME,
		mGAME,
	};

	// ゲームモード
	enum class MODE
	{
		NONE,
		TIMEATTACK,
		SCOREATTACK,
		HOST,
		CLIENT,
		SETTINGS,
	};

	enum class TEAM
	{
		WOLF,
		EAGLE,
		MAX
	};

	// プレイヤー称号
	enum class ALIAS
	{
		NONE,
		MVP,
		KILLER,
		LASTKILL,
		BOMMER,
		UNDEAD,
		HITMAN,
		COMEDIAN,
	};

	struct PLAYER_INFO
	{
		LOADOUT loadOut = { GUN_TYPE::AK47,GRENADE_TYPE::HAND,GRENADE_TYPE::SMOKE };
		int kill = 0;
		int death = 0;
		int bom = 0;
		int headShot = 0;
		int score = 0;
		int magazine = 0;
		TEAM team = TEAM::WOLF;
		GRENADE_TYPE greType = GRENADE_TYPE::HAND;
		int greNum = 0;
		float moveControlPosZ = 0.0f;
		float actionControlPosZ = 0.0f;
		std::string playerName = "プレイヤー";
		ALIAS alias = ALIAS::NONE;
		VECTOR respawnPos = {};
		VECTOR respawnRot = {};
		bool isHide = false;
		void Reset() {
			kill = 0;
			death = 0;
			bom = 0;
			headShot = 0;
			score = 0;
			magazine = 0;
			moveControlPosZ = 0.0f;
			actionControlPosZ = 0.0f;
			playerName = "プレイヤー";
			alias = ALIAS::NONE;
			isHide = false;
		}
		void GameReset() {
			kill = 0;
			death = 0;
			bom = 0;
			headShot = 0;
			score = 0;
			magazine = 0;
			alias = ALIAS::NONE;
			isHide = false;
		}
	};

	// 与えたダメージ
	struct DamageData {
		int damage_;
		VECTOR pos;
		float time_;
		VECTOR color_;
	};

	static constexpr int MAX_PLAYER = (4);

	//エフェクトの最大数
	static constexpr int EFFECT_MAX_NUM = 8000;

	// タイムアップ時間
	static constexpr float GAME_TIME_SCOREATTACK = (60.0f);
	static constexpr float GAME_TIME_TDM = (180.0f);

	// ゲーム開始までのカウントダウン時間
	static constexpr float TIME_TO_START = 5.0f;

	// ゲーム終了待機時間
	static constexpr float TIME_TO_END = 1.0f;

	// TDM目標スコア
	static constexpr int TDM_GOAL_SCORE[3] = { 5,8,12 };

	// 称号
	static constexpr int INFO_MAX = 5;

	// リスポーン位置
	static constexpr VECTOR SINGLE_RESPAWN_POS = { -1500.0f,0.0f,-3100.0f };
	static constexpr VECTOR MULTI_RESPAWN_POS_WOLF[2] = { { -1500.0f,0.0f,-3100.0f },{ 800.0f,0.0f,-3100.0f } };
	static constexpr VECTOR MULTI_RESPAWN_POS_EAGLE[2] = { { 800.0f,0.0f,2600.0f },{ -1500.0f,0.0f,2600.0f} };

	// リスポーン方向
	static constexpr VECTOR SINGLE_RESPAWN_ROT = { 0.0f,DX_PI_F * 45.0f / 180.0f,0.0f };
	static constexpr VECTOR MULTI_RESPAWN_ROT_WOLF[2] = { { 0.0f,DX_PI_F * 45.0f / 180.0f,0.0f },
														  { 0.0f,DX_PI_F * -45.0f / 180.0f,0.0f } };
	static constexpr VECTOR MULTI_RESPAWN_ROT_EAGLE[2] = { { 0.0f,DX_PI_F * -135.0f / 180.0f,0.0f },
														   { 0.0f,DX_PI_F * 135.0f / 180.0f,0.0f } };

	// TDM時位置制御
	static constexpr float WOLF_CONTROL_POS_Z = 1980.0f;
	static constexpr float EAGLE_CONTROL_POS_Z = -2470.0f;

	// インスタンスの生成
	static void CreateInstance(void);

	// インスタンスの取得
	static GameManager& GetInstance(void);

	void Init(void);
	void Update(void);
	void NetUpdate(void);

	// リソースの破棄
	void Destroy(void);

	const float GetDPI(void) const;
	void SetDPI(float dpi);

	// スコア
	const int GetScore(int no)const;
	void AddScore(unsigned int score, int no);

	// ボム
	void AddBom(int no);

	// ヘッドショット
	void AddHeadShot(int no);

	// 死亡
	void AddDead(int no);
	int GetDead(int no);

	// 弾数
	void SetMagazine(int no, int magazine);
	int GetMagazine(int no);

	// キル数取得
	const int GetKill(int no)const;

	// グレネード
	const GRENADE_TYPE GetGrenadeType(int no,int slot)const;

	// グレネード数
	int GetGrenadeNum(int no) const;
	void SetGrenadeNum(int no,int num);

	void SetName(int no, std::string name);

	const std::string GetPlayerName(int no)const;
	const std::string GetName(ID id)const;
	const TEAM GetTeam(int no)const;

	const bool IsHide(int no)const;
	void SetIsHide(int no , bool isHide);

	const GRENADE_TYPE GetNowGreType(int no)const;
	void SetGreType(int no, GRENADE_TYPE type);
	
	// 残り時間取得
	const float GetGameTime(void)const;

	// かかった時間取得
	const float GetAttackTime(void)const;

	// ゲーム開始時間取得
	const float GetTimeToStart(void)const;

	/// <summary>
	/// キル数増加
	/// </summary>
	/// <param name="die">死んだやつ</param>
	/// <param name="killed">殺したやつ</param>
	void Kill(ID die, ID killed);

	// 殺してきたやつの名前
	std::string GetKillerName(void);
	std::string GetDieName(void);

	// 撃破数
	int GetEliminationNum(ID id);
	void AddElimination(ID id);

	// シーン取得
	STATE GetState(void);

	// シーンの変更
	void ChangeState(STATE state);

	// ゲームモード取得
	MODE GetMode(void);

	// ゲームモードの変更
	void ChangeMode(MODE mode);

	// 武器タイプ取得
	void SetLoadOut(int no, LOADOUT loadOut);
	LOADOUT GetLoadOut(int no);

	void SetTeam(int no, TEAM team);

	// マルチのプレイヤー情報取得
	PLAYER_INFO& GetPlayerInfo(int no);

	// 勝利チーム取得
	const TEAM& GetWinner(void)const;

	const bool WinPlayer(void)const;

	const int& GetTDMScore(TEAM team)const;
	int GetGoalTDMScore(void)const;

	// マルチのプレイヤー情報変更
	void ChangePlayerInfo(PLAYER_INFO info, int no);

	// チームが均等に分かれているかどうか
	bool IsDivideTeam(void);

	// ゲームが継続可能かどうか
	void SustainableGame(void);

	// ゲームスタートしたかどうか
	bool IsStart(void);

	bool IsEnd(void);

	bool IsEndGame(void);

	IPDATA GetHostIP(void);
	void SetHostIP(IPDATA ip);

	std::string GetName(void);
	void SetName(std::string name);

	void ResetPlayerInfo(int no);

	// デバッグ
	void DrawDebug(void);

	void SetEnemyFollow(std::weak_ptr<ObjectBase> follow);
	void DrawEnemy(void);

	void UpdateEnemy();

	bool IsActive(void);
	void SetActive(bool isActive);

	// 解放
	void ReleseEnemy(void);

	void Back(void);

	void AddDamageData(DamageData data);
	std::vector<DamageData>& GetDamageData(void);

	// ログメッセージを格納
	void SetNormalLogMessage(const std::string& message, unsigned int color);
	void SetKillLogMessage(const std::string& by, unsigned int by_color, const std::string& killed, unsigned int killed_color);

	// ログメッセージを取得 & リセット
	const std::shared_ptr<UI_LogMessageArea::MESSAGE_DATA_BASE> GetLogMessageData(void);

	void DrawKillMessage(void);
	void DrawHelpPessage(void);

private:

	int imgHelp_;
	int imgHelpClose_;
	bool helpActive_;

	bool isActive_;

	// 静的インスタンス
	static GameManager* instance_;

	NetManager& netManager_;

	std::weak_ptr<ObjectBase> enemyFollow_;
	std::shared_ptr<EnemyCreator> enemyCreator_;

	std::vector<std::shared_ptr<ObjectBase>> enemys_;

	int remainingTime_;			//ゲームの残り時間
	//PLAYER_INFO winnerPlayer_;	//勝利したプレイヤーの情報

	// プレイヤー情報
	PLAYER_INFO playerInfo_[MAX_PLAYER];

	// シーンごとのタイプ
	STATE state_;

	// ゲームモード
	MODE mode_;

	// マウス感度
	float dpi_;

	//// 武器のタイプ
	//LoadOut loadOut_;

	// 最後にキルしたID
	//ID ID_;

	//// スコア
	//int score_;

	//// キル数
	//int kill_;

	// 与えたダメージのデータ
	std::vector<DamageData> damages_;

	// タイムアップ時間
	float gameTime_;

	// 敵を倒しきるまでの時間
	float attackTime_;

	// ゲーム開始までの時間
	float timeToStart_;
	float timeToEnd_;

	// TDM目標スコア
	int goalScore_;

	// TEAMスコア
	int tdmScore_[(int)TEAM::MAX];

	TEAM winTeam_;

	// 最後にキルした人
	int finishKill_;

	// 複数称号持ち
	int cnt_[MAX_PLAYER];
	int idx_;

	bool isChanging_;

	// 撃破数
	int eliminationNum_[(int)ID::ENEMY_GEORGE + 1];

	// 保存用ホストIPアドレス
	IPDATA hostIp_;
	std::string name_;

	// ゲーム中に出てくるキルログなどを一時的に格納
	std::shared_ptr<UI_LogMessageArea::MESSAGE_DATA_BASE> logMessageData_;

	//　キルしてきたやつのID
	ID killerID_;
	//　キルしたやつのID
	ID dieID_;

	Count killMessage_;
	std::unique_ptr<Font> fontKill_;
	std::unique_ptr<Font> fontDPI_;
	int killAlpha_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	GameManager(void);

	// コピーコンストラクタも同様
	GameManager(const GameManager&);

	// デストラクタも同様
	~GameManager(void);

	// 初期化
	// シングルロビー
	void sLobbyInit(void);

	// マルチロビー
	void mLobbyInit(void);

	// シングルゲーム
	void sGameInit(void);

	// マルチゲーム
	void mGameInit(void);

	// 称号付与
	void Alias(void);
	void SetAlias(int idx, int no);
	void SetMVP(void);


	void ChangeDPI(void);

	// 更新処理
	// シングルロビー
	void sLobbyUpdate(void);

	// マルチロビー
	void mLobbyUpdate(void);

	// シングルゲーム
	void sGameUpdate(void);

	// マルチゲーム
	void mGameUpdate(void);



};