#pragma once
#include "../Manager/GameManager.h"
#include "SceneBase.h"

class SingleResultScene final : public SceneBase
{

public:

	// コンストラクタ
	SingleResultScene(void);

	// デストラクタ
	~SingleResultScene(void) = default;

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

private:

	GameManager& gameManager_;

	std::unique_ptr<Font> fontHeading_;
	std::unique_ptr<Font> fontSubheading_;
	std::unique_ptr<Font> fontInfo_;
	std::unique_ptr<Font> fontMode_;

	int back_;

	void UISetting(void);
	void DrawUI(void);

};

