#pragma once
#include "GunBase.h"

class GunM24R : public GunBase
{
public:



	static constexpr VECTOR RELATIVE_POS = { 0.0f,8.0f,13.0f };

	GunM24R(const Transform& follow, int frameNo, std::weak_ptr<ActorBase> parent);
	~GunM24R(void) = default;

	// 弾生成
	void CreateBullet(void) override;

	// スコープの位置取得
	const VECTOR GetScopePos()const override;

	void PlayReloadSound(void) override;

private:

	// 各自独自の設定
	void SetParam(void) override;
	void PlayBurstSound(void) override;


};

