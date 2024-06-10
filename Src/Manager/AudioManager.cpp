#include "../Manager/ResourceManager.h"
#include "AudioManager.h"

void AudioManager::Init(void) {
	sounds_.clear();
}

void AudioManager::Release(void) {

}

int AudioManager::Play(AudioList::AUDIO_TYPE type, bool isLoop, float volume, int frequencyPal) {
	auto& audioList = AudioList::GetInstance();
	auto& resourceManager = ResourceManager::GetInstance();
	int id;
	Weak_Sound sound = std::dynamic_pointer_cast<Sound>(resourceManager.GetResourceFile(audioList.GetAudioData(type).resourceKey));
	// 再生する効果音のトータルの長さが3分以内だったら元のサウンドをコピーする
	if (sound.lock()->GetTotalTime() < 180000) {
		id = duplicateSounds_.size();
		duplicateSounds_[id] =
			std::make_shared<Sound>(
				DuplicateSoundMem(sound.lock()->GetHandle()),
				Sound::DIMENSION_TYPE::_2D
			);
		duplicateSounds_[id]->SetIsAutoDeleteHandle(true);

		// 音量を設定
		ChangeVolumeSoundMem(static_cast<int>(volume * 255), duplicateSounds_[id]->GetHandle());

		// 再生周波数を設定
		SetFrequencySoundMem(frequencyPal, duplicateSounds_[id]->GetHandle());
		
		duplicateSounds_[id]->Play(isLoop);
	}
	else {
		id = sounds_.size();
		sounds_[id] = sound;
		
		// 音量を設定
		ChangeVolumeSoundMem(static_cast<int>(volume * 255), sounds_[id].lock()->GetHandle());

		// 再生周波数を設定
		SetFrequencySoundMem(frequencyPal, sounds_[id].lock()->GetHandle());
		
		sounds_[id].lock()->Play(isLoop);

		// 元の効果音をそのまま使っている場合は
		// idに+100000してコピーした効果音と区別できるようにする
		id += 100000;
	}
    return id;
}

int AudioManager::Play(
	AudioList::AUDIO_TYPE type, 
	const VECTOR& listenerPos, 
	const VECTOR& listenerDir, 
	const VECTOR& emitterPos, 
	float radius, 
	bool isLoop, 
	float volume, int frequencyPal
) {
	auto& audioList = AudioList::GetInstance();
	auto& resourceManager = ResourceManager::GetInstance();
	int id;
	Weak_Sound sound = std::dynamic_pointer_cast<Sound>(resourceManager.GetResourceFile(audioList.GetAudioData(type).resourceKey));

	Set3DSoundListenerPosAndFrontPos_UpVecY(listenerPos, VAdd(listenerPos, listenerDir));
	// 再生する効果音のトータルの長さが3分以内だったら元のサウンドをコピーする
	if (sound.lock()->GetTotalTime() < 180000) {
		id = duplicateSounds_.size();
		duplicateSounds_[id] =
			std::make_shared<Sound>(
				DuplicateSoundMem(sound.lock()->GetHandle()),
				Sound::DIMENSION_TYPE::_3D
			);
		duplicateSounds_[id]->SetIsAutoDeleteHandle(true);

		// 音の再生位置を設定
		SetNextPlay3DPositionSoundMem(emitterPos, duplicateSounds_[id]->GetHandle());

		// 音が聞こえる距離を初期化
		SetNextPlay3DRadiusSoundMem(radius, duplicateSounds_[id]->GetHandle());

		// 音量を設定
		ChangeVolumeSoundMem(static_cast<int>(volume * 255), duplicateSounds_[id]->GetHandle());

		// 再生周波数を設定
		SetFrequencySoundMem(frequencyPal, duplicateSounds_[id]->GetHandle());

		// 音の再生を開始
		duplicateSounds_[id]->Play(isLoop);
	}
	else {
		id = sounds_.size();
		sounds_[id] = sound;

		// 音の再生位置を設定
		SetNextPlay3DPositionSoundMem(emitterPos, sounds_[id].lock()->GetHandle());

		// 音が聞こえる距離を初期化
		SetNextPlay3DRadiusSoundMem(radius, sounds_[id].lock()->GetHandle());

		// 音量を設定
		ChangeVolumeSoundMem(static_cast<int>(volume * 255), sounds_[id].lock()->GetHandle());

		// 再生周波数を設定
		SetFrequencySoundMem(frequencyPal, sounds_[id].lock()->GetHandle());

		// 音の再生を開始
		sounds_[id].lock()->Play(isLoop);

		// 元の効果音をそのまま使っている場合は
		// idに+100000してコピーした効果音と区別できるようにする
		id += 100000;
	}


	return id;
}

