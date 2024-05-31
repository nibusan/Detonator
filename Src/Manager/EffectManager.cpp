#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "../Common/Handle/Effect/EffectBase.h"
#include "../Common/Handle/Effect/2D/Effect2D.h"
#include "../Common/Handle/Effect/3D/Effect3D.h"
#include "../Common/nlohmann-json/json.hpp"
#include "../Manager/GameManager.h"
#include "EffectManager.h"

EffectManager* EffectManager::instance_ = nullptr;

EffectManager::EffectManager(void)
{

}

EffectManager::EffectManager(const EffectManager&)
{

}

void EffectManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new EffectManager();
	}
	instance_->Init();
}

EffectManager& EffectManager::GetInstance(void)
{
	return *instance_;
}

void EffectManager::Init(void)
{
	
	LoadEffectList();
}

void EffectManager::Update(void)
{
	// エフェクトの更新
	UpdateEffekseer3D();
}

void EffectManager::Draw(void)
{
	//DrawEffekseer2D();
	DrawEffekseer3D();
}

void EffectManager::Release(void)
{
	
}

void EffectManager::Destroy(void)
{
	Effkseer_End();
	delete instance_;
}

int EffectManager::AddEffect(EFFECT& effect)
{
	const auto& targetEffectResource = GetEffectResource(effect.type);
	int newEffectID = effects_.size();
	switch (targetEffectResource.dimensionType)
	{
	case DIMENSION_TYPE::_2D:
	{
		auto play2DEffect = std::make_shared<Effect2D>(GetPlay2DEffectHandle(targetEffectResource));
		play2DEffect->SetPos(effect.pos);
		play2DEffect->SetScale(effect.scl);
		play2DEffect->SetAngle(effect.rot);
		play2DEffect->SetSpeed(effect.speed);
		effects_[newEffectID] = play2DEffect;
		break;
	}
	case DIMENSION_TYPE::_3D:
	{
		auto play3DEffect = std::make_shared<Effect3D>(GetPlay3DEffectHandle(targetEffectResource));
		play3DEffect->SetPos(effect.pos);
		play3DEffect->SetScale(effect.scl);
		play3DEffect->SetAngle(effect.rot);
		play3DEffect->SetSpeed(effect.speed);
		effects_[newEffectID] = play3DEffect;
		break;
	}
	default:
		break;
	}

	return newEffectID;
}

void EffectManager::DeleteEffect(int id) 
{
	effects_.erase(id);
}

const std::weak_ptr<EffectBase> EffectManager::GetData(int id) const 
{
	return effects_.at(id);
}

void EffectManager::LoadEffectList(void)
{
	std::ifstream ifs("Data/Data/Effect/EffectList.json");
	nlohmann::json effectList;
	ifs >> effectList;

	std::filesystem::path basePath = effectList["Effect"]["BasePath"];
	for (int i = 0; i < effectList["Effect"]["EffectData"].size(); i++)
	{
		std::filesystem::path calcPath = basePath / effectList["Effect"]["EffectData"][i]["Path"];
		DIMENSION_TYPE dimensionType = effectList["Effect"]["EffectData"][i]["DimensionType"];
		std::shared_ptr<EffectBase> effect = std::make_shared<EffectBase>(calcPath);

		EFFECT_DATA addEffect = {
			calcPath,
			dimensionType,
			effect
		};

		effectList_[effectList["Effect"]["EffectData"][i]["ID"]] = addEffect;
	}
}

const EffectManager::EFFECT_DATA& EffectManager::GetEffectResource(EFFECT_TYPE type)
{
	return effectList_[static_cast<int>(type)];
}

int EffectManager::GetPlay2DEffectHandle(const EFFECT_DATA& resourceEffect) const
{
	return PlayEffekseer2DEffect(resourceEffect.resource->GetHandle());
}


int EffectManager::GetPlay3DEffectHandle(const EFFECT_DATA& resourceEffect) const
{
	return PlayEffekseer3DEffect(resourceEffect.resource->GetHandle());
}
