#include <chrono>
#include <memory>
#include <fstream>
#include <DxLib.h>
#include "../Common/imgui/imgui.h"
#include "../Common/imgui/backends/imgui_impl_dx11.h"
#include "../Common/imgui/backends/imgui_impl_win32.h"
#include "../Common/imgui/misc/cpp/imgui_stdlib.h"
#include "../Common/Fader.h"
#include "../Application.h"
#include "../Scene/TitleScene.h"
#include "../Scene/MenuScene.h"
#include "../Scene/SingleLobbyScene.h"
#include "../Scene/SingleGameScene.h"
#include "../Scene/SingleResultScene.h"
#include "../Scene/MultiLobbyScene.h"
#include "../Scene/MultiGameScene.h"
#include "../Scene/MultiResultScene.h"
#include "../Data/Message/MessageTable.h"
#include "../Data/UI/UILayerList.h"
#include "../UI/UIFunctionList.h"
#include "../UI/UIScreen/UIScreen.h"
#include "ResourceManager.h"
#include "InputManager.h"
#include "Camera/CameraBase.h"
#include "Camera/NormalCamera.h"
#include "Camera/GameCamera.h"
#include "EffectManager.h"
#include "CollisionManager.h"
#include "GameManager.h"
#include "AudioManager.h"
#include "SceneManager.h"

SceneManager* SceneManager::instance_ = nullptr;

void SceneManager::CreateInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new SceneManager();
	}
	instance_->Init();
}

SceneManager& SceneManager::GetInstance(void)
{
	return *instance_;
}

void SceneManager::Init(void)
{
	sceneId_ = SCENE_ID::TITLE;
	waitSceneId_ = SCENE_ID::NONE;

	fader_ = std::make_unique<Fader>();
	fader_->Init();

	// カメラ
	camera_ = std::make_shared<NormalCamera>();
	camera_->Init();

	isSceneChanging_ = false;

	// デルタタイム
	preTime_ = std::chrono::system_clock::now();

	isMultiPlayer_ = false;

	drawScreen_ = std::make_unique<Graphic>(MakeScreen(Application::SCREEN_SIZE_X,Application::SCREEN_SIZE_Y));
	drawScreen_->SetIsAutoDeleteHandle(true);

	// メッセージテーブルの読み込み
	MessageTable::GetInstance().Init();

	UIFunctionList::GetInstance().Init();
	UILayerList::GetInstance().Init();

	uiScreen_ = std::make_unique<UIScreen>();
	uiScreen_->Init();

	// 3D用の設定
	Init3D();

	// 初期シーンの設定
	DoChangeScene(SCENE_ID::TITLE);

	// ImGuiの初期化
	ImGui::DebugCheckVersionAndDataLayout("1.89.8 WIP", sizeof(ImGuiIO), sizeof(ImGuiStyle), sizeof(ImVec2), sizeof(ImVec4), sizeof(ImDrawVert), sizeof(ImDrawIdx));
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui_ImplWin32_Init(DxLib::GetMainWindowHandle());
	ImGui_ImplDX11_Init((ID3D11Device*)GetUseDirect3D11Device(), (ID3D11DeviceContext*)GetUseDirect3D11DeviceContext());

	PlayBGM();

}

void SceneManager::Init3D(void)
{

	// 背景色設定
	SetBackgroundColor(0, 0, 0);

	// Zバッファを有効にする
	SetUseZBuffer3D(true);

	// Zバッファへの書き込みを有効にする
	SetWriteZBuffer3D(true);

	// バックカリングを有効にする
	SetUseBackCulling(true);

	// ライトの設定
	SetUseLighting(true);

	// 正面から斜め下に向かったライト
	ChangeLightTypeDir({ -1.00f, -1.00f, -1.00f });

	// ライトハンドル
	lightHandle1_ = CreateDirLightHandle({ 1.00f, -1.00f, 1.00f });

	//CreateDirLightHandle({ -1.00f, -1.00f, 0.00f });
}

