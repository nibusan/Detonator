#include "../../Utility/Utility.h"
#include "../../Manager/InputManager.h"
#include "../../Application.h"
#include "../../Object/Actor/Player/Player.h"
#include "CameraBase.h"

CameraBase::CameraBase(void)
{
	pos_ = { 0.0f, 0.0f, 0.0f };
	targetPos_ = { 0.0f, 0.0f, 0.0f };
	rot_ = Quaternion::Identity();
	angles_ = Utility::VECTOR_ZERO;
}

CameraBase::~CameraBase(void)
{
}

void CameraBase::Init(void)
{
	// カメラの初期設定
	SetDefault();
}

void CameraBase::Update(void)
{

}

void CameraBase::SetBeforeDraw(void)
{
	// クリップ距離を設定する(SetDrawScreenでリセットされる)
	SetCameraNearFar(CAMERA_NEAR, CAMERA_FAR);

	
	// カメラの設定(位置と注視点による制御)
	SetCameraPositionAndTargetAndUpVec(
		pos_, 
		targetPos_, 
		cameraUp_
	);
	//SetCameraPositionAndAngle(
	//	pos_,
	//	angles_.x,
	//	angles_.y,
	//	angles_.z
	//);
}

void CameraBase::Draw(void)
{



}

void CameraBase::Release(void)
{
}

void CameraBase::NetUpdate(void)
{
}

const VECTOR& CameraBase::GetPos(void) const
{
	return pos_;
}

const VECTOR& CameraBase::GetAngle(void) const
{
	return angles_;
}

const Quaternion& CameraBase::GetRot(void) const
{
	return rot_;
}

VECTOR* CameraBase::GetCameraPos(void)
{
	return &pos_;
}

void CameraBase::SetDefault(void)
{
	// カメラの初期設定
	pos_ = DEFAULT_CAMERA_POS;

	// 注視点
	targetPos_ = VAdd(pos_, RELATIVE_C2T_POS);

	// カメラの上方向
	cameraUp_ = { 0.0f, 1.0f, 0.0f };

	// カメラはX軸に傾いているが、
	// この傾いた状態を角度ゼロ、傾き無しとする
	rot_ = Quaternion::Identity();
}

