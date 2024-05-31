#pragma once
#include "Collider.h"

class ColliderModel : public Collider
{
public:

	// ���f��
	ColliderModel(std::weak_ptr<ObjectBase> obj, Collider::OBJECT_TYPE type, int model);
	~ColliderModel(void) = default;

	// �ʒu�X�V��
	void Update(VECTOR followPos, Quaternion rot) override;
	void Update(VECTOR followPos, VECTOR rot) override;
	void Update(void);

	// ���f��ID�擾
	int GetModelId(void) const;

	// �w��̃t���[�����Փ˔��肩�����(�傫����0�ɂ���)
	void HideFrame(int frameNo);

	// �w��̃t���[���̏Փ˔���𕜊�(�傫����1�ɂ���)
	void AppearFrame(int frameNo);

	VECTOR GetScaleFrame(int frameNo) const;

private:

	int modelId_;
};

