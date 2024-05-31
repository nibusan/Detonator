#pragma once
#include "Collider.h"

class ColliderCapsule : public Collider
{
public:

	// カプセル
	ColliderCapsule(std::weak_ptr<ObjectBase> obj,
		Collider::OBJECT_TYPE type, float radius, float length);
	~ColliderCapsule(void) = default;

	// 位置更新等
	void Update(VECTOR followPos, Quaternion rot) override;
	void Update(VECTOR followPos, VECTOR rot) override;

	// 1フレーム前更新
	// 更新前座標等を使う場合
	// この関数を各更新処理の1番最初に
	// 1度だけ呼び出す
	void PreUpdate(void) override;

	// デバッグ用描画
	void DrawDebug(void) override;

	// 半径取得
	float GetRadius(void) const;

	// 点A取得
	VECTOR GetPosA(void) const;

	// 点B取得
	VECTOR GetPosB(void) const;

	// 半径取得
	float GetPreRadius(void) const;

	// 点A取得
	VECTOR GetPrePosA(void) const;

	// 点B取得
	VECTOR GetPrePosB(void) const;

private:

	// 半径
	float radius_;

	// 長さ
	float length_;

	// カプセル用座標
	VECTOR capPosA_;
	VECTOR capPosB_;

	// 半径
	float preRadius_;

	// 長さ
	float preLength_;

	// カプセル用座標
	VECTOR preCapPosA_;
	VECTOR preCapPosB_;

};

