#pragma once
#include <filesystem>
#include <unordered_map>
#include "../../Common/StaticSingleton.h"

class AudioList : public StaticSingleton<AudioList>
{
public:
	// Žg‚¤Œø‰Ê‰¹‚ÌŽí—Þ
	enum class AUDIO_TYPE {
		MUSIC_01 = 0,
		MUSIC_02,
		MUSIC_03,
		MUSIC_04,
		MUSIC_05,
		MUSIC_06,
		MUSIC_07,
		MUSIC_08,
		GUN_BURST_AK47,
		GUN_BURST_AWM,
		GUN_BURST_M4,
		GUN_BURST_M24R,
		_3DTEST,
		ENEMY_DAMAGE,
		ENEMY_DESTROY,
		ENMEY_SHOT,
		GUN_RELOAD_AK47,
		GUN_RELOAD_AWM,
		GUN_RELOAD_M4,
		GUN_RELOAD_M24R,
		GRENADE_EXPLOSION,
		GRENADE_HIT,
		GRENADE_SMOKE,
		PLAYER_ADS,
		PLAYER_CROUCHING,
		PLAYER_DEATH1,
		PLAYER_DEATH2,
		PLAYER_DEATH3,
		PLAYER_FOOTFALL,
		PLAYER_THROW,
		PLAYER_JUMP,
		PLAYER_JUMP_LANDING,
		GUN_EMPTY_MAGAZINE,
		SYSTEM_CHANGESCENE_SINGLE,
		SYSTEM_CHANGESCENE_MULTI,
		SYSTEM_CLICKBUTTON,
		SYSTEM_COUNTDOWN,
		SYSTEM_GAME_START,
		SYSTEM_REMAININGTIME,
		SYSTEM_RESULT,
		SYSTEM_WEAPON_SELECT,
		GRENADE_STICKY,
		SYSTEM_CANCEL,
	};

	struct AUDIO_DATA {
		std::string resourceKey;
	};

	~AudioList(void) = default;
	THIS_CLASS_IS_STATIC_SINGLETON(AudioList);

	void Init(void);
	const AUDIO_DATA& GetAudioData(AUDIO_TYPE type) const;
private:
	std::unordered_map<AUDIO_TYPE, AUDIO_DATA> audioDataList_;

	AudioList(void) = default;
	void Load(void);
};

