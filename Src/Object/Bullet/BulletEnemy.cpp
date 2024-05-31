#include "../../Utility/Utility.h"
#include "../../Manager/CollisionManager.h"
#include "../Common/ColliderLine.h"
#include "BulletEnemy.h"

BulletEnemy::BulletEnemy(int modelId, std::weak_ptr<ActorBase> parent) : BulletBase(modelId , parent)
{
	transform_.SetModel(MV1DuplicateModel(modelId));
}

void BulletEnemy::Init_Object(void)
{
	// �e���Ǝ��̐ݒ�
	SetParam();

	// �ړ���]�傫���������f���ɔ��f
	transform_.Update();

	// �R���C�_�[
	collider_ = std::make_shared<ColliderLine>(GetPtr(), Collider::OBJECT_TYPE::ENEMY_BULLET);
	CollisionManager::GetInstance().SetCollider(collider_);
}

void BulletEnemy::SetParam(void)
{

	// �X�s�[�h
	speed_ = 100.0f;

	// ��������
	aliveTime_ = 1.0f;

	// �_���[�W
	damage_ = 8.0f;

	// ���[�J����]
	// Z��������O��
	VECTOR localRot = { Utility::Deg2RadF(90.0f),Utility::Deg2RadF(0.0f),Utility::Deg2RadF(0.0f) };
	transform_.quaRotLocal = Quaternion::Euler(localRot);

	// �傫��
	float scale = 0.1f;
	transform_.scl = { scale,scale,scale };


}
