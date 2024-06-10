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
	// �Đ�������ʉ��̃g�[�^���̒�����3���ȓ��������猳�̃T�E���h���R�s�[����
	if (sound.lock()->GetTotalTime() < 180000) {
		id = duplicateSounds_.size();
		duplicateSounds_[id] =
			std::make_shared<Sound>(
				DuplicateSoundMem(sound.lock()->GetHandle()),
				Sound::DIMENSION_TYPE::_2D
			);
		duplicateSounds_[id]->SetIsAutoDeleteHandle(true);

		// ���ʂ�ݒ�
		ChangeVolumeSoundMem(static_cast<int>(volume * 255), duplicateSounds_[id]->GetHandle());

		// �Đ����g����ݒ�
		SetFrequencySoundMem(frequencyPal, duplicateSounds_[id]->GetHandle());
		
		duplicateSounds_[id]->Play(isLoop);
	}
	else {
		id = sounds_.size();
		sounds_[id] = sound;
		
		// ���ʂ�ݒ�
		ChangeVolumeSoundMem(static_cast<int>(volume * 255), sounds_[id].lock()->GetHandle());

		// �Đ����g����ݒ�
		SetFrequencySoundMem(frequencyPal, sounds_[id].lock()->GetHandle());
		
		sounds_[id].lock()->Play(isLoop);

		// ���̌��ʉ������̂܂܎g���Ă���ꍇ��
		// id��+100000���ăR�s�[�������ʉ��Ƌ�ʂł���悤�ɂ���
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
	// �Đ�������ʉ��̃g�[�^���̒�����3���ȓ��������猳�̃T�E���h���R�s�[����
	if (sound.lock()->GetTotalTime() < 180000) {
		id = duplicateSounds_.size();
		duplicateSounds_[id] =
			std::make_shared<Sound>(
				DuplicateSoundMem(sound.lock()->GetHandle()),
				Sound::DIMENSION_TYPE::_3D
			);
		duplicateSounds_[id]->SetIsAutoDeleteHandle(true);

		// ���̍Đ��ʒu��ݒ�
		SetNextPlay3DPositionSoundMem(emitterPos, duplicateSounds_[id]->GetHandle());

		// �����������鋗����������
		SetNextPlay3DRadiusSoundMem(radius, duplicateSounds_[id]->GetHandle());

		// ���ʂ�ݒ�
		ChangeVolumeSoundMem(static_cast<int>(volume * 255), duplicateSounds_[id]->GetHandle());

		// �Đ����g����ݒ�
		SetFrequencySoundMem(frequencyPal, duplicateSounds_[id]->GetHandle());

		// ���̍Đ����J�n
		duplicateSounds_[id]->Play(isLoop);
	}
	else {
		id = sounds_.size();
		sounds_[id] = sound;

		// ���̍Đ��ʒu��ݒ�
		SetNextPlay3DPositionSoundMem(emitterPos, sounds_[id].lock()->GetHandle());

		// �����������鋗����������
		SetNextPlay3DRadiusSoundMem(radius, sounds_[id].lock()->GetHandle());

		// ���ʂ�ݒ�
		ChangeVolumeSoundMem(static_cast<int>(volume * 255), sounds_[id].lock()->GetHandle());

		// �Đ����g����ݒ�
		SetFrequencySoundMem(frequencyPal, sounds_[id].lock()->GetHandle());

		// ���̍Đ����J�n
		sounds_[id].lock()->Play(isLoop);

		// ���̌��ʉ������̂܂܎g���Ă���ꍇ��
		// id��+100000���ăR�s�[�������ʉ��Ƌ�ʂł���悤�ɂ���
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
	// �Đ�������ʉ��̃g�[�^���̒�����3���ȓ��������猳�̃T�E���h���R�s�[����
	if (sound.lock()->GetTotalTime() < 180000) {
		id = duplicateSounds_.size();
		duplicateSounds_[id] =
			std::make_shared<Sound>(
				DuplicateSoundMem(sound.lock()->GetHandle()),
				Sound::DIMENSION_TYPE::_3D
			);
		duplicateSounds_[id]->SetIsAutoDeleteHandle(true);

		// ���̍Đ��ʒu��ݒ�
		SetNextPlay3DPositionSoundMem(emitterPos, duplicateSounds_[id]->GetHandle());

		// �����������鋗����������
		SetNextPlay3DRadiusSoundMem(radius, duplicateSounds_[id]->GetHandle());

		// ���ʂ�ݒ�
		ChangeVolumeSoundMem(static_cast<int>(volume * 255), duplicateSounds_[id]->GetHandle());

		// �Đ����g����ݒ�
		SetFrequencySoundMem(frequencyPal, duplicateSounds_[id]->GetHandle());

		// ���̍Đ����J�n
		duplicateSounds_[id]->Play(isLoop);
	}
	else {
		id = sounds_.size();
		sounds_[id] = sound;

		// ���̍Đ��ʒu��ݒ�
		SetNextPlay3DPositionSoundMem(emitterPos, sounds_[id].lock()->GetHandle());

		// �����������鋗����������
		SetNextPlay3DRadiusSoundMem(radius, sounds_[id].lock()->GetHandle());

		// ���ʂ�ݒ�
		ChangeVolumeSoundMem(static_cast<int>(volume * 255), sounds_[id].lock()->GetHandle());

		// �Đ����g����ݒ�
		SetFrequencySoundMem(frequencyPal, sounds_[id].lock()->GetHandle());

		// ���̍Đ����J�n
		sounds_[id].lock()->Play(isLoop);

		// ���̌��ʉ������̂܂܎g���Ă���ꍇ��
		// id��+100000���ăR�s�[�������ʉ��Ƌ�ʂł���悤�ɂ���
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

		// ���̍Đ��ʒu��ݒ�
		SetNextPlay3DPositionSoundMem(emitterPos, sounds_[handle - 100000].lock()->GetHandle());

		// �����������鋗����������
		SetNextPlay3DRadiusSoundMem(radius, sounds_[handle - 100000].lock()->GetHandle());
	}
	else {
		Set3DPresetReverbParamSoundMem(duplicateSounds_[handle]->GetHandle(), DX_REVERB_PRESET_UNDERWATER);

		// ���̍Đ��ʒu��ݒ�
		SetNextPlay3DPositionSoundMem(emitterPos, duplicateSounds_[handle]->GetHandle());

		// �����������鋗����������
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
