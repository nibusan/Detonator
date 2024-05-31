#include <string>
#include <DxLib.h>
#include "../Common/imgui/imgui.h"
#include "../Application.h"
#include "../Utility/Utility.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/Camera/CameraBase.h"
#include "../Manager/Camera/NormalCamera.h"
#include "../Manager/AudioManager.h"
#include "../Object/ObjectBase.h"
#include "../UI/UILayer/UILayer_Title.h"
#include "../UI/UIScreen/UIScreen.h"
#include "TitleScene.h"

TitleScene::TitleScene(void)
{
	
}

TitleScene::~TitleScene(void)
{

}

void TitleScene::Init(void)
{
	//シーン描画用スクリーンの生成
	sceneScreen_ = std::make_shared<Graphic>(MakeScreen(Application::SCREEN_SIZE_X,Application::SCREEN_SIZE_Y));
	sceneScreen_->SetIsAutoDeleteHandle(true);

	std::unique_ptr<UILayerBase> uiLayer = std::make_unique<UILayer_Title>();
	uiLayer->Init();
	auto uiScreen = SceneManager::GetInstance().GetUIScreen();
	uiScreen.lock()->PushUILayer(std::move(uiLayer));

	usingFont_ = std::make_unique<Font>(std::dynamic_pointer_cast<Font>(ResourceManager::GetInstance().Get("Font_GenShinGothic_Bold"))->GetFontName(), 32);
	usingFont_->SetIsAutoDeleteHandle(true);
	alphaRad_ = 0.0f;
	frameCNT_ = 0;
}

void TitleScene::Update(void)
{
	// シーン遷移
	InputManager& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_SPACE))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::MENU);
	}

	alphaRad_ += Utility::Rad2DegF(2.0f);
	frameCNT_++;
}

void TitleScene::Draw(void)
{
	int defScreen = DxLib::GetDrawScreen();
	SetDrawScreen(sceneScreen_->GetHandle());
	ClearDrawScreen();

	SceneManager::GetInstance().GetCamera()->SetBeforeDraw();


	if ((frameCNT_ / 120) % 2 == 0) {
		int width = GetDrawStringWidthToHandle("PRESS SPACE", 3, usingFont_->GetHandle());
		DrawStringToHandle(Application::SCREEN_SIZE_X / 2 - width / 2 - 80, 950, "PRESS SPACE", 0xFFFFFF, usingFont_->GetHandle());
	}

	SetDrawScreen(defScreen);
	sceneScreen_->Draw({ 0.0f,0.0f }, false, nullptr);
}

void TitleScene::Release(void)
{
	auto uiScreen = SceneManager::GetInstance().GetUIScreen();
	uiScreen.lock()->PopUILayer();
}
