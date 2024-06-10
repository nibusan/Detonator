#include <DxLib.h>
#include "../Application.h"
#include "../Common/imgui/imgui.h"
#include "../Common/imgui/backends/imgui_impl_dx11.h"
#include "../Common/imgui/backends/imgui_impl_win32.h"
#include "../Common/imgui/misc/cpp/imgui_stdlib.h"
#include "../Utility/Utility.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/GameManager.h"
#include "../Net/NetManager.h"
#include "../UI/UIScreen/UIScreen.h"
#include "../UI/UILayer/UILayer_SelectMenu.h"
#include "MenuScene.h"

void MenuScene::Init(void)
{
	std::unique_ptr<UILayerBase> uiLayer = std::make_unique<UILayer_SelectMenu>();
	uiLayer->Init();
	auto uiScreen = SceneManager::GetInstance().GetUIScreen();
	uiScreen.lock()->PushUILayer(std::move(uiLayer));

	back_ = LoadGraph("Data/Image/Menu_Background.png");
	colorGrHandle_ = LoadGraph("Data/Model/Gun/GunTextures/white.png");

	model_ = std::make_unique<Model>(ResourceManager::GetInstance().GetResourceFile("Model_Gun_AK47")->GetHandle());
	model_->SetIsAutoDeleteHandle(true);
	gun_.SetModel(model_->GetHandle());
	//gun_.SetModel(MV1LoadModel("Data/Model/Gun/AK47.mv1"));
	gun_.quaRot = Quaternion::Euler({ 0.0f,0.0f,Utility::Deg2RadF(-60.0f)});
	gun_.pos = VAdd({ 40.0f,-15.0f,90.0f }, gun_.quaRot.PosAxis({ -10.0f,-10.0f,0.0f }));
	gun_.scl = { 2.0f,2.0f,2.0f };
	gun_.Update();
	gunDeg_ = 0;

	MV1SetTextureGraphHandle(gun_.modelId, 0, colorGrHandle_, 0);
}

void MenuScene::Update(void)
{
	gunDeg_ = (gunDeg_ + 1) % 360;
	gun_.quaRot = Quaternion::Euler({ 0.0f,Utility::Deg2RadF((float)gunDeg_),Utility::Deg2RadF(-60.0f) });
	gun_.pos = VAdd({ 40.0f,-15.0f,90.0f }, gun_.quaRot.PosAxis({ -10.0f,-10.0f,0.0f }));
	gun_.Update();
}

void MenuScene::Draw(void)
{
	DrawGraph(0, 0, back_, true);

	SetUseLighting(false);
	gun_.Draw();
	SetUseLighting(true);
}

void MenuScene::Release(void)
{
	auto uiScreen = SceneManager::GetInstance().GetUIScreen();
	auto layerCount = uiScreen.lock()->GetUILayerCount();
	for (int i = 0; i < layerCount; i++) {
		uiScreen.lock()->PopUILayer();
	}
	DeleteGraph(back_);
	DeleteGraph(colorGrHandle_);
	gun_.Release();
}
