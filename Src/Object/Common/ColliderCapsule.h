#pragma once
#include "Collider.h"

class ColliderCapsule : public Collider
{
public:

	// �J�v�Z��
	ColliderCapsule(std::weak_ptr<ObjectBase> obj,
		Collider::OBJECT_TYPE type, float radius, float length);
	~ColliderCapsule(void) = default;

	// �ʒu�X�V��
	void Update(VECTOR followPos, Quaternion rot) override;
	void Update(VECTOR followPos, VECTOR rot) override;

	// 1�t���[���O�X�V
	// �X�V�O���W�����g���ꍇ
	// ���̊֐����e�X�V������1�ԍŏ���
	// 1�x�����Ăяo��
	void PreUpdate(void) override;

	// �f�o�b�O�p�`��
	void DrawDebug(void) override;

	// ���a�擾
	float GetRadius(void) const;

	// �_A�擾
	VECTOR GetPosA(void) const;

	// �_B�擾
	VECTOR GetPosB(void) const;

	// ���a�擾
	float GetPreRadius(void) const;

	// �_A�擾
	VECTOR GetPrePosA(void) const;

	// �_B�擾
	VECTOR GetPrePosB(void) const;

private:

	// ���a
	float radius_;

	// ����
	float length_;

	// �J�v�Z���p���W
	VECTOR capPosA_;
	VECTOR capPosB_;

	// ���a
	float preRadius_;

	// ����
	float preLength_;

	// �J�v�Z���p���W
	VECTOR preCapPosA_;
	VECTOR preCapPosB_;

};

