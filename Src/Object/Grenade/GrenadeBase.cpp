#include "../../Manager/CollisionManager.h"
#include "../../Manager/EffectManager.h"
#include "../../Manager/ResourceManager.h"
#include "../Common/ColliderSphere.h"
#include "../Common/ColliderLine.h"
#include "../../Utility/Utility.h"
#include "../Actor/ActorBase.h"
#include "GrenadeBase.h"

GrenadeBase::GrenadeBase(const Transform& follow, int frameNo, std::weak_ptr<ActorBase> parent) : 
	follow_(follow), netManager_(NetManager::GetInstance())
{
	parent_ = parent;
	followFrameNo_ = frameNo;
	effectID_ = -1;

	ID_ = parent_.lock()->GetID();


}

GrenadeBase::~GrenadeBase(void)
{

}

void GrenadeBase::Throw(VECTOR dir, float pow)
{
	// �ړ��ʌv�Z
	pow_ = pow;
	moveDir_ = dir;

	transform_.quaRot = Quaternion::LookRotation(moveDir_);

	// �W�����v
	transform_.Jump(dir.y * pow_ * 0.75f);

	// ������
	ChangeState(STATE::NONE);

	// �R���W�������I��
	collider_->isCollision = true;
	colliderLine_->isCollision = true;
}

bool GrenadeBase::IsEnd(void)
{
	return (state_ == STATE::END);
}

bool GrenadeBase::IsHold(void)
{
	return (state_ == STATE::FOLLOW);
}

void GrenadeBase::OnHit(MV1_COLL_RESULT_POLY info)
{

}


GRENADE_TYPE GrenadeBase::GetType(void)
{
	return type_;
}

void GrenadeBase::Init_Object(void)
{
	// �e���Ǝ��̐ݒ�
	SetParam();
	model_ = std::make_unique<Model>(ResourceManager::GetInstance().GetResourceFile(fileName_.c_str())->GetHandle());
	model_->SetIsAutoDeleteHandle(true);
	transform_.SetModel(model_->GetHandle());
	// ���f�����ǂݍ���
	//transform_.SetModel(
	//	MV1LoadModel(("Data/Model/Grenade/" + fileName_).c_str())
	//);

	// �ړ���
	moveDir_ = {};
	pow_ = 0.0f;

	// �d�͐ݒ�
	transform_.isGravity = true;
	transform_.heavy = 1.5f;

	// �Ǐ]
	ChangeState(STATE::FOLLOW);

	// �R���C�_�[
	collider_ = std::make_shared<ColliderSphere>(GetPtr(), Collider::OBJECT_TYPE::GRENADE_SPHERE, RADIUS_DEF);
	CollisionManager::GetInstance().SetCollider(collider_);
	collider_->isCollision = false;

	colliderLine_ = std::make_shared<ColliderLine>(GetPtr(), Collider::OBJECT_TYPE::GRENADE_LINE);
	CollisionManager::GetInstance().SetCollider(colliderLine_);
	colliderLine_->isCollision = false;
}

void GrenadeBase::Update_Object(void)
{
	if (state_ == STATE::END) return;

	transform_.PreUpdate();

	stateUpdate_();

	// ���f���ɔ��f
	transform_.Update();

	// �R���C�_�[�X�V
	auto sphere = std::dynamic_pointer_cast<ColliderSphere>(collider_);
	auto localPos = transform_.quaRot.PosAxis(relativePos_);
	sphere->Update(VAdd(transform_.pos, localPos), transform_.rot);

	auto line = std::dynamic_pointer_cast<ColliderLine>(colliderLine_);
	auto nowToPre = VNorm(VSub(transform_.prePos, transform_.pos));
	VECTOR posA = VAdd(transform_.prePos, VScale(nowToPre, sphere->GetRadius()));

	auto preToNow = VNorm(VSub(transform_.pos, transform_.prePos));
	VECTOR posB = VAdd(transform_.pos, VScale(preToNow, sphere->GetRadius()));
	line->Update(posA, posB, transform_.rot);
}

