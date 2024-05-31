#include "../../Object/Common/Transform.h"
#include "../ObjectBase.h"
#include "Collider.h"

Collider::Collider(std::weak_ptr<ObjectBase> obj) : parent_(obj)
{
	// �Փ˔�����s����
	isCollision = true;

	//// �Փ˂��Ă�����
	//isHit = false;

	// �Փ˃^�C�v
	colType = COL_TYPE::NONE;

	// ���΍��W
	relativePos = {};

	// ���W
	pos = {};

	// ��]
	quaRot = {};

	// �傫��
	scl = { 1.0f,1.0f,1.0f };

}

Collider::~Collider(void)
{
	
}



void Collider::Update(VECTOR followPos, Quaternion rot)
{

	// ��]
	quaRot = rot;

	// ���΍��W�������čX�V
	pos = VAdd(followPos,rot.PosAxis(relativePos));
}

void Collider::Update(VECTOR followPos, VECTOR rot)
{
	// ��]
	quaRot = rot;

	// ���΍��W�������čX�V
	Quaternion q = Quaternion::Euler(rot);
	pos = VAdd(followPos, q.PosAxis(relativePos));
}


void Collider::PreUpdate(void)
{
	// �傫��
	preScl = scl;
	// ��]
	preQuaRot = quaRot;
	// �ʒu
	prePos = pos;
}

void Collider::ChangeObjectType(OBJECT_TYPE type)
{
	


	objType = type;
}

void Collider::ClearHitObject(void)
{
	//hitObjects.clear();
}

void Collider::DrawDebug(void)
{

}
