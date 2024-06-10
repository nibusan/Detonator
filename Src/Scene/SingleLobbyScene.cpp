#include <DxLib.h>
#include "../Application.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Utility/Utility.h"
#include "../Object/Actor/Player/PlayerMenu.h"
#include "../Object/Gun/GunMenu.h"
#include "../Object/Grenade/GrenadeMenu.h"
#include "../UI/UILayer/UILayer_sLobby.h"
#include "../UI/UIScreen/UIScreen.h"
#include "SingleLobbyScene.h"

SingleLobbyScene::SingleLobbyScene(void) : gameManager_(GameManager::GetInstance())
{
}

SingleLobbyScene::~SingleLobbyScene(void)
{
	int a = 1;
	a = 0;
}

void SingleLobbyScene::Init(void)
{
	UISetting();

	std::unique_ptr<UILayerBase> uiLayer = std::make_unique<UILayer_sLobby>();
	uiLayer->Init();
	auto uiScreen = SceneManager::GetInstance().GetUIScreen();
	uiScreen.lock()->PushUILayer(std::move(uiLayer));
}

void SingleLobbyScene::Update(void)
{
	player_->Update();
	gun_->Update();
	grenades_[0]->Update();
	grenades_[1]->Update();
}

void SingleLobbyScene::Draw(void)
{
	DrawUI();

	grenades_[0]->Draw();
	grenades_[1]->Draw();
}

void SingleLobbyScene::Release(void)
{
	DeleteGraph(back_);
	player_->Release();
	gun_->Release();
	grenades_[0]->Release();
	grenades_[1]->Release();
	box_.scl = { 1.0f,1.0f,1.0f };
	box_.Update();
	box_.Release();

	auto uiScreen = SceneManager::GetInstance().GetUIScreen();
	uiScreen.lock()->PopUILayer();
}

void SingleLobbyScene::UISetting(void)
{
	back_ = LoadGraph("Data/Image/Menu_Background.png");

	// �\���p�v���C���[
	player_ = std::make_shared<PlayerMenu>(ID::PLAYER_1);
	player_->Init();

	VECTOR pos = { 35.0f,0.0f,120.0f };
	gun_ = std::make_shared<GunMenu>(ID::PLAYER_1, pos);

	pos = { -3.0f,-70.0f,200.0f };
	grenades_[0] = std::make_shared<GrenadeMenu>(ID::PLAYER_1, 0, pos);
	pos = { 125.0f,-70.0f,200.0f };
	grenades_[1] = std::make_shared<GrenadeMenu>(ID::PLAYER_1, 1, pos);

	box_.SetModel(MV1LoadModel("Data/Model/Box/box.mv1"));
	box_.pos = { 100.0f,-100.0f,500.0f };
	box_.quaRot = Quaternion::Euler({ Utility::Deg2RadF(22.0f),Utility::Deg2RadF(110.0f),Utility::Deg2RadF(-50.0f) });
	box_.scl = { 2.8f,2.8f,2.8f };
	box_.Update();

	std::weak_ptr<Font> s = std::dynamic_pointer_cast<Font>(ResourceManager::GetInstance().
		GetResourceFile("Font_GenShinGothic_Bold"));

	fontGunName_ = std::make_unique<Font>(s.lock()->GetFontName(), 50);
	fontGunName_->SetIsAutoDeleteHandle(true);
	fontInfo_ = std::make_unique<Font>(s.lock()->GetFontName(), 30);
	fontInfo_->SetIsAutoDeleteHandle(true);
	fontMode_ = std::make_unique<Font>(s.lock()->GetFontName(), 40);
	fontMode_->SetIsAutoDeleteHandle(true);
}

