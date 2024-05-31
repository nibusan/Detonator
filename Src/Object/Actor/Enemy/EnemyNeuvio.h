#pragma once
#include <unordered_map>
#include "../../Bullet/BulletBase.h"
#include "../../../Manager/AudioManager.h"
#include "EnemyBase.h"

class EnemyNeuvio : public EnemyBase
{

public:

	// 敵機が通る中継地点
	enum class POINT
	{
		A,
		B,
		C,
	};

	// 敵の行動
	enum class ACTION
	{
		FIXED_TURRET,	// 固定砲台
		FOLLOW,			// 追尾
		PASS,			// スルー
		RUSH,			// 突進
	};

	// 視野の広さ
	static constexpr float VIEW_RANGE = (1300.0f);

	// 視野角
	static constexpr float VIEW_ANGLE = (30.0f);

	// 聴覚の広さ
	static constexpr float HEARING_RANGE = (1300.0f);

	// 敵の速度
	static constexpr float ENEMY_SPEED = 3.0f;

	// 敵の高さ
	static constexpr float ENEMY_HIGH = 500.0f;

	// 中継地点の半径
	static constexpr float POINT_RADIUS = 10.0f;

	// 回転量(deg)
	static constexpr float SPEED_ROT_ENEMY = 3.0f;

	// 回転量(rad)
	static constexpr float SPEED_ROT_RAD_ENEMY = SPEED_ROT_ENEMY * (DX_PI_F / 180.0f);

	// プレイヤーとの距離
	static constexpr float PLAYER_DISTANCE = 700.0f;

	// 当たり判定用半径
	static constexpr float COL_RAD = 60.0f;

	// 当たり判定用相対座標
	static constexpr VECTOR LOCAL_COL_POS = {0.0f,50.0f,-20.0f};

	// 弾発射用相対座標
	static constexpr VECTOR LOCAL_SHOT_POS = { 0.0f,-50.0f,0.0f };

	// 敵機が通る中継地点
	static constexpr VECTOR POINT_A = { 0.0f,500.0f,0.0f };
	static constexpr VECTOR POINT_B = { 500.0f,500.0f,500.0f };
	static constexpr VECTOR POINT_C = { -500.0f,500.0f,500.0f };

	// 壁に衝突した時の回避ポイント
	static constexpr VECTOR POINT_AVOID = { 0.0f,300.0f,0.0f };

	// 敵ごとのスコア
	static constexpr int FIXED_TURRET_SCORE = 2000;
	static constexpr int FOLLOW_SCORE = 3000;
	static constexpr int PASS_SCORE = 500;
	static constexpr int RUSH_SCORE = 2000;

	// 敵リスポーン制御
	static constexpr float F_ENEMY_RESPAWN_POS_X = 2400.0f;
	static constexpr float B_ENEMY_RESPAWN_POS_X = -1900.0f;
	static constexpr float F_ENEMY_RESPAWN_POS_Y = 400.0f;
	static constexpr float B_ENEMY_RESPAWN_POS_Y = 100.0f;
	static constexpr float F_ENEMY_RESPAWN_POS_Z = 5000.0f;
	static constexpr float B_ENEMY_RESPAWN_POS_Z = -2300.0f;

	EnemyNeuvio(std::vector<VECTOR*> followPos,bool isFirst);
	~EnemyNeuvio(void);

	// 衝突したときの処理
	void OnHit(std::weak_ptr<Collider> obj) override;
	void OnHit(MV1_COLL_RESULT_POLY info) override;

	// 敵リセット時の初期化
	void EnemyReset(void);


	 
	
	//void CreateEnemy(std::shared_ptr<ObjectBase> enemys);

private:

	AudioManager& audioManager_;

	// 弾
	std::vector<std::shared_ptr<BulletBase>> bullets_;

	std::unordered_map<POINT, VECTOR> points_;

	std::shared_ptr<Collider> collider_;

	std::shared_ptr<Collider> colliderEye_;

	int soundHandle_;

	// 敵機が通る中継地点
	POINT point_;

	// 敵の行動
	ACTION action_;

	// 追従対象
	std::vector<VECTOR*> followPos_;
	VECTOR followRushPos_;

	// 最初かどうか
	bool isFirst_;
	bool isFirstCol_;

	// 敵機が通る中継地点
	VECTOR pointA_;
	VECTOR pointB_;
	VECTOR pointC_;

	// 突進中の向き
	VECTOR rushDir_;

	// アニメーション番号
	int animAttachNo_;

	// 弾の発射位置
	VECTOR barrelPos_;

	// 弾のモデルID
	int bulletModelId_;

	// バレルフレーム番号(弾の発射位置)
	int barrelFrameNo_;

	// 発射間隔
	float shotDelay_;
	float shotDelayCnt_;

	float deg_;

	// 回転中かどうか
	bool isRotate_;

	// 検知フラグ
	bool isNotice_;
	bool isNoticeHearing_;
	bool isNoticeBack_;

	// 追従フラグ
	bool isFollow_;

	// 突進フラグ
	bool isRush_;

	// 当たり判定フラグ
	bool isCollision_;

	// デバッグ用
	bool debugFlg_;

	void Init_Enemy(void)override;
	void Update_Enemy(void)override;
	void Draw_Enemy(void)override;
	void Release_Enemy(void)override;

	// 移動処理
	void ProcessMove(void);
	void GoToPoint(VECTOR start,VECTOR end,POINT point);

	// 回転処理
	void Rotation(VECTOR end);
	void LazyRotation(float goalRotY, float goalRotX);

	// 線形補間
	VECTOR Lerp(VECTOR start, VECTOR end, float speed);

	// 行動処理
	void EnemyAction(void);
	void GoToPoint(void);

	// モーション
	void Motion(void);

	// 追尾処理
	void Follow(void);

	// サーチ
	void Search(void);

	// 突進
	void Rush(void);

	// 当たり判定制御
	void SyncCollision(void);

	// 弾生成
	void CreateBullet(void);

	// 弾を発射状態に設定
	void SetBullet(std::shared_ptr<BulletBase> bullet);

	// 発射間隔処理
	void ShotDelay(void);

	// 弾発射処理
	void ProcessShot(void);

	// 死亡時処理
	void ProcessDead(void)override;

	// ベクトル乗算
	VECTOR VMult(VECTOR vec1, VECTOR vec2);

	// デバッグ
	void DrawDebug(void);

};

