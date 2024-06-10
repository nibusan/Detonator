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

	/// @brief 座標をセット
	/// @param pos 座標
	virtual void SetPos(const VECTOR& pos);

	/// @brief スケール値をセット
	/// @param scl スケール値
	virtual void SetScale(const VECTOR& scl);

	/// @brief 角度をセット
	/// @param rot 角度
	virtual void SetAngle(const VECTOR& rot);

	/// @brief 色をセット
	/// @param r Red
	/// @param g Green
	/// @param b Blue
	/// @param a Alpha
	virtual void SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	
	/// @brief スピードをセット
	/// @param speed スピード
	virtual void SetSpeed(float speed);
	
	/// @brief スピードを返す
	/// @return スピード
	virtual float GetSpeed(void) const;
	
	/// @brief エフェクトを再生
	/// @param pos 座標
	virtual void Play(const VECTOR& pos);
	
	/// @brief 再生されてるかを返す
	/// @return 再生されてるか
	virtual bool IsPlaying(void) const;
	
	/// @brief エフェクトを停止させる
	virtual void Stop(void) const;
};