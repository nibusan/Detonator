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
	
	/// @brief 音量を設定
	/// @param handle ハンドルID
	/// @param volume 音量(0.0fが無音 1.0fが最大)
	void ChangeVolume(int handle, float volume);

	/// @brief 再生周波数を設定
	/// @param handle ハンドルID
	/// @param frequencyPal 再生周波数(ヘルツ単位)
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