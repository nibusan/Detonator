#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "Effect3D.h"

Effect3D::Effect3D(int handle)
{
    handle_ = handle;
}

Effect3D::Effect3D(const Effect3D& ins)
{
}

void Effect3D::SetPos(VECTOR& pos)
{
    SetPosPlayingEffekseer3DEffect(GetHandle(), pos.x, pos.y, pos.z);
}

void Effect3D::SetScale(VECTOR& scl)
{
    SetScalePlayingEffekseer3DEffect(GetHandle(), scl.x, scl.y, scl.z);
}

void Effect3D::SetAngle(VECTOR& rot)
{
    SetRotationPlayingEffekseer3DEffect(GetHandle(), rot.x, rot.y, rot.z);
}

void Effect3D::SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    SetColorPlayingEffekseer3DEffect(
        GetHandle(),
        static_cast<int>(r),
        static_cast<int>(g),
        static_cast<int>(b),
        static_cast<int>(a)
    );
}

void Effect3D::SetSpeed(float speed)
{
    SetSpeedPlayingEffekseer3DEffect(GetHandle(), speed);
}

float Effect3D::GetSpeed(void) const
{
    return GetSpeedPlayingEffekseer3DEffect(GetHandle());
}

void Effect3D::Play(VECTOR& pos)
{
    SetPosPlayingEffekseer3DEffect(GetHandle(), pos.x, pos.y, pos.z);
}

bool Effect3D::IsPlaying(void) const
{
    return IsEffekseer3DEffectPlaying(GetHandle()) == -1 ? false : true;
}

void Effect3D::Stop(void) const
{
    StopEffekseer3DEffect(GetHandle());
}