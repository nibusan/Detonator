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
	// カメラの初期角度
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
	// クリップ距離を設定する(SetDrawScreenでリセットされる)
	SetCameraNearFar(CAMERA_NEAR, CAMERA_FAR);

	switch (mode_)
	{
	case NormalCamera::MODE::FIXED_POINT:
		SetBeforeDrawFixedPoint();
		break;
	}

	// カメラの設定(位置と注視点による制御)
	SetCameraPositionAndTargetAndUpVec(
		pos_,
		targetPos_,
		cameraUp_
	);
}

void NormalCamera::SetBeforeDrawFixedPoint(void)
{
}

