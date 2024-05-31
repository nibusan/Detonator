#include "../ObjectBase.h"
#include "ColliderModel.h"

ColliderModel::ColliderModel(std::weak_ptr<ObjectBase> obj, Collider::OBJECT_TYPE type, int model)
	: Collider(obj)
{
	// �Փ˃^�C�v
	colType = COL_TYPE::MODEL;

	// �I�u�W�F�N�g�^�C�v
	objType = type;

	// ���f��ID
	modelId_ = model;

	// �����蔻��(�R���C�_)�쐬(�œK�����ꂽ����)
	int ret = MV1SetupCollInfo(modelId_, -1, 1, 1, 1);
}

void ColliderModel::Update(VECTOR followPos, Quaternion rot)
{
	// ��]
	quaRot = rot;

	// ���΍��W�������čX�V
	pos = VAdd(followPos, rot.PosAxis(relativePos));

	// ���f���R���W�����p��info��DxLib���ōX�V
	MV1RefreshCollInfo(modelId_, -1);
}

void ColliderModel::Update(VECTOR followPos, VECTOR rot)
{
	// ��]
	quaRot = rot;

	// ���΍��W�������čX�V
	Quaternion q = Quaternion::Euler(rot);
	pos = VAdd(followPos, q.PosAxis(relativePos));

	// ���f���R���W�����p��info��DxLib���ōX�V
	MV1RefreshCollInfo(modelId_, -1);
}

void ColliderModel::Update(void)
{
	// ���f���R���W�����p��info��DxLib���ōX�V
	MV1RefreshCollInfo(modelId_, -1);
}

int ColliderModel::GetModelId(void) const
{
	return modelId_;
}

void ColliderModel::HideFrame(int frameNo)
{
	// �Ώۃt���[���̃��[�J���s��(�傫���A��]�A�ʒu)���擾����
	auto mat = MV1GetFrameLocalMatrix(modelId_, frameNo);
	auto scl = MGetSize(mat);			// �s�񂩂�傫�������o��
	auto rot = MGetRotElem(mat);		// �s�񂩂��]�����o��
	auto pos = MGetTranslateElem(mat);	// �s�񂩂�ړ��l�����o��

	// �傫���A��]�A�ʒu�����[�J���s��ɖ߂�
	MATRIX mix = MGetIdent();
	mix = MMult(mix, MGetScale({ 0.0f,0.0f,0.0f }));	// �傫����0�ɂ���
	mix = MMult(mix, rot);							// ��]
	mix = MMult(mix, MGetTranslate({0.0f,-100.0f,0.0f}));			// ���W

	// ���������s���Ώۃt���[���ɃZ�b�g������
	MV1SetFrameUserLocalMatrix(modelId_, frameNo, mix);
}

void ColliderModel::AppearFrame(int frameNo)
{
	// �Ώۃt���[���̃��[�J���s��(�傫���A��]�A�ʒu)���擾����
	auto mat = MV1GetFrameLocalMatrix(modelId_, frameNo);
	auto scl = MGetSize(mat);			// �s�񂩂�傫�������o��
	auto rot = MGetRotElem(mat);		// �s�񂩂��]�����o��
	auto pos = MGetTranslateElem(mat);	// �s�񂩂�ړ��l�����o��

	// �傫���A��]�A�ʒu�����[�J���s��ɖ߂�
	MATRIX mix = MGetIdent();
	mix = MMult(mix, MGetScale({ 1.0f,1.0f,1.0f }));	// �傫����1�ɂ���
	mix = MMult(mix, rot);							// ��]
	mix = MMult(mix, MGetTranslate(pos));			// ���W

	// ���������s���Ώۃt���[���ɃZ�b�g������
	MV1SetFrameUserLocalMatrix(modelId_, frameNo, mix);
}

VECTOR ColliderModel::GetScaleFrame(int frameNo) const
{
	// �Ώۃt���[���̃��[�J���s��(�傫���A��]�A�ʒu)���擾����
	auto mat = MV1GetFrameLocalMatrix(modelId_, frameNo);
	auto scl = MGetSize(mat);			// �s�񂩂�傫�������o��
	return scl;
}
