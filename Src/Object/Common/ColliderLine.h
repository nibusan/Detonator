#pragma once
#include "Collider.h"

class ColliderLine : public Collider
{
public:

	// ���f��
	ColliderLine(std::weak_ptr<ObjectBase> obj, Collider::OBJECT_TYPE type);
	~ColliderLine(void) = default;

	// �ʒu�X�V��
	void Update(VECTOR followPos, Quaternion rot) override;
	void Update(VECTOR followPos, VECTOR rot) override;
	void Update(VECTOR followPosA, VECTOR followPosB, Quaternion rot);
	void Update(VECTOR followPosA, VECTOR followPosB, VECTOR rot);

	// �_A�擾
	VECTOR GetPosA(void) const;

	// �_B�擾
	VECTOR GetPosB(void) const;

	// �f�o�b�O�p�`��
	void DrawDebug(void) override;

private:

	VECTOR posA_;
	VECTOR posB_;
};