void SceneManager::Update(void) {

	if (scene_ == nullptr) {
		return;
	}

	// ImGuiの処理
	ImGuiIO& io = ImGui::GetIO();
	auto mousePos = InputManager::GetInstance().GetMousePos();
	io.AddMousePosEvent(mousePos.x, mousePos.y);
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("DebugMenu");

	//各シーンの名前
	std::string sceneName[] = {
		"None",
		"Logo",
		"Title",
		"SelectM",
		"SingleMenu",
		"SingleGame",
		"SingleResult",
		"MultiMenu",
		"MultiGame",
		"MultiResult",
		"Connect"
	};
	static int g = 0;
	ImGui::Text(("FPS : " + std::to_string(g)).c_str());
	g++;
	ImGui::Text(("Scene : " + sceneName[static_cast<int>(sceneId_)]).c_str());

	// カメラ更新
	camera_->Update();
	
	fader_->Update();
	if (isSceneChanging_) {
		Fade();
	}
	else {
		auto& gameManager = GameManager::GetInstance();
		gameManager.Update();
		scene_->Update();
	}

	uiScreen_->Update();
	uiScreen_->DeleteUILayer();

	ImGui::End();
}

void SceneManager::Draw(void) {
	auto& effectManager = EffectManager::GetInstance();

	// 描画先グラフィック領域の指定
	// (３Ｄ描画で使用するカメラの設定などがリセットされる)
	auto defScreen = DX_SCREEN_BACK;
	SetDrawScreen(drawScreen_->GetHandle());

	// 画面を初期化
	ClearDrawScreen();

	// カメラ設定
	auto c = std::dynamic_pointer_cast<CameraBase>(camera_);
	c->SetBeforeDraw();

	// エフェクトの更新
	effectManager.Update();

	effectManager.Draw();

	// 描画
	scene_->Draw();

	uiScreen_->Draw();


	//// デバッグ
	//auto& gameManager = GameManager::GetInstance();
	//gameManager.DrawDebug();

	// 暗転・明転
	fader_->Draw();

	//DrawFormatString(0, 500, 0x00FF00, "FPS:%.2f", 1.0f / deltaTime_);
	//DrawFormatString(0, 500, 0x00FF00, "FPS:%.2f", GetFPS());

	// ImGuiの描画
	ImGui::Render();
	//ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	RefreshDxLibDirect3DSetting();

	SetDrawScreen(DX_SCREEN_BACK);
	drawScreen_->Draw({ 0.0f,0.0f }, false, nullptr);

	// フレームの終了時刻を記録
	auto endTime = std::chrono::system_clock::now();
	std::chrono::duration<double> frameTime = endTime - preTime_;

	// フレームが60FPSになるように待つ
	std::chrono::duration<double> targetFrameTime = std::chrono::duration<double>(1.0 / 64.0);
	std::chrono::duration<double> sleepTime = targetFrameTime - frameTime;
	if (sleepTime > std::chrono::duration<double>::zero()) {

		std::this_thread::sleep_for(sleepTime);

	}

	//while (sleepTime > std::chrono::duration<double>::zero())
	//{
	//	sleepTime -= frameTime;

	//	// マウス位置を中心にセット
	//	Vector2 center = { Application::SCREEN_SIZE_X / 2,Application::SCREEN_SIZE_Y / 2 };
	//	SetMousePoint(center.x, center.y);
	//}
}

void SceneManager::Destroy(void)
{
	scene_->Release();

	camera_->Release();

	// ライトハンドル
	DeleteLightHandle(lightHandle1_);

	// ImGuiの解放
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	AudioManager::GetInstance().Delete(sound_Music_);

	delete instance_;

}

void SceneManager::ChangeScene(SCENE_ID nextId)
{
	// フェード処理が終わってからシーンを変える場合もあるため、
	// 遷移先シーンをメンバ変数に保持
	waitSceneId_ = nextId;

	// フェードアウト(暗転)を開始する
	fader_->SetFade(Fader::STATE::FADE_OUT);
	isSceneChanging_ = true;
}

SceneManager::SCENE_ID SceneManager::GetSceneID(void)
{
	return sceneId_;
}

SceneManager::SCENE_ID SceneManager::GetPreSceneID(void)
{
	return preSceneId_;
}

