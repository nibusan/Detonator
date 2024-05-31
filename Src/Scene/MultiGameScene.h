#pragma once
#include <unordered_map>
#include "SceneBase.h"
#include "../Manager/GameManager.h"
#include "../Net/NetStructures.h"

class MultiGameScene final : public SceneBase
{

public:

	// コンストラクタ
	MultiGameScene(void);

	// デストラクタ
	~MultiGameScene(void) = default;

private:

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

	GameManager& gameManager_;

	std::unordered_map<PLAYER_TYPE, std::shared_ptr<ObjectBase>> players_;

	std::shared_ptr<ObjectBase> map_;

	Unique_Graphic vignetteScreen_;
	Weak_PixelShader pixelShader_Vignette_;

	bool isDisplayUI_;
	bool test_;
	bool isActive_;

	Shared_Font usingFont_;
	Shared_Font fontScore_;
	Shared_Font fontGoal_;
	Shared_Font fontDamage_;
	Shared_Font fontDeath_;
	Shared_Font fontDeathInfo_;
	float reloadAlphaRad_;

	int timeFrame_;
	int time_;
	int timeIcon_;
	int blueFrame_;
	int blue_;
	int redFrame_;
	int red_;
	int wolf_;
	int eagle_;

	Transform skyDoom_;

	// プレイヤーのID
	PLAYER_TYPE keyMain_;

	int Sound_3DTest_;
};

