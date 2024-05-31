#include "../../Utility/Utility.h"
#include "NormalCamera.h"

NormalCamera::NormalCamera(void)
{
}

NormalCamera::~NormalCamera(void)
{
}

void NormalCamera::Init(void)
{
	// �J�����̏����p�x
	angles_ = Utility::VECTOR_ZERO;
	//angles_.x = Utility::Deg2RadF(30.0f);

	pos_ = {};
	targetPos_ = {0.0f,0.0f,500.0f};
	cameraUp_ = { 0.0f,1.0f,0.0f };
}

void NormalCamera::Update(void)
{
}

void NormalCamera::SetBeforeDraw(void)
{
	// �N���b�v������ݒ肷��(SetDrawScreen�Ń��Z�b�g�����)
	SetCameraNearFar(CAMERA_NEAR, CAMERA_FAR);

	switch (mode_)
	{
	case NormalCamera::MODE::FIXED_POINT:
		SetBeforeDrawFixedPoint();
		break;
	}

	// �J�����̐ݒ�(�ʒu�ƒ����_�ɂ�鐧��)
	SetCameraPositionAndTargetAndUpVec(
		pos_,
		targetPos_,
		cameraUp_
	);
}

void NormalCamera::SetBeforeDrawFixedPoint(void)
{
}