double SceneManager::GetDeltaTime(void) const
{
	return 1.0f / 60.0f;
	//return deltaTime_;
}

float SceneManager::GetDeltaMult(float num)
{
	return num * GetDeltaTime();
}

std::shared_ptr<CameraBase> SceneManager::GetCamera(void) const
{
	return camera_;
}

bool SceneManager::IsGameScene(void)
{
	return (sceneId_ == SCENE_ID::sGAME || sceneId_ == SCENE_ID::mGAME);
}

void SceneManager::PlayBGM(void)
{
	switch (GetRand(2))
	{
	case 0:
		sound_Music_ = AudioManager::GetInstance().Play(AudioList::AUDIO_TYPE::MUSIC_01, true, 0.65f, 44100);
		break;
	case 1:
		sound_Music_ = AudioManager::GetInstance().Play(AudioList::AUDIO_TYPE::MUSIC_03, true, 0.65f, 44100);
		break;
	case 2:
		sound_Music_ = AudioManager::GetInstance().Play(AudioList::AUDIO_TYPE::MUSIC_04, true, 0.65f, 44100);
		break;
	}
}

void SceneManager::StopBGM(void)
{
	AudioManager::GetInstance().Stop(sound_Music_);
}

void SceneManager::CreateDebugText(const std::string& name, const std::string& data) {
	//デバッグフォルダが存在するかチェック(なければ作成)
	if (!std::filesystem::exists("Debug")) {
		std::filesystem::create_directory("Debug");
	}
	if (!std::filesystem::is_directory("Debug")) {
		std::filesystem::create_directory("Debug");
	}

	std::ofstream textFile;
	textFile.open("Debug/" + name + ".txt", std::ios::app);
	textFile << data;
	textFile.close();
}

std::weak_ptr<UIScreen> SceneManager::GetUIScreen(void) const {
	return uiScreen_;
}

SceneManager::SceneManager(void)
{

	sceneId_ = SCENE_ID::NONE;
	waitSceneId_ = SCENE_ID::NONE;

	scene_ = nullptr;
	fader_ = nullptr;

	isSceneChanging_ = false;

	// デルタタイム
	deltaTime_ = 1.0f / 60.0f;

	camera_ = nullptr;

	isMultiPlayer_ = false;

}

void SceneManager::ResetDeltaTime(void)
{
	deltaTime_ = 0.016f;
	preTime_ = std::chrono::system_clock::now();
}