int AudioManager::Play(AudioList::AUDIO_TYPE type, 
	const VECTOR& emitterPos, 
	float radius, 
	bool isLoop, 
	float volume, 
	int frequencyPal
) {
	auto& audioList = AudioList::GetInstance();
	auto& resourceManager = ResourceManager::GetInstance();
	int id;
	Weak_Sound sound = std::dynamic_pointer_cast<Sound>(resourceManager.GetResourceFile(audioList.GetAudioData(type).resourceKey));

	Set3DSoundListenerPosAndFrontPos_UpVecY(*listenerPos_, VAdd(*listenerPos_, *listenerDir_));
	// 再生する効果音のトータルの長さが3分以内だったら元のサウンドをコピーする
	if (sound.lock()->GetTotalTime() < 180000) {
		id = duplicateSounds_.size();
		duplicateSounds_[id] =
			std::make_shared<Sound>(
				DuplicateSoundMem(sound.lock()->GetHandle()),
				Sound::DIMENSION_TYPE::_3D
			);
		duplicateSounds_[id]->SetIsAutoDeleteHandle(true);

		// 音の再生位置を設定
		SetNextPlay3DPositionSoundMem(emitterPos, duplicateSounds_[id]->GetHandle());

		// 音が聞こえる距離を初期化
		SetNextPlay3DRadiusSoundMem(radius, duplicateSounds_[id]->GetHandle());

		// 音量を設定
		ChangeVolumeSoundMem(static_cast<int>(volume * 255), duplicateSounds_[id]->GetHandle());

		// 再生周波数を設定
		SetFrequencySoundMem(frequencyPal, duplicateSounds_[id]->GetHandle());

		// 音の再生を開始
		duplicateSounds_[id]->Play(isLoop);
	}
	else {
		id = sounds_.size();
		sounds_[id] = sound;

		// 音の再生位置を設定
		SetNextPlay3DPositionSoundMem(emitterPos, sounds_[id].lock()->GetHandle());

		// 音が聞こえる距離を初期化
		SetNextPlay3DRadiusSoundMem(radius, sounds_[id].lock()->GetHandle());

		// 音量を設定
		ChangeVolumeSoundMem(static_cast<int>(volume * 255), sounds_[id].lock()->GetHandle());

		// 再生周波数を設定
		SetFrequencySoundMem(frequencyPal, sounds_[id].lock()->GetHandle());

		// 音の再生を開始
		sounds_[id].lock()->Play(isLoop);

		// 元の効果音をそのまま使っている場合は
		// idに+100000してコピーした効果音と区別できるようにする
		id += 100000;
	}


	return id;
}

void AudioManager::PlayInTheMiddle(int handle, bool isLoop) {
	if (handle >= 100000) {
		sounds_[handle - 100000].lock()->Play(isLoop);
	}
	else {
		duplicateSounds_[handle]->Play(isLoop);
	}
}

void AudioManager::Stop(int handle) {
	if (handle >= 100000) {
		sounds_[handle - 100000].lock()->Stop();
	}
	else {
		duplicateSounds_[handle]->Stop();
	}
}

void AudioManager::Delete(int handle) {
	if (handle >= 100000) {
		sounds_.erase(handle - 100000);
	}
	else {
		duplicateSounds_.erase(handle);	
	}
}

void AudioManager::Update_3DSound(int handle, const VECTOR& listenerPos, const VECTOR& listenerDir, const VECTOR& emitterPos, float radius) {
	Set3DSoundListenerPosAndFrontPos_UpVecY(listenerPos, VAdd(listenerPos, listenerDir));
	if (handle >= 100000) {
		Set3DPresetReverbParamSoundMem(sounds_[handle - 100000].lock()->GetHandle(), DX_REVERB_PRESET_UNDERWATER);

		// 音の再生位置を設定
		SetNextPlay3DPositionSoundMem(emitterPos, sounds_[handle - 100000].lock()->GetHandle());

		// 音が聞こえる距離を初期化
		SetNextPlay3DRadiusSoundMem(radius, sounds_[handle - 100000].lock()->GetHandle());
	}
	else {
		Set3DPresetReverbParamSoundMem(duplicateSounds_[handle]->GetHandle(), DX_REVERB_PRESET_UNDERWATER);

		// 音の再生位置を設定
		SetNextPlay3DPositionSoundMem(emitterPos, duplicateSounds_[handle]->GetHandle());

		// 音が聞こえる距離を初期化
		SetNextPlay3DRadiusSoundMem(radius, duplicateSounds_[handle]->GetHandle());
	}
}

void AudioManager::ChangeVolume(int handle, float volume) {
	if (handle >= 100000) {
		ChangeVolumeSoundMem(static_cast<int>(volume * 255), sounds_[handle - 100000].lock()->GetHandle());
	}
	else {
		ChangeVolumeSoundMem(static_cast<int>(volume * 255), duplicateSounds_[handle]->GetHandle());
	}
}

void AudioManager::ChangeFrequency(int handle, int frequencyPal) {
	if (handle >= 100000) {
		SetFrequencySoundMem(frequencyPal, sounds_[handle - 100000].lock()->GetHandle());
	}
	else {
		SetFrequencySoundMem(frequencyPal, duplicateSounds_[handle]->GetHandle());
	}
}

std::weak_ptr<Sound> AudioManager::GetSound(int handle) const {
	return sounds_.at(handle);
}

void AudioManager::SetListenerInfo(VECTOR* pos, VECTOR* dir)
{
	listenerPos_ = pos;
	listenerDir_ = dir;
}
