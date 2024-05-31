#pragma once
#include <vector>
#include "SceneBase.h"
#include "../Manager/GameManager.h"
#include "../Object/Common/Transform.h"
#include "../Common/Handle/Graphic/Graphic.h"

class SingleGameScene final : public SceneBase
{

public:

	// コンストラクタ
	SingleGameScene(void);

	// デストラクタ
	~SingleGameScene(void) = default;

private:

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

	GameManager& gameManager_;

	Transform skyDoom_;

	Unique_Graphic vignetteScreen_;
	Weak_PixelShader pixelShader_Vignette_;


	std::shared_ptr<ObjectBase> player_;

	bool isDisplayUI_;
	bool test_;

	std::shared_ptr<ObjectBase> map_;
	
	bool isActive_;

	Shared_Font usingFont_;
	std::unique_ptr<Font> fontDamage_;
	std::unique_ptr<Font> fontDeath_;
	std::unique_ptr<Font> fontDeathInfo_;
	float reloadAlphaRad_;

	int g1_;
	int g2_;
	int g3_;
};

