#pragma once
#include <memory>
#include "../Common/Handle/Font/Font.h"
#include "SceneBase.h"

class ObjectBase;

class TitleScene : public SceneBase
{

public:

	// コンストラクタ
	TitleScene(void);

	// デストラクタ
	~TitleScene(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

private:
	Unique_Font usingFont_;
	float alphaRad_;
	int frameCNT_;

};
