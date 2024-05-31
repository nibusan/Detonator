#pragma once
#include <DxLib.h>
#include "../../Common/Quaternion.h"

/// <summary>
/// ���f������̊�{���
/// �傫���FVECTOR�
/// ��]�@�FQuaternion�
/// �ʒu�@�FVECTOR�
/// </summary>
class Transform
{

public:

	// �R���X�g���N�^
	Transform(void);
	//Object(int model);				// ���f��
	//Object(float rad);				// ����
	//Object(float rad,float len);	// �J�v�Z��

	// �f�X�g���N�^
	~Transform(void);



	/// <summary>
	/// ���f������̊�{���X�V
	/// </summary>
	/// <param name=""></param>
	void Update(void);

	void Draw(void);

	void Release(void);

	void SetModel(int modelHId);

	// 1�t���[���O�X�V
	// �X�V�O���W�����g���ꍇ
	// ���̊֐����e�X�V������1�ԍŏ���
	// 1�x�����Ăяo��
	void PreUpdate(void);


	// �O���������擾
	VECTOR GetForward(void) const;

	// ����������擾
	VECTOR GetBack(void) const;

	// �E�������擾
	VECTOR GetRight(void) const;

	// ���������擾
	VECTOR GetLeft(void) const;

	// ��������擾
	VECTOR GetUp(void) const;

	// ���������擾
	VECTOR GetDown(void) const;

	// �Ώە������擾
	VECTOR GetDir(const VECTOR& vec) const;

	// ���f���̃n���h��ID
	int modelId;

	// �傫��
	VECTOR scl;
	// ��]
	VECTOR rot;
	// �ʒu
	VECTOR pos;
	VECTOR localPos;

	MATRIX matScl;
	MATRIX matRot;
	MATRIX matPos;

	// ��]
	Quaternion quaRot;

	// ���[�J����]
	Quaternion quaRotLocal;

	// �X�V�O�v�f�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[

	// �傫��
	VECTOR preScl;
	// ��]
	VECTOR preRot;
	// �ʒu
	VECTOR prePos;

	// ��]
	Quaternion preQuaRot;

	// �ǉ����[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[

	// ��\���ɂ��邩
	bool isHide;

	// �d�͂�ON�ɂ��邩
	bool isGravity;

	// �d�͌W��
	float heavy;

	// ����(�g��)
	float height;

	// �w��̃t���[�����\���ɂ���(�傫����0�ɂ���)
	void HideFrame(int frameNo);

	// �w��̃t���[�����ĕ\������(�傫����1�ɂ���)
	void AppearFrame(int frameNo);

	// �w��̃t���[������\�����ǂ���
	const bool IsHidingFrame(int frameNo);

	// �d�͏���
	void Gravity(void);

	// �d�͉����x�����Z�b�g
	void ResetGravityAccel(void);

	// �d�͉����x���Q�b�g
	const float& GetGravityAccel(void) const;

	// �W�����v������
	void Jump(float pow);

	// �㏸�����ۂ�
	bool IsRising(void);

	// ���~�����ۂ�
	bool IsFalling(void);

private:

	const float GRAVITY = (9.8f);

	// �d�͉����x
	float gravitySpeed_;
};

