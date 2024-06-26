#pragma once
#include"GrenadeBase.h"

class GrenadeSticky final : public GrenadeBase
{
public:


	GrenadeSticky(const Transform& follow, int frameNo, std::weak_ptr<ActorBase> parent);
	~GrenadeSticky() = default;

	// 衝突したときの処理
	//void OnHit(MV1_COLL_RESULT_POLY info) override;
	void OnHit(VECTOR normal, VECTOR hitPos) override;


private:
	int soundHandle_;

	// 各自独自の設定
	void SetParam(void) override;

	// 爆発
	void Blast(void) override;

	// 移動
	void UpdateNone(void)override;

	void UpdateBlast(void)override;

	void Release_Object(void) override;

	// それぞれの初期化
	void Reset(void) override;
};

