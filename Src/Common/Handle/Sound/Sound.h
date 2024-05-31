#pragma once
#include <filesystem>
#include <string>
#include <DxLib.h>
#include "../HandleBase.h"

class Sound : public HandleBase
{

public:
	enum class DIMENSION_TYPE {
		_2D = 0,
		_3D
	};

private:
	LONGLONG totalTime_;	//トータルの再生時間
	DIMENSION_TYPE dimensionType_;	//

public:
	Sound(void);

	/*!
	@brief コンストラクタ
	@param path ファイルへのパス
	@param dimensionType 2D or 3Dのどちらか
	*/
	Sound(const std::filesystem::path& path, DIMENSION_TYPE dimensionType);

	/*!
	@brief コンストラクタ
	@param handle ハンドル
	@param dimensionType 2D or 3Dのどちらか
	*/
	Sound(int handle, DIMENSION_TYPE dimensionType);

	~Sound(void) override;

	/*!
	@brief サウンドの長さを返す
	@return LONGLONG型のサウンドの長さ
	*/
	[[nodiscard]] LONGLONG GetTotalTime(void) const;
	
	// 2D
	void Play(bool isLoop);
	bool IsPlaying(void);
	void Stop(void);

	// 3D
	//void Play3D(VECTOR listnerPos, VECTOR listnerDir, VECTOR emitterPos);
	//void Set3DPlayPos(VECTOR pos);
	//void Set3DRadius(float radius);

};

using Unique_Sound = std::unique_ptr<Sound>;
using Shared_Sound = std::shared_ptr<Sound>;
using Weak_Sound = std::weak_ptr<Sound>;