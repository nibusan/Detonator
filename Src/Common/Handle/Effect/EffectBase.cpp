#include <memory>
#include "../Graphic/Graphic.h"
#include "../HandleBase.h"
#include "EffectBase.h"

EffectBase::EffectBase(void)
{
	
}

EffectBase::EffectBase(const std::filesystem::path& path)
{
	handle_ = LoadEffekseerEffect(path.string().c_str());
}

EffectBase::EffectBase(int handle)
{
	handle_ = handle;
}

EffectBase::~EffectBase(void)
{
	if (IsAutoDeleteHandle())
	{
		DeleteEffekseerEffect(handle_);
	}
}

void EffectBase::SetPos(const VECTOR& pos) {}

void EffectBase::SetScale(const VECTOR& scl) {}

void EffectBase::SetAngle(const VECTOR& rot) {}

void EffectBase::SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {}

void EffectBase::SetSpeed(float speed) {}

float EffectBase::GetSpeed(void) const {
	return 0.0f;
}

void EffectBase::Play(const VECTOR& pos) {}

bool EffectBase::IsPlaying(void) const {
	return false;
}

void EffectBase::Stop(void) const {}