void SceneManager::DoChangeScene(SCENE_ID sceneId)
{
	if (sceneId_ == SCENE_ID::sRESULT ||
		sceneId_ == SCENE_ID::mRESULT) {
		AudioManager::GetInstance().Stop(sound_Music_);
		PlayBGM();
	}

	if ((sceneId_ == SCENE_ID::sGAME ||
		sceneId_ == SCENE_ID::mGAME) &&
		sceneId == SCENE_ID::MENU) {
		AudioManager::GetInstance().Stop(sound_Music_);
		PlayBGM();
	}

	if (sceneId_ == SCENE_ID::mGAME &&
		sceneId == SCENE_ID::mLOBBY) {
		AudioManager::GetInstance().Stop(sound_Music_);
		PlayBGM();
	}

	// シーンを変更する
	preSceneId_ = sceneId_;
	sceneId_ = sceneId;

	// 現在のシーンを解放
	if (scene_ != nullptr)
	{
		scene_->Release();
	}

	// 現在のカメラの解放
	if(camera_ != nullptr)
	{
		camera_->Release();
	}

	CollisionManager::GetInstance().ClearColliders();

	// シーンで使うリソースの読み込み & 不要なリソースの解放
	ResourceManager::GetInstance().LoadResource(sceneId_);

	switch (sceneId_)
	{
	case SceneManager::SCENE_ID::LOGO:
	case SceneManager::SCENE_ID::TITLE:
		scene_ = std::make_unique<TitleScene>();
		camera_ = std::make_shared<NormalCamera>();
		SetMouseDispFlag(true);
		break;
	case SceneManager::SCENE_ID::MENU:
		scene_ = std::make_unique<MenuScene>();
		camera_ = std::make_shared<NormalCamera>();
		SetMouseDispFlag(true);
		GameManager::GetInstance().ChangeState(GameManager::STATE::NONE);
		break;
	case SceneManager::SCENE_ID::sLOBBY:
		scene_ = std::make_unique<SingleLobbyScene>();
		camera_ = std::make_shared<NormalCamera>();
		SetMouseDispFlag(true);
		GameManager::GetInstance().ChangeState(GameManager::STATE::sLOBBY);
		break;
	case SceneManager::SCENE_ID::sGAME:
		scene_ = std::make_unique<SingleGameScene>();
		camera_ = std::make_shared<GameCamera>();
		SetMouseDispFlag(false);
		AudioManager::GetInstance().Stop(sound_Music_);
		GameManager::GetInstance().ChangeState(GameManager::STATE::sGAME);

		break;
	case SceneManager::SCENE_ID::sRESULT:
		scene_ = std::make_unique<SingleResultScene>();
		camera_ = std::make_shared<NormalCamera>();
		SetMouseDispFlag(true);
		GameManager::GetInstance().ChangeState(GameManager::STATE::NONE);
		sound_Music_ = AudioManager::GetInstance().Play(AudioList::AUDIO_TYPE::MUSIC_06, true, 0.6f, 44100);
		AudioManager::GetInstance().Play(AudioList::AUDIO_TYPE::SYSTEM_RESULT, false, 1.0f, 44100);
		break;
	case SceneManager::SCENE_ID::mLOBBY:
		scene_ = std::make_unique<MultiLobbyScene>();
		camera_ = std::make_shared<NormalCamera>();
		GameManager::GetInstance().ChangeState(GameManager::STATE::mLOBBY);
		NetManager::GetInstance().ChangeGameState(GAME_STATE::LOBBY);
		SetMouseDispFlag(true);
		break;
	case SceneManager::SCENE_ID::mGAME:
		scene_ = std::make_unique<MultiGameScene>();
		camera_ = std::make_shared<GameCamera>();
		SetMouseDispFlag(false);
		AudioManager::GetInstance().Stop(sound_Music_);
		GameManager::GetInstance().ChangeState(GameManager::STATE::mGAME);
		NetManager::GetInstance().ChangeGameState(GAME_STATE::GAME_PLAYING);
		break;
	case SceneManager::SCENE_ID::mRESULT:
		scene_ = std::make_unique<MultiResultScene>();
		camera_ = std::make_shared<NormalCamera>();
		SetMouseDispFlag(true);
		GameManager::GetInstance().ChangeState(GameManager::STATE::NONE);
		NetManager::GetInstance().ChangeGameState(GAME_STATE::RESULT);
		if (GameManager::GetInstance().WinPlayer()) {
			sound_Music_ = AudioManager::GetInstance().Play(AudioList::AUDIO_TYPE::MUSIC_06, true, 0.7f, 44100);
		}else {
			sound_Music_ = AudioManager::GetInstance().Play(AudioList::AUDIO_TYPE::MUSIC_08, true, 0.7f, 44100);
		}
		
		break;
	case SceneManager::SCENE_ID::NONE:
	default:
		break;
	}

	scene_->Init();
	camera_->Init();

	

	ResetDeltaTime();

	waitSceneId_ = SCENE_ID::NONE;

}

void SceneManager::Fade(void)
{

	Fader::STATE fState = fader_->GetState();
	switch (fState)
	{
	case Fader::STATE::FADE_IN:
		// 明転中
		if (fader_->IsEnd())
		{
			// 明転が終了したら、フェード処理終了
			fader_->SetFade(Fader::STATE::NONE);
			isSceneChanging_ = false;
		}
		break;
	case Fader::STATE::FADE_OUT:
		// 暗転中
		if (fader_->IsEnd())
		{
			// 完全に暗転してからシーン遷移
			DoChangeScene(waitSceneId_);
			// 暗転から明転へ
			fader_->SetFade(Fader::STATE::FADE_IN);
		}
		break;
	}

}
