#include "../../Utility/Utility.h"
#include "BulletSr.h"

BulletSr::BulletSr(int modelId, std::weak_ptr<ActorBase> parent) : BulletBase(modelId , parent)
{
	transform_.SetModel(MV1DuplicateModel(modelId));
}

void BulletSr::SetParam(void)
{

	// �X�s�[�h
	speed_ = 500.0f;

	// ��������
	aliveTime_ = 10.0f;

	// �_���[�W
	damage_ = 100.0f;

	// ���[�J����]
	// Z��������O��
	VECTOR localRot = { Utility::Deg2RadF(90.0f),Utility::Deg2RadF(0.0f),Utility::Deg2RadF(0.0f) };
	transform_.quaRotLocal = Quaternion::Euler(localRot);

	// �傫��
	float scale = 0.1f;
	transform_.scl = { scale,scale,scale };

}
