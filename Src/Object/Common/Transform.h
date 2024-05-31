#pragma once
#include <DxLib.h>
#include "../../Common/Quaternion.h"

/// <summary>
/// モデル制御の基本情報
/// 大きさ：VECTOR基準
/// 回転　：Quaternion基準
/// 位置　：VECTOR基準
/// </summary>
class Transform
{

public:

	// コンストラクタ
	Transform(void);
	//Object(int model);				// モデル
	//Object(float rad);				// 球体
	//Object(float rad,float len);	// カプセル

	// デストラクタ
	~Transform(void);



	/// <summary>
	/// モデル制御の基本情報更新
	/// </summary>
	/// <param name=""></param>
	void Update(void);

	void Draw(void);

	void Release(void);

	void SetModel(int modelHId);

	// 1フレーム前更新
	// 更新前座標等を使う場合
	// この関数を各更新処理の1番最初に
	// 1度だけ呼び出す
	void PreUpdate(void);


	// 前方方向を取得
	VECTOR GetForward(void) const;

	// 後方方向を取得
	VECTOR GetBack(void) const;

	// 右方向を取得
	VECTOR GetRight(void) const;

	// 左方向を取得
	VECTOR GetLeft(void) const;

	// 上方向を取得
	VECTOR GetUp(void) const;

	// 下方向を取得
	VECTOR GetDown(void) const;

	// 対象方向を取得
	VECTOR GetDir(const VECTOR& vec) const;

	// モデルのハンドルID
	int modelId;

	// 大きさ
	VECTOR scl;
	// 回転
	VECTOR rot;
	// 位置
	VECTOR pos;
	VECTOR localPos;

	MATRIX matScl;
	MATRIX matRot;
	MATRIX matPos;

	// 回転
	Quaternion quaRot;

	// ローカル回転
	Quaternion quaRotLocal;

	// 更新前要素ーーーーーーーーーーーーーーーーーーーーー

	// 大きさ
	VECTOR preScl;
	// 回転
	VECTOR preRot;
	// 位置
	VECTOR prePos;

	// 回転
	Quaternion preQuaRot;

	// 追加分ーーーーーーーーーーーーーーーーーーーーーーー

	// 非表示にするか
	bool isHide;

	// 重力をONにするか
	bool isGravity;

	// 重力係数
	float heavy;

	// 高さ(身長)
	float height;

	// 指定のフレームを非表示にする(大きさを0にする)
	void HideFrame(int frameNo);

	// 指定のフレームを再表示する(大きさを1にする)
	void AppearFrame(int frameNo);

	// 指定のフレームが非表示かどうか
	const bool IsHidingFrame(int frameNo);

	// 重力処理
	void Gravity(void);

	// 重力加速度をリセット
	void ResetGravityAccel(void);

	// 重力加速度をゲット
	const float& GetGravityAccel(void) const;

	// ジャンプさせる
	void Jump(float pow);

	// 上昇中か否か
	bool IsRising(void);

	// 下降中か否か
	bool IsFalling(void);

private:

	const float GRAVITY = (9.8f);

	// 重力加速度
	float gravitySpeed_;
};

