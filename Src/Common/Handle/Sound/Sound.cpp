#include <filesystem>
#include <string>
#include <memory>
#include <DxLib.h>
#include "Sound.h"

Sound::Sound(void) {

}

Sound::Sound(const std::filesystem::path& path, DIMENSION_TYPE dimensionType) :
	dimensionType_(dimensionType) {
	if (dimensionType == DIMENSION_TYPE::_3D) {
		// 3D
		SetCreate3DSoundFlag(true);
		handle_ = LoadSoundMem(path.string().c_str());
		SetCreate3DSoundFlag(false);
	}
	else {
		// 2D
		handle_ = LoadSoundMem(path.string().c_str());
	}

	// トータルの再生時間を取得
	totalTime_ = GetSoundTotalTime(handle_);
}

Sound::Sound(int handle, DIMENSION_TYPE dimensionType) :
	dimensionType_(dimensionType) {
	handle_ = handle;

	// トータルの再生時間を取得
	totalTime_ = GetSoundTotalTime(handle_);
}

Sound::~Sound(void) {
	if (IsAutoDeleteHandle()) {
		DeleteSoundMem(handle_);
	}
}

void Sound::Play(bool isLoop) {
	PlaySoundMem(handle_, isLoop ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK);
}

bool Sound::IsPlaying(void) {
	return CheckSoundMem(GetHandle()) == 1 ? true : false;
}

void Sound::Stop(void) {
	StopSoundMem(handle_);
}

LONGLONG Sound::GetTotalTime(void) const {
	return totalTime_;
}

void Sound::Play3D(VECTOR listnerPos, VECTOR listnerDir, VECTOR emitterPos) {
	// リスナーの位置と向きを設定
	//Set3DSoundListenerPosAndFrontPos_UpVecY(listnerPos, VAdd(listnerPos, listnerDir));

	//// 音が聞こえる距離を設定する
	//Set3DRadiusSoundMem(256.0f, GetHandle());

	//// 音の再生位置を設定
	//Set3DPositionSoundMem(emitterPos, GetHandle());

	//// 音の再生
	//Play(false);
}