#pragma once
#include "GunBase.h"

class GunEnemy : public GunBase
{
public:

	static constexpr VECTOR RELATIVE_POS = { 0.0f,1.0f,18.0f };

	//// 発射方向をばらけさせる際の始点から終点までの長さ
	//static constexpr float StoE_LENGTH = (30.0f);

	//// 発射方向をばらけさせる際の円の半径
	//static constexpr float RADIUS = (10.0f);

	GunEnemy(const Transform& follow, int frameNo, std::weak_ptr<ActorBase> parent);
	~GunEnemy(void) = default;

	// 弾生成
	void CreateBullet(void) override;

	void PlayReloadSound(void) override;

private:

	// 各自独自の設定
	void SetParam(void) override;
	void PlayBurstSound(void) override;

	// 通常時の追従(もう一つのフレームを向く)
	void FollowNone(void) override;

	// 発砲時の追従(注視点を向く)
	void FollowShot(void) override;

	// リロード中の追従
	void FollowReload(void) override;

	// 弾を発射状態に設定
	void SetBullet(std::shared_ptr<BulletBase> bullet) override;

};