void GrenadeBase::Draw_Object(void)
{
	transform_.Draw();
	//collider_->DrawDebug();
	//colliderLine_->DrawDebug();
}

void GrenadeBase::Release_Object(void)
{
	transform_.Release();
}

void GrenadeBase::Hold(void)
{
	// �ړ���
	moveDir_ = {};
	pow_ = 0.0f;

	// �Ǐ]
	ChangeState(STATE::FOLLOW);

	// �d�̓��Z�b�g
	transform_.ResetGravityAccel();

	// �\��
	transform_.isHide = false;

	// �R���W�������I�t
	collider_->isCollision = false;
	colliderLine_->isCollision = false;

	// ���̃I�u�W�F�N�g�^�C�v�z�񂩂�v�f�������Ă���
	CollisionManager::GetInstance().DeleteCollider(collider_);
	
	// �I�u�W�F�N�g�^�C�v��ς��čĂуZ�b�g
	collider_->ChangeObjectType(Collider::OBJECT_TYPE::GRENADE_SPHERE);
	CollisionManager::GetInstance().SetCollider(collider_);

	// ���a���f�t�H���g��
	auto col = std::dynamic_pointer_cast<ColliderSphere>(collider_);
	col->SetRadius(RADIUS_DEF);

	Reset();
}

const float& GrenadeBase::GetDamage(void) const
{
	return damage_;
}

const std::weak_ptr<ActorBase>& GrenadeBase::GetParent(void) const
{
	return parent_;
}


void GrenadeBase::ChangeState(STATE state)
{
	state_ = state;

	switch (state)
	{
	case GrenadeBase::STATE::NONE:
		stateUpdate_ = std::bind(&GrenadeBase::UpdateNone, this);
		break;
	case GrenadeBase::STATE::FOLLOW:
		stateUpdate_ = std::bind(&GrenadeBase::UpdateFollow, this);
		break;
	case GrenadeBase::STATE::BLAST:
		stateUpdate_ = std::bind(&GrenadeBase::UpdateBlast, this);
		break;
	case GrenadeBase::STATE::END:
		stateUpdate_ = std::bind(&GrenadeBase::UpdateEnd, this);
		collider_->isCollision = false;
		colliderLine_->isCollision = false;
		break;
	}
}

void GrenadeBase::UpdateNone(void)
{
	// �ړ�
	VECTOR movePow = VScale(moveDir_, pow_);
	movePow.y = 0.0f;
	transform_.pos = VAdd(transform_.pos, movePow);

	// ��]
	// pow_��1�Ȃ�1�x��]������
	auto rot = Quaternion::Euler({ Utility::Deg2RadF(pow_),0.0f,0.0f });
	transform_.quaRot = transform_.quaRot.Mult(rot);

	// �d��
	transform_.Gravity();
}

void GrenadeBase::UpdateFollow(void)
{
	// �Ώۂ̍��W�擾
	auto mat = MV1GetFrameLocalWorldMatrix(follow_.modelId, followFrameNo_);
	auto pos = MGetTranslateElem(mat);
	auto rot = MGetRotElem(mat);

	// �ݒ�
	transform_.pos = pos;
	transform_.quaRot = Quaternion::GetRotation(rot);
}

void GrenadeBase::UpdateBlast(void)
{
	auto col = std::dynamic_pointer_cast<ColliderSphere>(collider_);
	auto r = col->GetRadius();
	r += 1.5f;
	if (r >= 300.0f) r = 300.0f;
	col->SetRadius(r);

	auto e = EffectManager::GetInstance().GetData(effectID_);
	if (!e.lock()->IsPlaying()) {
		ChangeState(STATE::END);
	}
}

void GrenadeBase::UpdateEnd(void)
{
}