void SingleLobbyScene::DrawUI(void)
{
	// UI�Ƃ��Ĉ��������Ȃ����m�̕`��

	DrawGraph(0, 0, back_, true);

	box_.Draw();

	auto info = gameManager_.GetLoadOut(0);
	switch (info.gunType_)
	{
	case GUN_TYPE::AK47:
		DrawStringToHandle(750, 200, "AK-47", 0xffffff, fontGunName_->GetHandle());
		DrawStringToHandle(750, 260,
			"1949�N�Ƀ\�r�G�g�A�M�R�������̗p�����������e", 0xffffff, fontInfo_->GetHandle());
		DrawStringToHandle(750, 300,
			"�Η͍͂������������傫��", 0xffffff, fontInfo_->GetHandle());
		break;
	case GUN_TYPE::AWM:
		DrawStringToHandle(750, 200, "AWM", 0xffffff, fontGunName_->GetHandle());
		DrawStringToHandle(750, 260,
			"�C�M���X�R�Ő����̗p����Ă���{���g�A�N�V���������̑_���e", 0xffffff, fontInfo_->GetHandle());
		DrawStringToHandle(750, 300,
			"�������ɋ����A�ߋ����Ɏア", 0xffffff, fontInfo_->GetHandle());
		break;
	case GUN_TYPE::M4:
		DrawStringToHandle(750, 200, "M4", 0xffffff, fontGunName_->GetHandle());
		DrawStringToHandle(750, 260,
			"���݃A�����J�R���m�̑唼���������Ă���A�T���g���C�t��", 0xffffff, fontInfo_->GetHandle());
		DrawStringToHandle(750, 300,
			"�����͏��������Η͂�����������", 0xffffff, fontInfo_->GetHandle());
		break;
	case GUN_TYPE::M24R:
		DrawStringToHandle(750, 200, "M24R", 0xffffff, fontGunName_->GetHandle());
		DrawStringToHandle(750, 260,
			"�A�ː��\�������ߋ����퓬�������T�u�}�V���K��", 0xffffff, fontInfo_->GetHandle());
		DrawStringToHandle(750, 300,
			"M24�Ƃ����X�i�C�p�[���C�t���Ƃ͕ʕ�", 0xffffff, fontInfo_->GetHandle());
		break;
	}

	DrawStringToHandle(750, 710,
		"�X���b�g1", 0xffffff, fontInfo_->GetHandle());
	switch (info.greType_[0])
	{
	case GRENADE_TYPE::HAND:
		DrawStringToHandle(900, 910,
			"��֒e", 0xffffff, fontInfo_->GetHandle());
		break;
	case GRENADE_TYPE::SMOKE:
		DrawStringToHandle(870, 910,
			"������֒e", 0xffffff, fontInfo_->GetHandle());
		break;
	case GRENADE_TYPE::STICKY:
		DrawStringToHandle(885, 910,
			"�S���֒e", 0xffffff, fontInfo_->GetHandle());
		break;
	}



	DrawStringToHandle(1350, 710,
		"�X���b�g2", 0xffffff, fontInfo_->GetHandle());
	switch (info.greType_[1])
	{
	case GRENADE_TYPE::HAND:
		DrawStringToHandle(1500, 910,
			"��֒e", 0xffffff, fontInfo_->GetHandle());
		break;
	case GRENADE_TYPE::SMOKE:
		DrawStringToHandle(1470, 910,
			"������֒e", 0xffffff, fontInfo_->GetHandle());
		break;
	case GRENADE_TYPE::STICKY:
		DrawStringToHandle(1485, 910,
			"�S���֒e", 0xffffff, fontInfo_->GetHandle());
		break;
	}

	if (gameManager_.GetMode() == GameManager::MODE::TIMEATTACK) {
		DrawStringToHandle(33, 40,
			"TIMEATTACK", 0xffffff, fontMode_->GetHandle());
		DrawStringToHandle(320, 49,
			"-�^�C���A�^�b�N", 0xffffff, fontInfo_->GetHandle());
	}
	else if (gameManager_.GetMode() == GameManager::MODE::SCOREATTACK) {
		DrawStringToHandle(33, 40,
			"SCOREATTACK", 0xffffff, fontMode_->GetHandle());
		DrawStringToHandle(355, 49,
			"-�X�R�A�A�^�b�N", 0xffffff, fontInfo_->GetHandle());
	}


	player_->Draw();
	gun_->Draw();
	grenades_[0]->Draw();
	grenades_[1]->Draw();
}
