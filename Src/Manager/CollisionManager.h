#pragma once
#include <unordered_map>
#include "../Object/Common/Collider.h"

class CollisionManager
{
public:

	// マップ側面と行う衝突判定回数
	static constexpr int MAP_COL_MAX = (30);

	// インスタンスの生成
	static void CreateInstance(void);

	// インスタンスの取得
	static CollisionManager& GetInstance(void);

	void Init(void);

	// リソースの破棄
	void Destroy(void);

	// コライダー配列クリア
	void ClearColliders(void);

	// コライダー追加
	void SetCollider(std::weak_ptr<Collider> col);

	// 親がいなくなったコライダーを自動的に削除
	void DeleteColliderNonParent(void);

	// 特定のコライダーを削除
	void DeleteCollider(std::weak_ptr<Collider> col);

	// 衝突判定(シングルプレイ用)
	void CollisionSingle(void);
	// 衝突判定(マルチプレイ用)
	void CollisionMulti(void);

	// プレイヤーとマップーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
	// 今現在4回分余分にマップとの衝突判定を行っているので修正予定
	// ①プレイヤーとマップの衝突判定で衝突ポリゴン配列を取得
	// ②衝突ポリゴン配列を法線ベクトルごとにわける
	// ③各処理
	void CollisionPlayerMap(void);

	// プレイヤーとマップ(壁)
	void CollisionPlayerMapSide(const VECTOR movePow, std::weak_ptr<Collider> player, int no);
	void CollisionPlayerMapSide2(const VECTOR movePow, std::weak_ptr<Collider> player, int no);

	// プレイヤーとマップ(天井)
	void CollisionPlayerMapHead(std::weak_ptr<Collider> player, int no);

	// プレイヤーとマップ(地面)
	void CollisionPlayerMapFoot(std::weak_ptr<Collider> player, int no);

	// プレイヤーとマップ(斜め)
	void CollisionPlayerMapSlopeUp(std::weak_ptr<Collider> player, int no);	// 法線ベクトル↑
	void CollisionPlayerMapSlopeDown(std::weak_ptr<Collider> player, int no);	// 法線ベクトル↓

	// ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー


	// プレイヤーとプレイヤーの弾
	void CollisionPlayerBullet(void);

	// プレイヤーとエネミーの弾
	void CollisionPlayerEnemyBullet(void);

	// プレイヤーの弾とエネミー
	void CollisionPlayerBulletEnemy(void);

	// プレイヤーの弾とマップ
	void CollisionPlayerBulletMap(void);

	// プレイヤーの弾とシールド
	void CollisionPlayerBulletShield(void);

	// エネミーとマップ
	void CollisionEnemyMap(void);

	// エネミーの弾とマップ
	void CollisionEnemyBulletMap(void);

	// エネミーの目線とマップ
	void CollisionEnemyEyeMap(void);
	bool IsCollisionEnemyEyeMap(std::weak_ptr<Collider> obj);

	// カメラとマップ
	void CollisionCameraMap(void);

	// グレネードとマップ
	void CollisionGrenadeMap(void);

	// 爆発とマップの衝突
	void CollisionExplosionMap(void);

	// 爆発とプレイヤー
	void CollisionExplosionPlayer(void);
	void CollisionSmokePlayer(void);

	// 爆発とエネミー
	void CollisionExplosionEnemy(void);

	// プレイヤーの弾とシールド
	void CollisionExplosionShield(void);

	// 射線とマップの衝突判定
	bool CollisionRayMap(std::weak_ptr<Collider> obj);


	// 計算後の値を返すーーーーーーーーーーーーーーーーーーーーー

	// プレイヤーから視点方向にレイを飛ばし、
	// 衝突したオブジェクトの中から最も近い点を返す
	// これが銃の注視点になる
	VECTOR GetGunImpactPos(void);


	// ーーーーーーーーーーーーーーーーーーーーーーーーーーーーー	




private:

	// 静的インスタンス
	static CollisionManager* instance_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	CollisionManager(void);

	// コピーコンストラクタも同様
	CollisionManager(const CollisionManager&);

	// デストラクタも同様
	~CollisionManager(void) = default;

	// weakでいいかも
	// 衝突判定コライダー
	std::unordered_map<Collider::OBJECT_TYPE, std::vector<std::weak_ptr<Collider>>> colliders_;

};

