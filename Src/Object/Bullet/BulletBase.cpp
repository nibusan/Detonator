#include "math.h"
#include "../../Utility/Utility.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/CollisionManager.h"
#include "../../Manager/EffectManager.h"
#include "../Common/ColliderLine.h"
#include "../Actor/ActorBase.h"
#include "../Actor/Player/Player.h"
#include "BulletBase.h"

BulletBase::BulletBase(int modelId,std::weak_ptr<ActorBase> parent)
{
	//transform_.SetModel(MV1DuplicateModel(modelId));

	parent_ = parent;

	ID_ = parent_.lock()->GetID();

	// ��������
	aliveTimeCnt_ = 0.0f;

	auto player = std::dynamic_pointer_cast<Player>(parent_.lock());
	if (player == nullptr) return;

	team_ = player->GetTeam();
	controlPosZ_ = player->GetControlPosZ();
}

BulletBase::~BulletBase(void)
{

}

void BulletBase::ReadyBullet(VECTOR pos, VECTOR dir)
{
	// ���W�ݒ�
	transform_.pos = pos;

	// �����ݒ�
	dir_ = dir;

	// ��������
	aliveTimeCnt_ = aliveTime_;

	// ��]�ݒ�
	// �P�ʃx�N�g������N�H�[�^�j�I����
	transform_.quaRot = Quaternion::LookRotation(dir);

	// ��\���ݒ�
	transform_.isHide = false;

	// �Փ˔�����I��
	collider_->isCollision = true;

	//EffectManager::EFFECT temp;
	//temp.pos = transform_.pos;
	//temp.rot = transform_.quaRot.ToEuler();
	//temp.scl = { 0.8f,0.8f,50.0f };
	//temp.speed = 3.0f;
	//temp.type = EffectManager::EFFECT_TYPE::BULLETBOOST;
	//effectHandle_ = EffectManager::GetInstance().AddEffect(temp);

}

void BulletBase::ResetBullet(void)
{
	// ��\���ݒ�
	transform_.isHide = true;

	// �Փ˔�����I�t
	collider_->isCollision = false;

	//const auto& data = EffectManager::GetInstance().GetData(effectHandle_).lock();
	//data->Stop();
}

const float& BulletBase::GetDamage(void) const
{
	return damage_;
}

void BulletBase::SetDamage(float damage)
{
	damage_ = damage;
}

void BulletBase::Move(void)
{
	// �ړ��ʍ쐬
	VECTOR movePow = VScale(dir_, speed_);
	transform_.pos = VAdd(transform_.pos, movePow);



	//const auto& data = EffectManager::GetInstance().GetData(effectHandle_).lock();
	//data->SetPos(transform_.pos);
	//auto angle = transform_.quaRot.Mult(Quaternion::Euler({0.0f,0.0f,0.0f})).ToEuler();
	//data->SetAngle(angle);
}

void BulletBase::AliveTime(void)
{
	// �������ԏ���
	if (aliveTimeCnt_ > 0.0f)
	{
		// �f���^�^�C���v�Z���Ĉ����Ă���
		aliveTimeCnt_ -= SceneManager::GetInstance().GetDeltaMult(1.0f);

		if (aliveTimeCnt_ <= 0.0f)
		{

			aliveTimeCnt_ = 0.0f;

			ResetBullet();
		}
	}
}

void BulletBase::Init_Object(void)
{
	// �e���Ǝ��̐ݒ�
	SetParam();

	// �ړ���]�傫���������f���ɔ��f
	transform_.Update();

	// �R���C�_�[
	collider_ = std::make_shared<ColliderLine>(GetPtr(), Collider::OBJECT_TYPE::PLAYER_BULLET);
	CollisionManager::GetInstance().SetCollider(collider_);
}

void BulletBase::Update_Object(void)
{

	// ��\�����͏������s��Ȃ�
	if (transform_.isHide)return;


	// �ړ��O���
	transform_.PreUpdate();

	// �ړ�����
	Move();

	// �������ԏ���
	AliveTime();

	// �ړ���]�傫���������f���ɔ��f
	transform_.Update();

	// �R���C�_�[�X�V
	auto col = std::dynamic_pointer_cast<ColliderLine>(collider_);
	col->Update(transform_.prePos, transform_.pos,transform_.quaRot);
}

