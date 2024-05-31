#pragma once
#include "GunBase.h"

class GunM4 : public GunBase
{
public:

	static constexpr VECTOR RELATIVE_POS = { 0.0f,4.0f,25.0f };

	GunM4(const Transform& follow, int frameNo, std::weak_ptr<ActorBase> parent);
	~GunM4(void) = default;



	// 弾生成
	void CreateBullet(void) override;

	// スコープの位置取得
	const VECTOR GetScopePos()const override;

	void PlayReloadSound(void) override;

private:

	// 各自独自の設定
	void SetParam(void) override;
	void PlayBurstSound(void) override;

	void Draw_Object(void) override;
};

