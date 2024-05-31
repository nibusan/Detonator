#pragma once
#include "SceneBase.h"
#include "../Manager/GameManager.h"
#include "../Net/NetManager.h"

class MultiResultScene final : public SceneBase
{

public:

	// コンストラクタ
	MultiResultScene(void);

	// デストラクタ
	~MultiResultScene(void) = default;

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

private:

	GameManager& gameManager_;
	NetManager& netManager_;

	std::unique_ptr<Font> fontWinner_;
	std::unique_ptr<Font> fontScore_;
	std::unique_ptr<Font> fontTeam_;
	std::unique_ptr<Font> fontName_;
	std::unique_ptr<Font> fontAlias_;
	std::unique_ptr<Font> fontMode_;

	int back_;
	int alias_[(int)GameManager::ALIAS::COMEDIAN + 1];

	void UISetting(void);
	void DrawUI(void);

};

