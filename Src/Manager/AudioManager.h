#pragma once
#include <memory>
#include <unordered_map>
#include <DxLib.h>
#include "../Common/StaticSingleton.h"
#include "../Common/Handle/Sound/Sound.h"
#include "../Data/Audio/AudioList.h"

class AudioManager : public StaticSingleton<AudioManager>
{
public:
	~AudioManager(void) = default;
	THIS_CLASS_IS_STATIC_SINGLETON(AudioManager);

	void Init(void);
	void Release(void);

	int Play(AudioList::AUDIO_TYPE type, bool isLoop, float volume, int frequencyPal);

	int Play(
		AudioList::AUDIO_TYPE type, 
		const VECTOR& listenerPos,
		const VECTOR& listenerDir, 
		const VECTOR& emitterPos, 
		float radius, 
		bool isLoop, 
		float volume,
		int frequencyPal
	);
	
	int Play(
		AudioList::AUDIO_TYPE type, 
		const VECTOR& emitterPos, 
		float radius,
		bool isLoop,
		float volume,
		int frequencyPal = -1
	);

	void PlayInTheMiddle(int handle, bool isLoop);
	void Stop(int handle);
	void Delete(int handle);

	void Update_3DSound(int handle, const VECTOR& listenerPos, const VECTOR& listenerDir, const VECTOR& emitterPos, float radius);
	
	/// @brief ���ʂ�ݒ�
	/// @param handle �n���h��ID
	/// @param volume ����(0.0f������ 1.0f���ő�)
	void ChangeVolume(int handle, float volume);

	/// @brief �Đ����g����ݒ�
	/// @param handle �n���h��ID
	/// @param frequencyPal �Đ����g��(�w���c�P��)
	void ChangeFrequency(int handle, int frequencyPal);
	
	Weak_Sound GetSound(int handle) const;

	void SetListenerInfo(VECTOR* pos, VECTOR* dir);
private:
	std::unordered_map<int, Shared_Sound> duplicateSounds_;
	std::unordered_map<int, Weak_Sound> sounds_;

	VECTOR* listenerPos_;
	VECTOR* listenerDir_;

	AudioManager(void) = default;
};