void BulletBase::Draw_Object(void)
{
	// ���f���`��
	//transform_.Draw();

	// ���`��
	//DrawLine3D(transform_.prePos, transform_.pos, 0xd9a605);

	collider_->DrawDebug();
}

void BulletBase::Release_Object(void)
{
	// ���f���폜(��)
	transform_.Release();
}

void BulletBase::UpdateAfterCollision(void)
{
	if (team_ == 0) {
		if (transform_.pos.z > controlPosZ_) {
			EffectManager::EFFECT temp;
			temp.pos = transform_.pos;
			temp.pos.z = controlPosZ_ + 20.0f;
			temp.rot = { 0.0f,Utility::Deg2RadF(180.0f),0.0f };
			temp.scl = { 2.0f,2.0f,2.0f };
			temp.speed = 2.0f;
			temp.type = EffectManager::EFFECT_TYPE::BARRIER;
			EffectManager::GetInstance().AddEffect(temp);

			ResetBullet();
		}
	}
	else if (team_ == 1) {
		if (transform_.pos.z < controlPosZ_) {
			EffectManager::EFFECT temp;
			temp.pos = transform_.pos;
			temp.pos.z = controlPosZ_ - 20.0f;
			temp.rot = {};
			temp.scl = { 2.0f,2.0f,2.0f };
			temp.speed = 2.0f;
			temp.type = EffectManager::EFFECT_TYPE::BARRIER;
			EffectManager::GetInstance().AddEffect(temp);


			ResetBullet();
		}
	}
}

const std::weak_ptr<ActorBase>& BulletBase::GetParent(void) const
{
	return parent_;
}

void BulletBase::OnHit(std::weak_ptr<Collider> obj)
{
	if (obj.lock()->objType == Collider::OBJECT_TYPE::ENEMY)
	{
		EffectManager::EFFECT temp;
		temp.pos = transform_.pos;
		temp.rot = transform_.quaRot.ToEuler();
		temp.scl = { 15.0f,15.0f,15.0f };
		temp.speed = 4.0f;
		temp.type = EffectManager::EFFECT_TYPE::DAMAGE;
		EffectManager::GetInstance().AddEffect(temp);
	}
	else if (obj.lock()->objType == Collider::OBJECT_TYPE::PLAYER_HEAD ||
			 obj.lock()->objType == Collider::OBJECT_TYPE::PLAYER_BODY ||
			 obj.lock()->objType == Collider::OBJECT_TYPE::PLAYER_LIMBS ||
			 obj.lock()->objType == Collider::OBJECT_TYPE::PLAYER_CAP)
	{
		EffectManager::EFFECT temp;
		temp.pos = transform_.pos;
		temp.rot = transform_.quaRot.ToEuler();
		temp.scl = { 15.0f,15.0f,15.0f };
		temp.speed = 4.0f;
		temp.type = EffectManager::EFFECT_TYPE::DAMAGE;
		EffectManager::GetInstance().AddEffect(temp);
	}
	else if (obj.lock()->objType == Collider::OBJECT_TYPE::SHIELD)
	{
		EffectManager::EFFECT temp;
		temp.pos = transform_.pos;
		temp.rot = transform_.quaRot.ToEuler();
		temp.scl = { 2.0f,2.0f,2.0f };
		temp.speed = 2.0f;
		temp.type = EffectManager::EFFECT_TYPE::BULLETSMOKE;
		EffectManager::GetInstance().AddEffect(temp);
	}

	ResetBullet();
}

void BulletBase::OnHit(VECTOR hitPos)
{
	EffectManager::EFFECT temp;
	temp.pos = hitPos;
	temp.rot = transform_.quaRot.ToEuler();
	temp.scl = { 2.0f,2.0f,2.0f };
	temp.speed = 2.0f;
	temp.type = EffectManager::EFFECT_TYPE::BULLETSMOKE;
	EffectManager::GetInstance().AddEffect(temp);

	//transform_.isHide = true;
	//collider_->isCollision = false;

	ResetBullet();
}
