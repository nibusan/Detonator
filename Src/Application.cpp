#include <DxLib.h>
#include "Data/Audio/AudioList.h"
#include "Manager/AudioManager.h"
#include "Manager/ResourceManager.h"
#include "Manager/InputManager.h"
#include "Manager/SceneManager.h"
#include "Manager/EffectManager.h"
#include "Manager/CollisionManager.h"
#include "Manager/GameManager.h"
#include "Net/NetManager.h"
#include "Application.h" 

Application* Application::instance_ = nullptr;

const std::string Application::PATH_IMAGE = "Data/Image/";
const std::string Application::PATH_MODEL = "Data/Model/";
const std::string Application::PATH_EFFECT = "Data/Effect/";

void Application::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new Application();
	}
	instance_->Init();
}

Application& Application::GetInstance(void)
{
	return *instance_;
}

void Application::Init(void)
{

	// アプリケーションの初期設定
	SetWindowText("DETONATION");

	// ウィンドウサイズ
	int screenSize_X = SCREEN_SIZE_X;
	int screenSize_Y = SCREEN_SIZE_Y;

	// 色モード
	int bitColor = 32;

	// 仮想フルスクリーンにする
	SetGraphMode(screenSize_X, screenSize_Y, 32);
	ChangeWindowMode(true);
	GetDefaultState(&screenSize_X, &screenSize_Y, &bitColor);
	SetWindowSize(screenSize_X, screenSize_Y);

	// ウィンドウからフォーカスが外れた時も処理を実行する
	//SetAlwaysRunFlag(true);

	// XAudio2を使用する
	SetEnableXAudioFlag(true);

	// １メートルに相当する値を設定する(3Dのサウンドで使用)
	Set3DSoundOneMetre(100.0f);

	isShutdown_ = false;

	// DxLibの初期化
	SetUseDirect3DVersion(DX_DIRECT3D_11);
	isInitFail_ = false;
	if (DxLib_Init() == -1)
	{
		isInitFail_ = true;
		return;
	}

	Effekseer_Init(8000);
	SetChangeScreenModeGraphicsSystemResetFlag(false);
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();

	// キー制御初期化
	SetUseDirectInputFlag(true);
	InputManager::CreateInstance();

	// 衝突判定管理初期化
	CollisionManager::CreateInstance();

	// リソース管理初期化
	ResourceManager::GetInstance().Init();

	// オーディオリスト初期化
	AudioList::GetInstance().Init();

	// オーディオ管理初期化
	AudioManager::GetInstance().Init();

	// シーン管理初期化
	SceneManager::CreateInstance();
	
	// エフェクト管理初期化
	EffectManager::CreateInstance();

	// ネットワーク管理初期化
	NetManager::CreateInstance();

	// ゲーム管理初期化
	GameManager::CreateInstance();
}

void Application::Run(void)
{

	auto& inputManager = InputManager::GetInstance();
	auto& sceneManager = SceneManager::GetInstance();
	auto& netManager = NetManager::GetInstance();

	// ゲームループ
	while (ProcessMessage() == 0 && !isShutdown_)
	{
		inputManager.Update();
		// ネットワーク管理更新(フレームの最初)
		netManager.Update();

		inputManager.NetUpdate();

		
		sceneManager.Update();
		
		sceneManager.Draw();

		// ネットワーク管理更新(フレームの最後)
		netManager.UpdateEndOfFrame();

		ScreenFlip();

	}

}

void Application::Destroy(void)
{
	// 以前の設計だとメモリリークが起こっていたため、
	// (静的インスタンスを解放していたデストラクタが呼び出されない)
	// 明示的にインスタンスを破棄する
	InputManager::GetInstance().Destroy();
	SceneManager::GetInstance().Destroy();
	EffectManager::GetInstance().Destroy();
	CollisionManager::GetInstance().Destroy();
	GameManager::GetInstance().Destroy();
	NetManager::GetInstance().Destroy();
	
	Effkseer_End();

	// DxLib終了
	if (DxLib_End() == -1)
	{
		isReleaseFail_ = true;
	}

	delete instance_;

}

bool Application::IsInitFail(void) const
{
	return isInitFail_;
}

bool Application::IsReleaseFail(void) const
{
	return isReleaseFail_;
}

void Application::Shutdown(void) {
	isShutdown_ = true;
}

Application::Application(void)
{
	isInitFail_ = false;
	isReleaseFail_ = false;
}
