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

	// �g�[�^���̍Đ����Ԃ��擾
	totalTime_ = GetSoundTotalTime(handle_);
}

Sound::Sound(int handle, DIMENSION_TYPE dimensionType) :
	dimensionType_(dimensionType) {
	handle_ = handle;

	// �g�[�^���̍Đ����Ԃ��擾
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
	// ���X�i�[�̈ʒu�ƌ�����ݒ�
	//Set3DSoundListenerPosAndFrontPos_UpVecY(listnerPos, VAdd(listnerPos, listnerDir));

	//// �����������鋗����ݒ肷��
	//Set3DRadiusSoundMem(256.0f, GetHandle());

	//// ���̍Đ��ʒu��ݒ�
	//Set3DPositionSoundMem(emitterPos, GetHandle());

	//// ���̍Đ�
	//Play(false);
}