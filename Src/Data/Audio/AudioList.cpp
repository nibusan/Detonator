#include <fstream>
#include <iostream>
#include "../../Common/nlohmann-json/json.hpp"
#include "AudioList.h"

void AudioList::Init(void) {
	Load();
}

const AudioList::AUDIO_DATA& AudioList::GetAudioData(AUDIO_TYPE type) const {
	return audioDataList_.at(type);
}

void AudioList::Load(void) {
	std::ifstream ifs("Data/Data/Audio/AudioList.json");
	nlohmann::json audioList;
	ifs >> audioList;
	for (const auto& audio : audioList["AudioList"]) {
		AUDIO_DATA audioData;
		audioData.resourceKey = audio["ResourceKey"];
		audioDataList_[audio["ID"]] = audioData;
	}
}
