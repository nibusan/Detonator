#pragma once
#include <filesystem>
#include <memory> 
#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "../HandleBase.h"
#include "../Graphic/Graphic.h"

class EffectBase : public HandleBase
{

public:
	EffectBase(void);
	EffectBase(const std::filesystem::path& path);
	EffectBase(int handle);

	~EffectBase(void) override;

	// 座標をセット
	virtual void SetPos(VECTOR& pos);

	// スケール値をセット
	virtual void SetScale(VECTOR& scl);

	// 角度をセット
	virtual void SetAngle(VECTOR& rot);

	// 色をセット
	virtual void SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	
	// スピードをセット
	virtual void SetSpeed(float speed);
	
	// スピードを返す
	virtual float GetSpeed(void) const;
	
	// エフェクトを再生
	virtual void Play(VECTOR& pos);
	
	// 再生されてるかを返す
	virtual bool IsPlaying(void) const;
	
	// エフェクトを停止させる
	virtual void Stop(void) const;
};

using Unique_Effect = std::unique_ptr<EffectBase>;
using Shared_Effect = std::shared_ptr<EffectBase>;