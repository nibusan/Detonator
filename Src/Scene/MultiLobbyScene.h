#pragma once
#include "SceneBase.h"
#include "../Manager/GameManager.h"
#include "../Net/NetManager.h"

class MultiLobbyScene final : public SceneBase
{

public:

	// コンストラクタ
	MultiLobbyScene(void);

	// デストラクタ
	~MultiLobbyScene(void) = default;

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

private:

	GameManager& gameManager_;
	NetManager& netManager_;

	std::shared_ptr<GunMenu> gun_;
	std::shared_ptr<GrenadeMenu> grenades_[2];

	std::unique_ptr<Font> fontGunName_;
	std::unique_ptr<Font> fontInfo_;
	std::unique_ptr<Font> fontMode_;
	std::unique_ptr<Font> fontGun_;
	std::unique_ptr<Font> fontSmoll_;

	Transform box_;

	int back_;
	int menu_;
	int wolf_;
	int eagle_;

	void UISetting(void);
	void DrawUI(void);

};

