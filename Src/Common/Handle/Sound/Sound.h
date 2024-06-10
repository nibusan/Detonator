#pragma once
#include <filesystem>
#include <string>
#include <DxLib.h>
#include "../HandleBase.h"

class Sound : public HandleBase {
public:
	/// @brief 2Dの音か3Dの音か
	enum class DIMENSION_TYPE {
		_2D = 0,
		_3D
	};

	Sound(void);

	/// @brief コンストラクタ
	/// @param path ファイルへのパス
	/// @param dimensionType  2D or 3Dのどちらか
	Sound(const std::filesystem::path& path, DIMENSION_TYPE dimensionType);

	/// @brief コンストラクタ
	/// @param handle ハンドル
	/// @param dimensionType 2D or 3Dのどちらか
	Sound(int handle, DIMENSION_TYPE dimensionType);

	~Sound(void) override;

	/// @brief サウンドの長さを返す
	/// @return サウンドの長さ
	[[nodiscard]] LONGLONG GetTotalTime(void) const;

	/// @brief 音を再生(2D)
	/// @param isLoop ループするか
	void Play(bool isLoop);

	/// @brief 音が再生かを返す(2D)
	/// @return 再生中かどうか
	bool IsPlaying(void);

	/// @brief 音を止める(2D)
	void Stop(void);

	/// @brief 音を再生(3D)
	/// @param listnerPos 聞く人の座標
	/// @param listnerDir 聞く人が向いてる方向
	/// @param emitterPos 音を鳴らす座標
	void Play3D(VECTOR listnerPos, VECTOR listnerDir, VECTOR emitterPos);
private:
	// トータルの再生時間
	LONGLONG totalTime_;

	// 2Dか3Dか
	DIMENSION_TYPE dimensionType_;
};

using Unique_Sound = std::unique_ptr<Sound>;
using Shared_Sound = std::shared_ptr<Sound>;
using Weak_Sound = std::weak_ptr<Sound>;