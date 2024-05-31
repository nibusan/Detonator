#pragma once
#include <chrono>
#include <memory>
#include <functional>
#include "../Common/Handle/Graphic/Graphic.h"
class SceneBase;
class Fader;
class CameraBase;
class ObjectBase;
class UIScreen;

class SceneManager
{

public:

	// シーン管理用　
	enum class SCENE_ID
	{
		NONE,
		LOGO,		// ロゴ
		TITLE,		// タイトル
		MENU,		// メニュー
		sLOBBY,		// シングルプレイロビー
		sGAME,		// シングルプレイゲーム
		sRESULT,	// シングルプレイリザルト
		mLOBBY,		// マルチプレイロビー
		mGAME,		// マルチプレイゲーム
		mRESULT,	// マルチプレイリザルト
		MAX
	};

	// インスタンスの生成
	static void CreateInstance(void);

	// インスタンスの取得
	static SceneManager& GetInstance(void);

	void Init(void);
	void Init3D(void);
	void Update(void);
	void Draw(void);

	// リソースの破棄
	void Destroy(void);

	// 状態遷移
	void ChangeScene(SCENE_ID nextId);

	// シーンIDの取得
	SCENE_ID GetSceneID(void);
	SCENE_ID GetPreSceneID(void);

	// デルタタイムの取得
	double GetDeltaTime(void) const;

	// 引数にデルタタイムをかけたものを返す
	float GetDeltaMult(float num);

	// カメラの取得
	std::shared_ptr<CameraBase> GetCamera(void) const;

	bool IsGameScene(void);

	void PlayBGM(void);
	void StopBGM(void);

	/// <summary>
	/// デバッグ用のテキストファイルを出力する
	/// </summary>
	/// <param name="name"> 出力するテキストファイルの名前 </param>
	/// <param name="data"> 出力するテキスト </param>
	static void CreateDebugText(const std::string& name, const std::string& data);

	std::weak_ptr<UIScreen> GetUIScreen(void) const;
private:

	// 静的インスタンス
	static SceneManager* instance_;

	SCENE_ID sceneId_;
	SCENE_ID waitSceneId_;
	SCENE_ID preSceneId_;

	// フェード
	std::unique_ptr<Fader>fader_;

	// 各種シーン
	std::unique_ptr<SceneBase> scene_;

	// カメラ
	std::shared_ptr<CameraBase> camera_;

	// シーン遷移中判定
	bool isSceneChanging_;

	// デルタタイム
	std::chrono::system_clock::time_point preTime_;
	double deltaTime_;

	// マルチプレイ判定
	bool isMultiPlayer_;

	// SceneManagerの描画スクリーン
	Unique_Graphic drawScreen_;

	// ライトハンドル
	int lightHandle1_;
	int lightHandle2_;

	// UIスクリーン
	std::shared_ptr<UIScreen> uiScreen_;

	int sound_Music_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	SceneManager(void);
	// コピーコンストラクタも同様
	SceneManager(const SceneManager&);
	// デストラクタも同様
	~SceneManager(void) = default;

	// デルタタイムをリセットする
	void ResetDeltaTime(void);

	// シーン遷移
	void DoChangeScene(SCENE_ID sceneId);

	// フェード
	void Fade(void);

};
