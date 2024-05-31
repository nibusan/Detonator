#pragma once
#include <DxLib.h>
#include <vector>
#include "../../Common/Quaternion.h"

class Transform;
class ObjectBase;

class Collider
{

public:

	// 衝突判定種類
	enum class OBJECT_TYPE
	{
		PLAYER_CAP,		// マップとの衝突に使用
		PLAYER_TOPLINE, // マップとの衝突に使用(頭上)
		PLAYER_DOWNLINE,// マップとの衝突に使用(足元)
		PLAYER_HEAD,	// プレイヤーの頭
		PLAYER_BODY, 	// プレイヤーの体
		PLAYER_LIMBS, 	// プレイヤーの手足
		PLAYER_BULLET, 	// プレイヤーの弾
		PLAYER_EYE, 	// プレイヤーの視線(カメラ→注視点)
		ENEMY,			// 敵
		ENEMY_EYE,		// 敵の視線
		ENEMY_BULLET,	// 敵の弾
		MAP,			// マップ(オブジェクト)
		MAP_UNBREAK,	// マップ(破壊不可-壁地面)
		GRENADE_SPHERE,	// グレネード(球体)
		GRENADE_LINE,	// グレネード(線分)
		EXPLOSION,		// 爆発
		SMOKE,			// スモーク
		CAMERA,			// カメラ
		SHIELD,			// シールド
		RAY,			// 射線
	};

	// 衝突タイプ
	enum class COL_TYPE
	{
		NONE,
		MODEL,		// モデル
		SPHERE,		// 球
		CAPSULE,	// カプセル
		LINE,		// 線分
		MAX
	};


	Collider(std::weak_ptr<ObjectBase> obj);
	~Collider(void);

	// 位置更新等
	virtual void Update(VECTOR followPos, Quaternion rot);
	virtual void Update(VECTOR followPos, VECTOR rot);

	// 1フレーム前更新
	// 更新前座標等を使う場合
	// この関数を各更新処理の1番最初に
	// 1度だけ呼び出す
	virtual void PreUpdate(void);

	// オブジェクトタイプを変更
	void ChangeObjectType(OBJECT_TYPE type);

	// 衝突していたオブジェクト配列を初期化(毎フレーム呼び出す)
	void ClearHitObject(void);

	// デバッグ用描画
	virtual void DrawDebug(void);

	// 衝突判定を行うか
	bool isCollision;

	// オブジェクトの種類
	OBJECT_TYPE objType;

	// 衝突タイプ
	COL_TYPE colType;

	// 座標
	VECTOR pos;

	// 相対座標
	VECTOR relativePos;

	// 回転
	Quaternion quaRot;

	// 大きさ
	VECTOR scl;

	// 更新前要素ーーーーーーーーーーーーーーーーーーーーー

	// 大きさ
	VECTOR preScl;
	// 回転
	Quaternion preQuaRot;
	// 位置
	VECTOR prePos;

	// ーーーーーーーーーーーーーーーーーーーーーーーーーー

	// 親のポインタ
	std::weak_ptr<ObjectBase> parent_;

};

