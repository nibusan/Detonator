#pragma once
#include "SceneBase.h"
#include "../Object/Common/Transform.h"
#include "../Manager/GameManager.h"

class ActorBase;
class GunMenu;
class GrenadeMenu;
class Graphic;
class Font;

class SingleLobbyScene final : public SceneBase
{

public:

	// �R���X�g���N�^
	SingleLobbyScene(void);

	// �f�X�g���N�^
	~SingleLobbyScene(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;



private:

	GameManager& gameManager_;

	// �\���p�v���C���[
	std::shared_ptr<ActorBase> player_;
	std::shared_ptr<GunMenu> gun_;
	std::shared_ptr<GrenadeMenu> grenades_[2];

	std::unique_ptr<Font> fontGunName_;
	std::unique_ptr<Font> fontInfo_;
	std::unique_ptr<Font> fontMode_;

	Transform box_;

	int back_;

	void UISetting(void);
	void DrawUI(void);

};

