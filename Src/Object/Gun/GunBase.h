#pragma once
#include <memory>
#include <vector>
#include "../ObjectBase.h"
#include "../Bullet/BulletBase.h"
#include "../../Net/NetManager.h"

class ActorBase;

class GunBase : public ObjectBase
{

public:

	enum class STATE
	{
		NONE,
		SHOT,
		//SHOT_ADS,
		RELOAD,
	};

	// 最大装填数
	static constexpr int MAX_MAGAZINE_DEF = { 30 };

	// 銃回転にかかる時間
	static constexpr float ROTATE_TIME = { 0.5f };

	// 注視点遷移
	static constexpr float TARGET_CHANGE_TIME = { 1.0f };

	GunBase(const Transform& follow, int frameNo, std::weak_ptr<ActorBase> parent);
	~GunBase(void);

	void UpdateAfterCollision(void) override;

	virtual void DrawTrans(void) override;

	// 位置回転更新
	void UpdateTransform(void);

	// 銃を一発撃つ
	void Shot(void);
	void ShotUpdate(void);

	// 現在のリコイルパターン取得
	const Recoil::RECOIL_TYPE GetRecoilType(void) const;
	const int GetMagazine(void) const;
	const int GetMaxMagazine(void) const;

	// 反動の大きさを取得
	const float GetRecoilMag(void) const;

	// 連続して撃った数をリセット
	void ResetBurstNum(void);

	// 弾数リセット
	void ReloadGun(void);

	// 銃の状態変更
	void ChangeState(STATE state, float time = 0.0f);

	// 回転中かどうか
	bool IsRotate(void) const;

	// 弾数があるかどうか
	bool IsRemainMagazine(void)const;

	// 弾が発射可能かどうか
	bool IsShootable(void)const;

	// 拡大倍率取得
	const float& GetMagnification(void)const;

	virtual void PlayReloadSound(void) = 0;

	const VECTOR& GetBarrelPos(void)const;
	const float& GetRandLength(void)const;
	const float& GetRandRadius(void)const;
	const float& GetShotDelayTime(void)const;
	VECTOR GetFoward(void)const;
	virtual const VECTOR GetScopePos(void)const;

	std::vector<std::shared_ptr<BulletBase>>& GetBullets(void);

protected:

	NetManager& netManager_;

	bool isShot_;

	// 所有元のポインタ
	std::weak_ptr<ActorBase> parent_;

	// エフェクトID
	std::vector<int> effectID_;

	// 所有元のkey(通信で使用)
	int parentKey_;

	// 発射方向をばらけさせる際の始点から終点までの長さ
	float randLength_;
	// 発射方向をばらけさせる際の円の半径
	float randRadius_;

	// 腰撃ち時
	float hipRandLength_;
	float hipRandRadius_;


	// スコープ関連ーーーーーーーーーーーーーーーーーーー

	std::unique_ptr<Model> scopeModel_;
	// スコープのモデルID
	int scopeModelId_;

	// スコープモデルファイル名
	std::string scopeFileName_;

	// スコープ用Transform
	Transform scpTransform_;

	// スコープ相対座標
	VECTOR scpRelativePos_;

	// スコープ拡大倍率
	float magnification_;

	// デバッグ
	VECTOR deg_;

	// ーーーーーーーーーーーーーーーーーーーーーーーーー
	
	// 弾関連ーーーーーーーーーーーーーーーーーーーーーー
	// 弾
	std::vector<std::shared_ptr<BulletBase>> bullets_;

	// 弾のモデルファイル名
	std::string bulletFileName_;

	// 弾のモデルID
	int bulletModelId_;
	// ーーーーーーーーーーーーーーーーーーーーーーーーー

	// 追従対象
	const Transform& follow_;

	// 追従フレーム番号
	int followFrameNo_;

	// 相対座標
	VECTOR relativePos_;

	// モデルファイル名
	std::string fileName_;

	// 銃の状態
	STATE state_;

	STATE preState_;

	// バレルフレーム番号(弾の発射位置)
	int barrelFrameNo_;

	// バレル相対座標
	VECTOR barrelRelativePos_;

	// 弾の発射位置
	VECTOR barrelPos_;

	// 最大弾数
	int maxMagazine_;

	// 発射間隔
	float shotDelay_;
	float shotDelayCnt_;

	// リコイルパターン
	Recoil::RECOIL_TYPE recoil_[MAX_MAGAZINE_DEF];

	// 発射時の反動の大きさ
	float recoilMag_;

	// 連続して撃った数
	int burstCnt_;

	// 弾数
	int magazine_;

	// 回転中かどうか
	bool isRotate_;

	// 回転開始時のqua
	Quaternion startQuaRot_;

	// 回転割合
	float rotateRate_;
	float rotateTime_;
	float rotateTimeCnt_;

	//
	VECTOR targetPos_;
	VECTOR goalTargetPos_;

	// 遷移
	float changeCnt_;
	
	void Init_Object(void) override;
	void Update_Object(void) override;
	void Draw_Object(void) override;
	void Release_Object(void) override;

	// 各自独自の設定
	virtual void SetParam(void) = 0;
	virtual void PlayBurstSound(void) = 0;

	// 銃を対象に向けるかつ追従
	void Follow(void);

	// 通常時の追従(もう一つのフレームを向く)
	virtual void FollowNone(void);

	// 発砲時の追従(注視点を向く)
	virtual void FollowShot(void);

	// リロード中の追従
	virtual void FollowReload(void);

	// 目標の角度に向けて徐々に回転
	void Rotation(Quaternion goalRot);

	// 目標のゴールに向けて徐々に移動
	//void Target(VECTOR goalTargetPos);

	// 発射間隔処理
	void ShotDelay(void);

	// 弾生成
	virtual void CreateBullet(void) = 0;

	// 弾を発射状態に設定
	virtual void SetBullet(std::shared_ptr<BulletBase> bullet);

};
