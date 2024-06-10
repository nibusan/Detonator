#pragma once
#include <DxLib.h>
#include "../EffectBase.h"

class Effect3D final : public EffectBase
{

public:
	Effect3D(void) = default;
	Effect3D(int handle);
	~Effect3D(void) = default;
	Effect3D(const Effect3D& ins);

	void SetPos(const VECTOR& pos) override;
	void SetScale(const VECTOR& scl) override;
	void SetAngle(const VECTOR& rot) override;
	void SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) override;
	void SetSpeed(float speed) override;
	float GetSpeed(void) const override;
	void Play(const VECTOR& pos) override;
	bool IsPlaying(void) const override;
	void Stop(void) const override;
};
