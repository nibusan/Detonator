#pragma once
#include"GrenadeBase.h"

class GrenadeSmoke final : public GrenadeBase
{
public:

	GrenadeSmoke(const Transform& follow, int frameNo, std::weak_ptr<ActorBase> parent);
	~GrenadeSmoke() = default;

	// 衝突したときの処理
	void OnHit(MV1_COLL_RESULT_POLY info) override;

private:

	// 各自独自の設定
	void SetParam(void) override;

	// 爆発
	void Blast(void) override;

	// それぞれの初期化
	void Reset(void) override;
};

