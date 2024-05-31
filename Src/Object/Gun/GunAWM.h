#pragma once
#include "GunBase.h"

class GunAWM : public GunBase
{
public:



	static constexpr VECTOR RELATIVE_POS = { 0.0f,-3.0f,5.0f };

	GunAWM(const Transform& follow, int frameNo, std::shared_ptr<ActorBase> parent);
	~GunAWM(void) = default;



	// 弾生成
	void CreateBullet(void) override;

	// スコープの位置取得
	const VECTOR GetScopePos()const override;

	void PlayReloadSound(void) override;

private:

	// 画像
	int imgH_;

	// 各自独自の設定
	void SetParam(void) override;
	void PlayBurstSound(void) override;

	void Draw_Object(void) override;
	void DrawTrans(void) override;
	void Release_Object(void) override;

};

