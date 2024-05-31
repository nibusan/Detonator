#pragma once
#include "GunBase.h"

class GunAK47 : public GunBase
{
public:

	static constexpr VECTOR RELATIVE_POS = { 0.0f,-2.0f,22.0f };

	//// 発射方向をばらけさせる際の始点から終点までの長さ
	//static constexpr float StoE_LENGTH = (30.0f);

	//// 発射方向をばらけさせる際の円の半径
	//static constexpr float RADIUS = (10.0f);

	GunAK47(const Transform& follow, int frameNo, std::weak_ptr<ActorBase> parent);
	~GunAK47(void) = default;

	// 弾生成
	void CreateBullet(void) override;

	// スコープの位置取得
	const VECTOR GetScopePos()const override;

	void PlayReloadSound(void) override;

private:

	// 各自独自の設定
	void SetParam(void) override;
	void PlayBurstSound(void) override;

	//// 弾を発射状態に設定
	//void SetBullet(std::shared_ptr<BulletBase> bullet)override;

};

