#pragma once
#include <map>
#include <mutex>
#include "NetStructures.h"

class NetSend;
class NetBase;

class NetManager
{
public:

	// 最大受信回数
	static constexpr int MAX_RECEIVE_NUM = (10);

	// 受信待機時間(ミリ秒)
	static constexpr float RECEIVE_TERM_MSEC = (1.0f / 120.0f * 1000.0f);

	// 同期待機時間(ミリ秒)
	static constexpr float SYNC_TERM_MSEC = (1.0f / 120.0f * 1000.0f);

	// 
	static constexpr float RECEIVE_TIME = (5.0f);

	struct NET_POOL
	{

		// ホストのネットユーザ情報
		NET_JOIN_USER hostJoinUser_;

		// 自分自身のネットユーザ情報
		NET_JOIN_USER selfJoinUser_;

		// 自分自身のアクション情報
		NET_ACTION selfAction_;

		// 自分自身のアクション情報履歴
		NET_ACTION_HIS selfActionHis_;

		// 自分自身の死亡情報
		DEAD selfDead_;

		// 接続相手(自分自身も含む)(任意のキーで判別)
		std::map<int, NET_JOIN_USER> joinUsers_;

		// 接続相手(自分自身も含む)のアクション情報履歴(任意のキーで判別)
		std::map<int, NET_ACTION_HIS> joinUserActionHis_;

		// 接続相手(自分自身も含む)の死亡情報(任意のキーで判別)
		std::map<int, DEAD> joinUserDeath_;



	};

	// インスタンスの生成
	static void CreateInstance(void);

	// インスタンスの取得
	static NetManager& GetInstance(void);

	void Init(void);

	// リソースの破棄
	void Destroy(void);

	// ホストのIPアドレス
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

	// モード取得
	NET_MODE GetMode(void) const;

	// ホストモードか判断
	bool IsHost(void) const;
	bool IsRunning(void) const;

	// 通信開始
	void Run(NET_MODE mode);

	// 更新処理
	void Update(void);
	void DataReflection(void);
	void UpdateEndOfFrame(void);

	// 時間経過
	void UpdateTime(void);

	// 次のフレームへ
	void NextFrame(void);

	// 通信終了
	void Stop(void);

	// 通信を送る
	void Send(NET_DATA_TYPE type);

	// 状態遷移
	void ChangeGameState(GAME_STATE state);

#pragma region 専用プール

	// 自分のネット情報を取得
	const NET_JOIN_USER GetSelf(void) const;

	// 接続相手を取得
	const std::map<int, NET_JOIN_USER> GetNetUsers(void) const;
	const std::map<int, NET_ACTION_HIS> GetNetUserActionHis(void) const;

	// ホストのゲーム進行状況
	GAME_STATE GetGameStateHost(void) const;

	// 自分自身のゲーム進行状況
	GAME_STATE GetGameStateSelf(void) const;

	// 全ての接続相手が指定されたゲーム進行状態か判断
	bool IsSameGameState(GAME_STATE state);

	// 自分のアクション情報を取得
	const NET_ACTION GetSelfAction(void) const;

	const DEAD GetSelfDead(void) const;

	// 接続相手のアクション情報を取得
	const std::map<int, NET_ACTION_HIS> GetActionHis(void) const;

	const NET_JOIN_USER& GetHostJoinUser(void)const;

	// アクション判定
	bool IsAction(int key, PLAYER_ACTION action);

	// アクション判定
	bool IsAction(int key, PLAYER_ACTION action, bool isAction);
	Vector2<float> GetRotatePow(int key, Vector2<float> pow);
	int GetRandSeed(int key, int seed);
	//CAMERA_INFO GetCameraInfo(int key, CAMERA_INFO info);
	//VECTOR GetTargetPos(int key, VECTOR pos);
	//VECTOR GetBulletDir(int key, VECTOR vec);
	//Transform GetTransform(int key, Transform transform);
	//VECTOR GetFrameRot(int key, VECTOR rot);
	//VECTOR GetCameraRot(int key, VECTOR rot);

	// アクション設定
	void SetAction(PLAYER_ACTION act);
	void SetRotPow(Vector2<float> pow);
	void SetRandSeed(int seed);
	//void SetCameraInfo(CAMERA_INFO info);
	//void SetTargetPos(VECTOR pos);
	//void SetBulletDir(VECTOR vec);
	//void SetTransform(Transform transform);
	//void SetFrameRot(VECTOR rot);
	//void SetCameraRot(VECTOR rot);

	// アクション設定
	void SetGameMode(int mode);

	// 全ての接続相手が指定されたゲームカウントか判断
	bool IsSameFrameNo(void);

	// 全ユーザからデータを受け取ったか
	bool IsReceiveAll(void);
	void ResetReceiveAll(void);

	// 自分のアクション情報履歴
	const NET_ACTION_HIS GetSelfActionHis(void) const;



#pragma endregion

	// ホストのポート番号取得
	int GetHostPort(void) const;

	// ※※※アクション設定
	void ResetAction(void);

	unsigned int GetFrameNo(void) const;

	//void MakeActionHis(int frameNo, NET_ACTION action);
	void MakeActionHis(NET_ACTION action);

	bool IsSync(void);

	bool HostIsNotFound(void);

private:

	const int RECEIVE_PORT_HOST = 65000;
	const int RECEIVE_PORT_CLIENT_RANGE = RECEIVE_PORT_HOST + 100;

	// 通信がくるまでの時間
	// 一定期間通信がなければユーザーから消す
	std::map<int, float> joinUserTime_;
	float hostTime_;

	// ホストが見つからなかった場合true
	bool hostIsNotFound_;

	// 静的インスタンス
	static NetManager* instance_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	NetManager(void);

	// コピーコンストラクタも同様
	NetManager(const NetManager&);

	// デストラクタも同様
	~NetManager(void) = default;

	// 通信開始フラグ
	bool isRunning_;

	// 同期フラグ(ゲーム中のみtrue)
	bool isSync_;

	// ソケットハンドル
	int sendSocketId_;
	int recvSocketId_;

	std::unique_ptr<NetBase> net_;
	std::unique_ptr<NetSend> netSend_;

	// 役割(ホスト、クライアント)
	NET_MODE mode_;

	// ホストのIPアドレス
	IPDATA hostIp_;

	// スレッド
	std::thread recvThread_;

	// 送受信データの排他制御
	// 排他的⇒部外者を排斥して退ける
	// poolShare_用
	std::mutex poolMutex_;

	// 送受信時のデータ同期用
	// 排他制御外では絶対に読み書きしないこと
	NET_POOL poolShare_;

	// 排他制御外で使用
	NET_POOL pool_;

	int frameNo_;

	bool isFirstReceive_;

	// 通信種別ごとの更新処理
	void UpdateState(void);

	// 受信スレッド
	void UdpReceiveThread(void);

	// データ受信処理
	void UdpReceiveData(void);

	// 送受信用の基本データをバッファから取得する
	NET_BASIC_DATA GetBasicData(const char* bufptr);

#pragma region 送受信データ書き込み

	// 接続相手を追加・変更
	void ReplaceUser(NET_JOIN_USER entity);

	// 接続相手を設定(基本的にクライアント側の処理)
	// ってわけでもないかも
	void ReplaceUsers(NET_JOIN_USERS entities);

	// アクション情報を追加・変更
	void ReplaceActionHis(NET_ACTION_HIS entity);

#pragma endregion

};

