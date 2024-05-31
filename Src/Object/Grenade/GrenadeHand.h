#pragma once
#include"../../Common/Count.h"
#include"GrenadeBase.h"

class GrenadeHand final : public GrenadeBase
{
public:

	// 爆発までの時間
	static constexpr float BLAST_TIME = (3.0f);

	GrenadeHand(const Transform& follow, int frameNo, std::weak_ptr<ActorBase> parent);
	~GrenadeHand() = default;

	// 衝突したときの処理
	void OnHit(MV1_COLL_RESULT_POLY info) override;
	void OnHit(VECTOR normal, VECTOR hitPos) override;



private:

	// 爆発カウント
	Count blast_;

	// 各自独自の設定
	void SetParam(void) override;

	// 爆発
	void Blast(void) override;

	// 移動
	void UpdateNone(void)override;

	void UpdateBlast(void)override;

	// それぞれの初期化
	void Reset(void) override;
};

