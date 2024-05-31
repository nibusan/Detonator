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
	// �J�����̏����ݒ�
	SetDefault();
}

void CameraBase::Update(void)
{

}

void CameraBase::SetBeforeDraw(void)
{
	// �N���b�v������ݒ肷��(SetDrawScreen�Ń��Z�b�g�����)
	SetCameraNearFar(CAMERA_NEAR, CAMERA_FAR);

	
	// �J�����̐ݒ�(�ʒu�ƒ����_�ɂ�鐧��)
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
	// �J�����̏����ݒ�
	pos_ = DEFAULT_CAMERA_POS;

	// �����_
	targetPos_ = VAdd(pos_, RELATIVE_C2T_POS);

	// �J�����̏����
	cameraUp_ = { 0.0f, 1.0f, 0.0f };

	// �J������X���ɌX���Ă��邪�A
	// ���̌X������Ԃ��p�x�[���A�X�������Ƃ���
	rot_ = Quaternion::Identity();
}

