#include "../../Utility/Utility.h"
#include "BulletAR.h"

BulletAR::BulletAR(int modelId, std::weak_ptr<ActorBase> parent) : BulletBase(modelId , parent)
{
	
}

void BulletAR::SetParam(void)
{

	// �X�s�[�h
	speed_ = 200.0f;

	// ��������
	aliveTime_ = 1.0f;

	// �_���[�W
	damage_ = 15.0f;

	// ���[�J����]
	// Z��������O��
	VECTOR localRot = { Utility::Deg2RadF(90.0f),Utility::Deg2RadF(0.0f),Utility::Deg2RadF(0.0f) };
	transform_.quaRotLocal = Quaternion::Euler(localRot);

	// �傫��
	float scale = 0.1f;
	transform_.scl = { scale,scale,scale };



}
