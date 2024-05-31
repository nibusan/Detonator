#include <DxLib.h>
#include "../../Manager/SceneManager.h"
#include "../../Utility/Utility.h"
#include "Transform.h"

Transform::Transform(void)
{
	modelId = -1;

	scl = Utility::VECTOR_ONE;
	rot = Utility::VECTOR_ZERO;
	pos = Utility::VECTOR_ZERO;
	localPos = Utility::VECTOR_ZERO;
	prePos = pos;

	matScl = MGetIdent();
	matRot = MGetIdent();
	matPos = MGetIdent();
	quaRot = Quaternion();
	quaRotLocal = Quaternion();


	// ��\���ɂ��邩
	isHide = false;

	// �d�͂�ON�ɂ��邩
	isGravity = false;

	// �d�͉����x
	gravitySpeed_ = 0.0f;

	// �d�͌W��
	heavy = 1.0f;
}


Transform::~Transform(void)
{
}

void Transform::Update(void)
{

	// �傫��
	matScl = MGetScale(scl);

	// ��]
	rot = quaRot.ToEuler();
	matRot = quaRot.ToMatrix();

	// �ʒu
	// ���[�J��
	auto lPos = VAdd(pos, quaRot.PosAxis(localPos));
	matPos = MGetTranslate(lPos);

	// �s��̍���
	MATRIX mat = MGetIdent();
	mat = MMult(mat, matScl);					// �傫��
	Quaternion q = quaRot.Mult(quaRotLocal);	// ���݂̉�]�ʂɃ��[�J����]�ʂ�����
	mat = MMult(mat, q.ToMatrix());				// ��](�N�H�[�^�j�I������s��֕ϊ�)
	mat = MMult(mat, matPos);					// �ʒu



	// �s������f���ɔ���
	if (modelId != -1)
	{
		MV1SetMatrix(modelId, mat);
	}

}

void Transform::Draw(void)
{
	// ��\����ԂȂ�\�����Ȃ�
	if (isHide) return;

	MV1DrawModel(modelId);
}

void Transform::Release(void)
{
	MV1DeleteModel(modelId);
}

void Transform::SetModel(int model)
{
	modelId = model;
}

void Transform::PreUpdate(void)
{
	// �傫��
	preScl = scl;
	// ��]
	preRot = rot;
	// �ʒu
	prePos = pos;

	// ��]
	preQuaRot = quaRot;
}


VECTOR Transform::GetForward(void) const
{
	return GetDir(Utility::DIR_F);
}

VECTOR Transform::GetBack(void) const
{
	return GetDir(Utility::DIR_B);
}

VECTOR Transform::GetRight(void) const
{
	return GetDir(Utility::DIR_R);
}

VECTOR Transform::GetLeft(void) const
{
	return GetDir(Utility::DIR_L);
}

VECTOR Transform::GetUp(void) const
{
	return GetDir(Utility::DIR_U);
}

VECTOR Transform::GetDown(void) const
{
	return GetDir(Utility::DIR_D);
}

VECTOR Transform::GetDir(const VECTOR& vec) const
{
	return quaRot.PosAxis(vec);
}

void Transform::ResetGravityAccel(void)
{
	// �d�͉����x
	gravitySpeed_ = 0.0f;
}

const float& Transform::GetGravityAccel(void) const
{
	return gravitySpeed_;
}

void Transform::Jump(float pow)
{
	// �d�͉����x
	gravitySpeed_ -= pow;
}

bool Transform::IsRising(void)
{
	return (gravitySpeed_ < 0.0f);
}

bool Transform::IsFalling(void)
{
	return (gravitySpeed_ >= 0.0f);
}

void Transform::HideFrame(int frameNo)
{

	// �Ώۃt���[���̃��[�J���s��(�傫���A��]�A�ʒu)���擾����
	auto mat = MV1GetFrameLocalMatrix(modelId, frameNo);
	auto scl = MGetSize(mat);			// �s�񂩂�傫�������o��
	auto rot = MGetRotElem(mat);		// �s�񂩂��]�����o��
	auto pos = MGetTranslateElem(mat);	// �s�񂩂�ړ��l�����o��

	// �傫���A��]�A�ʒu�����[�J���s��ɖ߂�
	MATRIX mix = MGetIdent();
	mix = MMult(mix, MGetScale({ 0.0f,0.0f,0.0f }));	// �傫����0�ɂ���
	mix = MMult(mix, rot);							// ��]
	mix = MMult(mix, MGetTranslate(pos));			// ���W

	// ���������s���Ώۃt���[���ɃZ�b�g������
	MV1SetFrameUserLocalMatrix(modelId, frameNo, mix);
}

void Transform::AppearFrame(int frameNo)
{
	// �Ώۃt���[���̃��[�J���s��(�傫���A��]�A�ʒu)���擾����
	auto mat = MV1GetFrameLocalMatrix(modelId, frameNo);
	auto scl = MGetSize(mat);			// �s�񂩂�傫�������o��
	auto rot = MGetRotElem(mat);		// �s�񂩂��]�����o��
	auto pos = MGetTranslateElem(mat);	// �s�񂩂�ړ��l�����o��

	// �傫���A��]�A�ʒu�����[�J���s��ɖ߂�
	MATRIX mix = MGetIdent();
	mix = MMult(mix, MGetScale({ 1.0f,1.0f,1.0f }));	// �傫����1�ɂ���
	mix = MMult(mix, rot);							// ��]
	mix = MMult(mix, MGetTranslate(pos));			// ���W

	// ���������s���Ώۃt���[���ɃZ�b�g������
	MV1SetFrameUserLocalMatrix(modelId, frameNo, mix);
}

const bool Transform::IsHidingFrame(int frameNo)
{
	// �Ώۃt���[���̃��[�J���s��(�傫���A��]�A�ʒu)���擾����
	auto mat = MV1GetFrameLocalMatrix(modelId, frameNo);
	auto scl = MGetSize(mat);			// �s�񂩂�傫�������o��

	return Utility::EqualsVZero(scl);
}

void Transform::Gravity(void)
{
	if (!isGravity)return;

	// �d�͉����x
	gravitySpeed_ += SceneManager::GetInstance().GetDeltaMult(GRAVITY * heavy);

	// ����
	pos = VAdd(pos, VScale({ 0.0f,-1.0f,0.0f }, gravitySpeed_));
}

