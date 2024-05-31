#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "Effect2D.h"

Effect2D::Effect2D(int handle)
{
    handle_ = handle;
}

void Effect2D::SetPos(VECTOR& pos)
{
    SetPosPlayingEffekseer2DEffect(GetHandle(), pos.x, pos.y, pos.z);
}

void Effect2D::SetScale(VECTOR& scl)
{
    SetScalePlayingEffekseer2DEffect(GetHandle(), scl.x, scl.y, scl.z);
}

void Effect2D::SetAngle(VECTOR& rot)
{
    SetRotationPlayingEffekseer2DEffect(GetHandle(), rot.x, rot.y, rot.z);
}

void Effect2D::SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    SetColorPlayingEffekseer2DEffect(
        GetHandle(), 
        static_cast<int>(r), 
        static_cast<int>(g), 
        static_cast<int>(b), 
        static_cast<int>(a)
    );
}

void Effect2D::SetSpeed(float speed)
{
    SetSpeedPlayingEffekseer2DEffect(GetHandle(), speed);
}

float Effect2D::GetSpeed(void) const
{
    return GetSpeedPlayingEffekseer2DEffect(GetHandle());
}

void Effect2D::Play(VECTOR& pos)
{
    SetPosPlayingEffekseer2DEffect(GetHandle(), pos.x, pos.y, pos.z);
}

bool Effect2D::IsPlaying(void) const
{
    return IsEffekseer2DEffectPlaying(GetHandle()) == -1 ? false : true;
}

void Effect2D::Stop(void) const
{
    StopEffekseer2DEffect(GetHandle());
}