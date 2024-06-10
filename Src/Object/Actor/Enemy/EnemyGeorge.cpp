#include "../../../Manager/CollisionManager.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/GameManager.h"
#include "../../../Manager/Camera/GameCamera.h"
#include "../../Common/ColliderCapsule.h"
#include "../../Common/ColliderSphere.h"
#include "../../Common/ColliderLine.h"
#include "../../Bullet/BulletBase.h"
#include "../../Grenade/GrenadeBase.h"
#include "../../Gun/GunEnemy.h"
#include "Armour.h"
#include "EnemyGeorge.h"

EnemyGeorge::EnemyGeorge(std::vector<VECTOR*> followPos, bool isFirst):followPos_(followPos)
{
}

void EnemyGeorge::EnemyReset(void)
{
}

const VECTOR& EnemyGeorge::GetFollowPos(void) const
{
	return *followPos_[0];
}

VECTOR EnemyGeorge::GetPosForCamera(void) const
{
	return VAdd(transform_.pos, { 0.0f,150.0f,0.0f });
}

void EnemyGeorge::AnimNotice(int id)
{
	if (id == (int)ACTION::DEAD)
	{
		animCon_->SetIsStop((int)ACTION::DEAD, true);
	}
}

void EnemyGeorge::OnHit(std::weak_ptr<Collider> obj)
{
}

void EnemyGeorge::OnHit(std::weak_ptr<Collider> objA, std::weak_ptr<Collider> objB)
{
	float rate = 1.0f;

	switch (objA.lock()->objType)
	{
	case Collider::OBJECT_TYPE::PLAYER_HEAD:
		rate = 1.5f;
		break;
	case Collider::OBJECT_TYPE::PLAYER_BODY:
		rate = 1.0f;
		break;
	case Collider::OBJECT_TYPE::PLAYER_LIMBS:
		rate = 0.75f;
		break;
	case Collider::OBJECT_TYPE::PLAYER_CAP:
		rate = 1.0f;
		break;
	default:
		return;
		break;
	}

	float damage = 0.0f;

	if (objB.lock()->objType == Collider::OBJECT_TYPE::PLAYER_BULLET)
	{
		auto bullet = dynamic_pointer_cast<BulletBase>(objB.lock()->parent_.lock());

		damage = bullet->GetDamage() * rate;

		lastAtkID_ = bullet->GetID();

		// ���Q��(�Ō�ɍU�����Ă����I�u�W�F�N�g)
		lastAtkObj_ = bullet->GetParent();

		isNoticeDamage_ = true;
	}
	else if (objB.lock()->objType == Collider::OBJECT_TYPE::EXPLOSION) {

		auto grenade = dynamic_pointer_cast<GrenadeBase>(objB.lock()->parent_.lock());

		rate = 1.0f - (float)aliveShieldNum_ / (float)(SHIELD_NUM * ARMOUR_NUM);
		damage = grenade->GetDamage() * rate;

		lastAtkID_ = grenade->GetID();

		// ���Q��(�Ō�ɍU�����Ă����I�u�W�F�N�g)
		lastAtkObj_ = grenade->GetParent();

		isNoticeDamage_ = true;
	}
	else if (objB.lock()->objType == Collider::OBJECT_TYPE::SMOKE) {
		return;
	}

	damageCareTime_.Set();
	

	Damage(damage);

	auto id = objB.lock()->parent_.lock()->GetID();
	if ((int)NetManager::GetInstance().GetSelf().playerType == (int)id) {
		GameManager::DamageData data;
		data.damage_ = damage;
		data.pos = objA.lock()->pos;
		data.pos.x += (-10 + GetRand(20));
		data.pos.y += (-10 + GetRand(20));
		data.time_ = 1.0f;
		if (rate == 1.5f)data.color_ = { 255,255,0 };
		else data.color_ = { 255,0,0 };
		GameManager::GetInstance().AddDamageData(data);
	}
}

void EnemyGeorge::OnHit(MV1_COLL_RESULT_POLY info)
{
	// ������ɉ����߂��ꂽ�Ȃ�W�����v��d�͂����Z�b�g����
	if (info.Normal.y == 1.0f)
	{
		if (transform_.IsRising()) return;

		// �v���C���[���X�e�[�W��Ɉړ�
		//transform_.pos = info.HitPosition;
		transform_.pos.y = info.Position[0].y;

		// �d�͉����x�����Z�b�g
		transform_.ResetGravityAccel();
	}

	// �������ɉ����߂��ꂽ�Ȃ�W�����v�͂����Z�b�g����
	if (info.Normal.y == -1.0f)
	{
		if (transform_.IsFalling()) return;

		// �v���C���[���X�e�[�W��Ɉړ�
		transform_.pos = info.HitPosition;

		transform_.ResetGravityAccel();
	}

	// �΂ߏ�����ɉ����߂��ꂽ�Ȃ�W�����v��d�͂����Z�b�g����
	if (0.0f < info.Normal.y && info.Normal.y < 1.0f)
	{
		if (transform_.IsRising()) return;

		// �v���C���[���X�e�[�W��Ɉړ�
		transform_.pos = info.HitPosition;

		// �d�͉����x�����Z�b�g
		transform_.ResetGravityAccel();
	}

	// �������ɉ����߂��ꂽ�Ȃ�
	if (info.Normal.y == 0.0f)
	{
		// �v�Z�p�v���C���[���W(Y���l�����Ȃ�)
		VECTOR pPos = transform_.pos;
		pPos.y = 0.0f;

		// �v�Z�p����(Y���l�����Ȃ�)
		VECTOR normal = info.Normal;
		VECTOR planePos = info.Position[0];
		planePos.y = 0.0f;

		// �N�����v�p
		// X���W
		float Xmin = info.Position[0].x;
		float Xmax = info.Position[0].x;
		// Z���W
		float Zmin = info.Position[0].z;
		float Zmax = info.Position[0].z;

		for (auto p : info.Position)
		{
			// X���W
			if (Xmin > p.x)Xmin = p.x;
			if (Xmax < p.x)Xmax = p.x;

			// Z���W
			if (Zmin > p.z)Zmin = p.z;
			if (Zmax < p.z)Zmax = p.z;
		}

		// �v���C���[�ɍł��߂����ʏ�̍��W
		VECTOR nearPos = Plane_Point_MinLength_Position(planePos, normal, pPos);

		// �|���S���̒�����ŃN�����v
		nearPos.x = std::clamp(nearPos.x, Xmin, Xmax);
		nearPos.z = std::clamp(nearPos.z, Zmin, Zmax);


		// �����߂������x�N�g���v�Z
		VECTOR vec = VNorm(VSub(pPos, nearPos));


		// �v���C���[���X�e�[�W��Ɉړ�
		// ���a�擾
		auto col = std::dynamic_pointer_cast<ColliderCapsule>(colliders_[(int)BODY::CAP]);
		nearPos.y = transform_.pos.y;
		transform_.pos = VAdd(nearPos,
			VScale(vec, col->GetRadius() + 0.1f));
	}

	// ���W��]�ȂǍX�V
	//TransformUpdate();
}

void EnemyGeorge::OnHit(HITRESULT_LINE info)
{
	if (transform_.IsRising()) return;

	// �v���C���[���X�e�[�W��Ɉړ�
	transform_.pos = info.Position;

	// �d�͉����x�����Z�b�g
	transform_.ResetGravityAccel();


	// ���W��]�ȂǍX�V
	//TransformUpdate();
}

void EnemyGeorge::OnHit(VECTOR normal)
{
	// �@�������։����Ԃ�
	transform_.pos = VAdd(transform_.pos, VScale(normal, 0.5f));

	// ������ɉ����߂��ꂽ�Ȃ�W�����v��d�͂����Z�b�g����
	if (normal.y == 1)
	{
		if (transform_.IsRising()) return;

		transform_.ResetGravityAccel();
	}

	// �������ɉ����߂��ꂽ�Ȃ�W�����v�͂����Z�b�g����
	if (normal.y == -1)
	{
		transform_.ResetGravityAccel();
	}


	// ���W��]�ȂǍX�V
	//TransformUpdate();
}

void EnemyGeorge::Init_Enemy(void)
{
	ID_ = ID::ENEMY_GEORGE;

	// ���f���̓ǂݍ���
	model_ = std::make_unique<Model>(ResourceManager::GetInstance().GetResourceFile("Model_Enemy_Vanguard By T. Choonyung")->GetHandle());
	model_->SetIsAutoDeleteHandle(true);
	transform_.SetModel(model_->GetHandle());

	// �f�t�H���g�n���h��
	modelGrHandle_ = MV1GetTextureGraphHandle(transform_.modelId, 0);
	modelGrHandle2_ = MV1GetTextureGraphHandle(transform_.modelId, 3);
	// �����p�n���h��
	colorGrHandle_ = LoadGraph("Data/Model/Enemy/Vanguard By T. Choonyung.fbm/red.png");

	// ���f���̑傫��
	transform_.scl = { 1.5f,1.5f,1.5f };

	transform_.pos = {};

	// ���f���̊p�x����
	transform_.quaRotLocal = Quaternion::Euler({ 0.0f,Utility::Deg2RadF(180.0f),0.0f });
	transform_.quaRot = {};

	transform_.isGravity = true;
	transform_.heavy = 3.0f;
	transform_.height = 270.0f;

	transform_.Update();

	AnimationSetting();

	ColliderSetting();

	// �R���C�_�[���W�X�V
	UpdateCollider();

	// ���݂̃A�N�V����
	act_ = ACTION::IDLE;
	preAct_ = act_;
	actUp_ = ACTION::IDLE;
	preActUp_ = actUp_;

	// �ϋv��
	hp_ = hpMax_ = 1000.0f;

	gun_ = std::make_shared<GunEnemy>(transform_, 37, std::dynamic_pointer_cast<ActorBase>(GetPtr()));
	gun_->Init();

	// �e�̉�]
	gun_->ChangeState(GunBase::STATE::NONE);
	gunState_ = GunBase::STATE::NONE;
	
	int i = 0;
	for (auto& armour : armours_) {
		armour = std::make_shared<Armour>();
		armour->SetShield(VAdd(transform_.pos, ARMOUR_RELATIVE_POS[i]),i % 2);
		i++;
	}

	aliveShieldNum_ = SHIELD_NUM * ARMOUR_NUM;
	
	waitTime_.Init(TARGET_WAIT_TIME, 1.0f / 60.0f);
	searchHearingTime_.Init(HEARING_SEARCH_TIME, 1.0f / 60.0f);
	searchViewTime_.Init(VIEW_TIME, 1.0f / 60.0f);
	damageCareTime_.Init(DAMAGE_CARE_TIME, 1.0f / 60.0f);

	targetIndex_ = 0;
	targetPos_ = TARGET_POS[targetIndex_];

	// ���I�C���[
	waistRot_ = {};

	// ���m�t���O
	isNotice_ = false;
	isNoticeHearing_ = false;
	isNoticeDamage_ = false;

	ChangeState(STATE::IDLE);
}

void EnemyGeorge::Update_Enemy(void)
{

	// �X�V�O���X�V
	transform_.PreUpdate();
	for (auto col : colliders_)
	{
		col->PreUpdate();
	}

	preAct_ = act_;
	preActUp_ = actUp_;

	if (act_ == ACTION::DEAD)
	{
		DeadUpdate();
		return;
	}

	//if (!isEscape_) {
		if (isNoticeDamage_ || state_ == STATE::SHOT) {
			SearchExpansionUpdate();
		}
		else {
			SearchUpdate();
		}
	//}

	
	CollisionManager::GetInstance().CollisionEnemyEyeMap();

	if (isNotice_) {
		ChangeState(STATE::SHOT);
	}
	else if (isNoticeHearing_) {
		ChangeState(STATE::SEARCH);
	}
	else if (isNoticeDamage_ && state_ != STATE::SEARCH && state_ != STATE::SHOT) {
		ChangeState(STATE::SEARCH);
	}

	//if (isEscape_) {
	//	ChangeState(STATE::RUN);
	//}

	ChangeGunState(GunBase::STATE::NONE);

	stateUpdate_();

	// �A�j���[�V�����J��
	animCon_->ChangeAnimation((int)act_);
	animCon_->ChangeAnimation((int)actUp_);

	// �A�j���[�V�����X�V
	animCon_->Update();

	transform_.Update();

	// �R���C�_�[���W�X�V
	UpdateCollider();

	// �e�̉�]
	gun_->ChangeState(gunState_, 0.5f);

	// �e���W�X�V
	gun_->Update();

	int i = 0;
	aliveShieldNum_ = 0;
	for (auto& armour : armours_) {
		armour->UpdateShield(VAdd(transform_.pos, ARMOUR_RELATIVE_POS[i]));
		aliveShieldNum_ += armour->GetAliveShieldNum();
		i++;
	}
}

void EnemyGeorge::Draw_Enemy(void)
{
	SetUseLighting(false);
	MV1SetTextureGraphHandle(transform_.modelId, 0, colorGrHandle_, 0);
	MV1SetTextureGraphHandle(transform_.modelId, 3, colorGrHandle_, 0);

	transform_.scl = { 1.55f,1.55f, 1.55f };
	auto camera = std::dynamic_pointer_cast<GameCamera>(SceneManager::GetInstance().GetCamera());
	auto dir = VNorm(VSub(transform_.pos, camera->GetPos()));
	transform_.pos = VAdd(transform_.pos,VScale(dir,5.0f));
	transform_.pos.y -= 5.0f;
	transform_.Update();
	MV1DrawModel(transform_.modelId);

	MV1SetTextureGraphHandle(transform_.modelId, 0, modelGrHandle_, 0);
	MV1SetTextureGraphHandle(transform_.modelId, 3, modelGrHandle2_, 0);
	SetUseLighting(true);

	transform_.scl = { 1.5f,1.5f,1.5f };
	transform_.pos = VSub(transform_.pos, VScale(dir, 5.0f));
	transform_.pos.y += 5.0f;
	transform_.Update();
	transform_.Draw();

	// �e
	gun_->Draw();

	for (auto& armour : armours_) {
		armour->Draw();
	}

	//for (auto& col : colliders_) {
	//	col->DrawDebug();
	//}
	//for (int i = 0; i < (int)BODY::CAP; i++) {
	//	colliders_[i]->DrawDebug();
	//}
	//colliders_[(int)BODY::EYE]->DrawDebug();

	//if (!isNoticeDamage_) {
	//	// ���o
	//	DrawCone3D(
	//		VAdd(transform_.pos, { 0.0f,0.0f,0.0f }),
	//		VAdd(transform_.pos, { 0.0f,1.0f,0.0f }),
	//		HEARING_RANGE, 10, 0xfff7f, 0xfff7f, true);

	//	// ���o
	//	float viewRad = Utility::Deg2RadF(VIEW_ANGLE);
	//	VECTOR localPos = { 0.0f,5.0f,VIEW_RANGE };
	//	// ��]�s��
	//	MATRIX mat = MGetIdent();
	//	mat = MMult(mat, MGetRotY(transform_.rot.y));

	//	// �O��
	//	VECTOR forwardPos = VAdd(transform_.pos, VTransform(localPos, mat));

	//	// �E��
	//	MATRIX rightMat = MMult(mat, MGetRotY(viewRad));
	//	VECTOR rightPos = VAdd(transform_.pos, VTransform(localPos, rightMat));

	//	DrawTriangle3D({ transform_.pos.x,transform_.pos.y + 5,transform_.pos.z },
	//		forwardPos, rightPos, 0xffff00, true);

	//	// ����
	//	MATRIX leftMat = MMult(mat, MGetRotY(-viewRad));
	//	VECTOR leftPos = VAdd(transform_.pos, VTransform(localPos, leftMat));

	//	DrawTriangle3D({ transform_.pos.x,transform_.pos.y + 5,transform_.pos.z },
	//		leftPos, forwardPos, 0xffff00, true);
	//}
	//else {
	//	// ���o
	//	DrawCone3D(
	//		VAdd(transform_.pos, { 0.0f,0.0f,0.0f }),
	//		VAdd(transform_.pos, { 0.0f,1.0f,0.0f }),
	//		HEARING_RANGE_EXPANSION, 10, 0xfff7f, 0xfff7f, true);

	//	// ���o
	//	float viewRad = Utility::Deg2RadF(VIEW_ANGLE);
	//	VECTOR localPos = { 0.0f,5.0f,VIEW_RANGE_EXPANSION };
	//	// ��]�s��
	//	MATRIX mat = MGetIdent();
	//	mat = MMult(mat, MGetRotY(transform_.rot.y));

	//	// �O��
	//	VECTOR forwardPos = VAdd(transform_.pos, VTransform(localPos, mat));

	//	// �E��
	//	MATRIX rightMat = MMult(mat, MGetRotY(viewRad));
	//	VECTOR rightPos = VAdd(transform_.pos, VTransform(localPos, rightMat));

	//	DrawTriangle3D({ transform_.pos.x,transform_.pos.y + 5,transform_.pos.z },
	//		forwardPos, rightPos, 0xffff00, true);

	//	// ����
	//	MATRIX leftMat = MMult(mat, MGetRotY(-viewRad));
	//	VECTOR leftPos = VAdd(transform_.pos, VTransform(localPos, leftMat));

	//	DrawTriangle3D({ transform_.pos.x,transform_.pos.y + 5,transform_.pos.z },
	//		leftPos, forwardPos, 0xffff00, true);
	//}

}

void EnemyGeorge::Release_Enemy(void)
{
	transform_.Release();

	animCon_->Release();

	// �e
	gun_->Release();

	for (auto& armour : armours_) {
		armour->Release();
	}

	DeleteGraph(modelGrHandle_);
	DeleteGraph(modelGrHandle2_);
	DeleteGraph(colorGrHandle_);
}

void EnemyGeorge::TransformUpdate(void)
{
	// �ړ���]�傫���������f���ɔ��f
	transform_.Update();

	// �R���C�_�[���W�X�V
	UpdateCollider();

	// �e���W�X�V
	gun_->UpdateTransform();
}

void EnemyGeorge::AnimationSetting(void)
{
	// �A�j���[�^�[�R���g���[��
	std::vector<int> array = { 3 };
	animCon_ = std::make_unique<AnimatorController>(std::dynamic_pointer_cast<ActorBase>(GetPtr()), transform_, array);

	// �A�j���[�V�����ɂ����W�ړ��������p���W
	VECTOR defPos = { 0.0f, 0.0f, 0.0f };

	// �A�j���[�V����
	int anim = -1;
	// �ҋ@
	anim = MV1LoadModel("Data/Model/Enemy/Animation/Rifle Idle.mv1");
	animCon_->SetAnimData((int)ACTION::IDLE, anim, defPos);
	// ����
	anim = MV1LoadModel("Data/Model/Enemy/Animation/Walk With Rifle.mv1");
	animCon_->SetAnimData((int)ACTION::WALK, anim, defPos);
	// ����
	anim = MV1LoadModel("Data/Model/Enemy/Animation/Rifle Run.mv1");
	animCon_->SetAnimData((int)ACTION::RUN, anim, defPos);
	// ����
	anim = MV1LoadModel("Data/Model/Enemy/Animation/Firing Rifle.mv1");
	animCon_->SetAnimData((int)ACTION::SHOT, anim, defPos);
	// �����[�h
	anim = MV1LoadModel("Data/Model/Enemy/Animation/Reloading.mv1");
	animCon_->SetAnimData((int)ACTION::RELOAD, anim, defPos);
	// ��
	anim = MV1LoadModel("Data/Model/Enemy/Animation/Death From Front Headshot.mv1");
	animCon_->SetAnimData((int)ACTION::DEAD, anim, defPos);
	animCon_->SetNoticeTime((int)ACTION::DEAD, 170.0f);
}

void EnemyGeorge::ChangeAction(ACTION act)
{
	act_ = act;
}

void EnemyGeorge::ChangeActionUp(ACTION actUp)
{
	//if (isAct_) return;
	actUp_ = actUp;
}

void EnemyGeorge::ColliderSetting(void)
{
	// ��
	colliders_[(int)BODY::HEAD] = std::make_shared<ColliderSphere>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_HEAD,
		22.5f);
	// �ՓˊǗ��N���X�ɃZ�b�g
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::HEAD]);

	// ��
	colliders_[(int)BODY::CHEST] = std::make_shared<ColliderCapsule>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_BODY,
		30.0f, 30.0f);
	// �ՓˊǗ��N���X�ɃZ�b�g
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::CHEST]);

	// ��
	colliders_[(int)BODY::WAIST] = std::make_shared<ColliderCapsule>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_BODY,
		30.0f, 15.0f);
	// �ՓˊǗ��N���X�ɃZ�b�g
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::WAIST]);

	// ���r
	colliders_[(int)BODY::ARM_LEFT] = std::make_shared<ColliderCapsule>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_LIMBS,
		15.0f, 22.5f);
	// �ՓˊǗ��N���X�ɃZ�b�g
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::ARM_LEFT]);

	// �����
	colliders_[(int)BODY::WRIST_LEFT] = std::make_shared<ColliderCapsule>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_LIMBS,
		15.0f, 22.5f);
	// �ՓˊǗ��N���X�ɃZ�b�g
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::WRIST_LEFT]);

	// �E�r
	colliders_[(int)BODY::ARM_RIGHT] = std::make_shared<ColliderCapsule>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_LIMBS,
		15.0f, 22.5f);
	// �ՓˊǗ��N���X�ɃZ�b�g
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::ARM_RIGHT]);

	// �E���
	colliders_[(int)BODY::WRIST_RIGHT] = std::make_shared<ColliderCapsule>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_LIMBS,
		15.0f, 22.5f);
	// �ՓˊǗ��N���X�ɃZ�b�g
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::WRIST_RIGHT]);

	// ��������
	colliders_[(int)BODY::THIGH_LEFT] = std::make_shared<ColliderCapsule>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_LIMBS,
		18.0f, 35.0f);
	// �ՓˊǗ��N���X�ɃZ�b�g
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::THIGH_LEFT]);

	// ������
	colliders_[(int)BODY::SHIN_LEFT] = std::make_shared<ColliderCapsule>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_LIMBS,
		15.0f, 45.0f);
	// �ՓˊǗ��N���X�ɃZ�b�g
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::SHIN_LEFT]);

	// �E������
	colliders_[(int)BODY::THIGH_RIGHT] = std::make_shared<ColliderCapsule>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_LIMBS,
		18.0f, 35.0f);
	// �ՓˊǗ��N���X�ɃZ�b�g
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::THIGH_RIGHT]);

	// �E����
	colliders_[(int)BODY::SHIN_RIGHT] = std::make_shared<ColliderCapsule>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_LIMBS,
		15.0f, 45.0f);
	// �ՓˊǗ��N���X�ɃZ�b�g
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::SHIN_RIGHT]);

	// �}�b�v�Փ˗p�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[

	// �J�v�Z��
	colliders_[(int)BODY::CAP] = std::make_shared<ColliderCapsule>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_CAP,
		60.0f, 180.0f);
	// �ՓˊǗ��N���X�ɃZ�b�g
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::CAP]);

	// ����(����)
	colliders_[(int)BODY::TOPLINE] = std::make_shared<ColliderLine>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_TOPLINE);
	// �ՓˊǗ��N���X�ɃZ�b�g
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::TOPLINE]);

	// ����(����)
	colliders_[(int)BODY::DOWNLINE] = std::make_shared<ColliderLine>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_DOWNLINE);
	// �ՓˊǗ��N���X�ɃZ�b�g
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::DOWNLINE]);

	// �[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[

	// ����
	colliders_[(int)BODY::EYE] = std::make_shared<ColliderLine>(GetPtr(),
		Collider::OBJECT_TYPE::ENEMY_EYE);
	// �ՓˊǗ��N���X�ɃZ�b�g
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::EYE]);
}

void EnemyGeorge::UpdateCollider(void)
{
	VECTOR sendPos = {};
	Quaternion sendRot = {};

	// ���̃��[�J���s����擾����
	auto mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, 7);
	auto pos = MGetTranslateElem(mat);	// �s�񂩂�ړ��l�����o��
	auto rot = MGetRotElem(mat);		// �s�񂩂��]�����o��
	sendPos = pos;
	sendRot = Quaternion::GetRotation(rot);
	sendPos = VAdd(sendPos, sendRot.PosAxis({ 0.0f,15.0f,0.0f }));
	colliders_[(int)BODY::HEAD]->Update(sendPos, sendRot);

	// ��
	mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, 4);
	pos = MGetTranslateElem(mat);	// �s�񂩂�ړ��l�����o��
	rot = MGetRotElem(mat);		// �s�񂩂��]�����o��
	sendPos = pos;
	sendRot = Quaternion::GetRotation(rot);
	sendPos = VAdd(sendPos, sendRot.PosAxis({ 0.0f,10.0f,-7.5f }));
	colliders_[(int)BODY::CHEST]->Update(sendPos, sendRot);

	// ��
	mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, 3);
	pos = MGetTranslateElem(mat);	// �s�񂩂�ړ��l�����o��
	rot = MGetRotElem(mat);		// �s�񂩂��]�����o��
	sendPos = pos;
	sendRot = Quaternion::GetRotation(rot);
	sendPos = VAdd(sendPos, sendRot.PosAxis({ 0.0f,-7.5f,-4.5f }));
	colliders_[(int)BODY::WAIST]->Update(sendPos, sendRot);

	// ���r
	mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, 10);
	pos = MGetTranslateElem(mat);	// �s�񂩂�ړ��l�����o��
	rot = MGetRotElem(mat);		// �s�񂩂��]�����o��
	sendPos = pos;
	sendRot = Quaternion::GetRotation(rot);
	sendPos = VAdd(sendPos, sendRot.PosAxis({ 0.0f,15.0f,0.0f }));
	colliders_[(int)BODY::ARM_LEFT]->Update(sendPos, sendRot);

	// �����
	mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, 11);
	pos = MGetTranslateElem(mat);	// �s�񂩂�ړ��l�����o��
	rot = MGetRotElem(mat);		// �s�񂩂��]�����o��
	sendPos = pos;
	sendRot = Quaternion::GetRotation(rot);
	sendPos = VAdd(sendPos, sendRot.PosAxis({ 0.0f,15.0f,0.0f }));
	colliders_[(int)BODY::WRIST_LEFT]->Update(sendPos, sendRot);

	// �E�r
	mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, 34);
	pos = MGetTranslateElem(mat);	// �s�񂩂�ړ��l�����o��
	rot = MGetRotElem(mat);		// �s�񂩂��]�����o��
	sendPos = pos;
	sendRot = Quaternion::GetRotation(rot);
	sendPos = VAdd(sendPos, sendRot.PosAxis({ 0.0f,15.0f,0.0f }));
	colliders_[(int)BODY::ARM_RIGHT]->Update(sendPos, sendRot);

	// �E���
	mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, 35);
	pos = MGetTranslateElem(mat);	// �s�񂩂�ړ��l�����o��
	rot = MGetRotElem(mat);		// �s�񂩂��]�����o��
	sendPos = pos;
	sendRot = Quaternion::GetRotation(rot);
	sendPos = VAdd(sendPos, sendRot.PosAxis({ 0.0f,15.0f,0.0f }));
	colliders_[(int)BODY::WRIST_RIGHT]->Update(sendPos, sendRot);

	// ��������
	mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, 62);
	pos = MGetTranslateElem(mat);	// �s�񂩂�ړ��l�����o��
	rot = MGetRotElem(mat);		// �s�񂩂��]�����o��
	sendPos = pos;
	sendRot = Quaternion::GetRotation(rot);
	sendPos = VAdd(sendPos, sendRot.PosAxis({ 0.0f,40.0f,0.0f }));
	colliders_[(int)BODY::THIGH_LEFT]->Update(sendPos, sendRot);

	// ������
	mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, 63);
	pos = MGetTranslateElem(mat);	// �s�񂩂�ړ��l�����o��
	rot = MGetRotElem(mat);		// �s�񂩂��]�����o��
	sendPos = pos;
	sendRot = Quaternion::GetRotation(rot);
	sendPos = VAdd(sendPos, sendRot.PosAxis({ 0.0f,40.0f,0.0f }));
	colliders_[(int)BODY::SHIN_LEFT]->Update(sendPos, sendRot);

	// �E������
	mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, 57);
	pos = MGetTranslateElem(mat);	// �s�񂩂�ړ��l�����o��
	rot = MGetRotElem(mat);		// �s�񂩂��]�����o��
	sendPos = pos;
	sendRot = Quaternion::GetRotation(rot);
	sendPos = VAdd(sendPos, sendRot.PosAxis({ 0.0f,40.0f,0.0f }));
	colliders_[(int)BODY::THIGH_RIGHT]->Update(sendPos, sendRot);

	// �E����
	mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, 58);
	pos = MGetTranslateElem(mat);	// �s�񂩂�ړ��l�����o��
	rot = MGetRotElem(mat);		// �s�񂩂��]�����o��
	sendPos = pos;
	sendRot = Quaternion::GetRotation(rot);
	sendPos = VAdd(sendPos, sendRot.PosAxis({ 0.0f,40.0f,0.0f }));
	colliders_[(int)BODY::SHIN_RIGHT]->Update(sendPos, sendRot);

	// �}�b�v�Փ˗p�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[

	// �J�v�Z��
	sendPos = VAdd(transform_.pos, { 0.0f,135.0f,0.0f });
	sendRot = {};
	colliders_[(int)BODY::CAP]->Update(sendPos, sendRot);

	// ����(����)
	sendPos = transform_.pos;
	sendRot = {};
	auto col = std::dynamic_pointer_cast<ColliderLine>(colliders_[(int)BODY::TOPLINE]);
	col->Update(
		{ sendPos.x,sendPos.y + 270.0f,sendPos.z },
		{ sendPos.x,sendPos.y + 225.0f,sendPos.z },
		sendRot);

	// ����(����)
	sendPos = transform_.pos;
	sendRot = {};
	col = std::dynamic_pointer_cast<ColliderLine>(colliders_[(int)BODY::DOWNLINE]);
	col->Update(
		{ sendPos.x,sendPos.y + 20.0f,sendPos.z },
		{ sendPos.x,sendPos.y - 15.0f,sendPos.z },
		sendRot);

	// �[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[

	// �ڐ�
	// �G�l�~�[�ƃv���C���[�̐�
	mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, 7);
	pos = MGetTranslateElem(mat);	// �s�񂩂�ړ��l�����o��
	sendPos = pos;
	sendRot = {};
	auto cameraPos = SceneManager::GetInstance().GetCamera()->GetPos();
	col = std::dynamic_pointer_cast<ColliderLine>(colliders_[(int)BODY::EYE]);
	col->Update(sendPos, cameraPos, sendRot);
}

std::shared_ptr<GunBase> EnemyGeorge::GetGun(void)
{
	return gun_;
}

void EnemyGeorge::ProcessDead(void)
{
	auto& gMng = GameManager::GetInstance();
	gMng.AddScore(10000, (int)lastAtkID_);
	gMng.Kill(ID_, lastAtkID_);
	gMng.AddElimination(ID_);

	animCon_->Reset();
	animCon_->SetValidAnimMove(true);
	// �t���[����]���Z�b�g
	MV1ResetFrameUserLocalMatrix(transform_.modelId, 0);

	animCon_->SetIsStop((int)ACTION::DEAD, false);
	//isAct_ = false;
	ChangeAction(ACTION::DEAD);
	ChangeActionUp(ACTION::DEAD);
	// �A�j���[�V�����J��
	animCon_->ChangeAnimation((int)act_);
	animCon_->ChangeAnimation((int)actUp_);

	// �e�̉�]
	gun_->ChangeState(GunBase::STATE::RELOAD, 0.5f);
}

void EnemyGeorge::DeadUpdate(void)
{
	// �d��
	transform_.Gravity();

	// �A�j���[�V�����J��
	animCon_->ChangeAnimation((int)act_);
	animCon_->ChangeAnimation((int)actUp_);

	// �A�j���[�V�����X�V
	animCon_->Update();

	// �e���W�X�V
	gun_->Update();

	// ���W��]�ȂǍX�V
	TransformUpdate();
}

void EnemyGeorge::Update_EnemyAfterCollision(void)
{

}

void EnemyGeorge::OnHit()
{
	// ���m�t���O
	isNotice_ = false;
	isNoticeHearing_ = false;
}

void EnemyGeorge::ChangeState(STATE state)
{
	preState_ = state_;
	state_ = state;
	switch (state_)
	{
	case EnemyGeorge::STATE::IDLE:
		stateUpdate_ = std::bind(&EnemyGeorge::Idle, this);
		waitTime_.Set();

		// ���ɖړI�Ƃ���ꏊ�����߂�
		if (preState_ != STATE::SEARCH &&
			preState_ != STATE::SHOT) {
			targetIndex_++;
		}
		
		if (targetIndex_ >= TARGET_NUM) {
			targetIndex_ = 0;
		}
		targetPos_ = TARGET_POS[targetIndex_];
		targetGoalRot_ = Quaternion::ToEuler(
			Quaternion::LookRotation(VNorm(VSub(TARGET_POS[targetIndex_], transform_.pos)))
		);
		break;
	case EnemyGeorge::STATE::ROTATE:
		stateUpdate_ = std::bind(&EnemyGeorge::Rotation, this);
		isRotate_ = true;
		break;
	case EnemyGeorge::STATE::MOVE:
		stateUpdate_ = std::bind(&EnemyGeorge::Move, this);
		break;
	case EnemyGeorge::STATE::SHOT:
		stateUpdate_ = std::bind(&EnemyGeorge::Shot, this);
		searchViewTime_.Set();
		break;
	case EnemyGeorge::STATE::SEARCH:
		stateUpdate_ = std::bind(&EnemyGeorge::Search, this);
		searchHearingTime_.Set();
		break;
	}
}

void EnemyGeorge::ChangeGunState(GunBase::STATE state)
{
	gunState_ = state;
}

void EnemyGeorge::SearchUpdate(void)
{
	// ���m�t���O���Z�b�g
	isNotice_ = false;
	isNoticeHearing_ = false;

	// �v���C���[�̍��W���擾
	VECTOR pPos = *followPos_[0];

	// �G�l�~�[����v���C���[�܂ł̃x�N�g��
	VECTOR diff = VSub(pPos, transform_.pos);

	// ���o
	if ((diff.z * diff.z + diff.x * diff.x + diff.y * diff.y) <= HEARING_RANGE * HEARING_RANGE)
	{
		isNoticeHearing_ = true;
	}

	// �G�l�~�[�̌����Ă������
	float degE = Utility::DegIn360(
		Utility::Rad2DegF(transform_.rot.y));

	// �G�l�~�[�������Ă��������0�x�Ƃ����Ƃ��̃v���C���[�̊p�x
	float degP = Utility::DegIn360(
		Utility::Rad2DegF(
			atan2f(diff.x, diff.z)));

	degP = Utility::DegIn360(degP - degE);

	// ����͈�
	float degRangeL = 360.0f - VIEW_ANGLE;
	float degRangeR = 0.0f + VIEW_ANGLE;

	if (degRangeL <= degP || degP <= degRangeR)
	{
		if ((diff.z * diff.z + diff.x * diff.x + diff.y * diff.y)
			<= VIEW_RANGE * VIEW_RANGE)
		{
			isNotice_ = true;
		}
	}


}

void EnemyGeorge::SearchExpansionUpdate(void)
{
	// ���m�t���O���Z�b�g
	isNotice_ = false;
	isNoticeHearing_ = false;

	// �v���C���[�̍��W���擾
	VECTOR pPos = *followPos_[0];

	// �G�l�~�[����v���C���[�܂ł̃x�N�g��
	VECTOR diff = VSub(pPos, transform_.pos);

	// ���o
	if ((diff.z * diff.z + diff.x * diff.x + diff.y * diff.y) <= HEARING_RANGE_EXPANSION * HEARING_RANGE_EXPANSION)
	{
		isNoticeHearing_ = true;
	}

	// �G�l�~�[�̌����Ă������
	float degE = Utility::DegIn360(
		Utility::Rad2DegF(transform_.rot.y));

	// �G�l�~�[�������Ă��������0�x�Ƃ����Ƃ��̃v���C���[�̊p�x
	float degP = Utility::DegIn360(
		Utility::Rad2DegF(
			atan2f(diff.x, diff.z)));

	degP = Utility::DegIn360(degP - degE);

	// ����͈�
	float degRangeL = 360.0f - VIEW_ANGLE;
	float degRangeR = 0.0f + VIEW_ANGLE;

	if (degRangeL <= degP || degP <= degRangeR)
	{
		if ((diff.z * diff.z + diff.x * diff.x + diff.y * diff.y)
			<= VIEW_RANGE_EXPANSION * VIEW_RANGE_EXPANSION)
		{
			isNotice_ = true;
		}
	}
}

void EnemyGeorge::Idle(void)
{
	ChangeAction(ACTION::IDLE);
	ChangeActionUp(ACTION::IDLE);
	WaistRotate(0.0f);

	if (waitTime_.Update()) {
		ChangeState(STATE::ROTATE);
	}

	if (damageCareTime_.IsCount()) {
		if (waitTime_.Update()) {
			ChangeState(STATE::ROTATE);
		}
	}
}

void EnemyGeorge::Rotation(void)
{
	ChangeAction(ACTION::IDLE);
	ChangeActionUp(ACTION::IDLE);

	LazyRotationY(targetGoalRot_.y);
	WaistRotate(0.0f);
	if (!isRotate_) {
		ChangeState(STATE::MOVE);
	}
}

void EnemyGeorge::LazyRotationY(float goalRotY)
{
	// ��]����
	float degNowAngleY = Utility::Rad2DegF(transform_.rot.y);
	float degGoalAngleY = Utility::Rad2DegF(goalRotY);

	// 0�x�`360�x�ȓ��Ɋp�x�������߂�
	degGoalAngleY = static_cast<float>(Utility::DegIn360(degGoalAngleY));
	degNowAngleY = static_cast<float>(Utility::DegIn360(degNowAngleY));

	// ��]�����Ȃ����̉�]�������擾����(���v���:1�A�����v���:-1)
	int aroundDirY = Utility::DirNearAroundDeg(degNowAngleY, degGoalAngleY);

	// ���B�������p�x�ɉ�]��������
	if (degNowAngleY <= degGoalAngleY + 5.0f &&
		degNowAngleY >= degGoalAngleY - 5.0f)
	{
		transform_.rot.y = goalRotY;
		isRotate_ = false;
	}
	else
	{
		if (!damageCareTime_.IsCount()) {
			transform_.rot.y += ROTATE_POW * static_cast<float>(aroundDirY);
		}else {
			transform_.rot.y += ROTATE_POW * 3.0f * static_cast<float>(aroundDirY);
		}
		
	}



	// 0�x�`360�x�ȓ��Ɋp�x�������߂�
	transform_.rot.y = static_cast<float>(Utility::RadIn2PI(transform_.rot.y));

	// �N�H�[�^�j�I���ɕϊ�
	transform_.quaRot = Quaternion::Euler(transform_.rot);
}

void EnemyGeorge::WaistRotate(float goalRotX)
{
	// ���̃t���[��
	// �Ώۃt���[���̃��[�J���s��(�傫���A��]�A�ʒu)���擾����
	auto fMat = MV1GetFrameLocalMatrix(transform_.modelId, 3);
	auto fScl = MGetSize(fMat);			// �s�񂩂�傫�������o��
	auto fPos = MGetTranslateElem(fMat);	// �s�񂩂�ړ��l�����o��

	// ����]
	LazyRotationX(goalRotX);

	//if (waistRot_.x == 0.0f) return;

	//// ����X����]����
	//waistRot_.x = std::clamp(waistRot_.x, Utility::Deg2RadF(-70.0f), Utility::Deg2RadF(40.0f));

	// ��
	// �傫���A��]�A�ʒu�����[�J���s��ɖ߂�
	MATRIX mix = MGetIdent();
	mix = MMult(mix, MGetScale(fScl));		// �傫��
	MATRIX matRot = MGetIdent();			// ��]
	matRot = MMult(matRot, MGetRotX(waistRot_.x));
	matRot = MMult(matRot, MGetRotY(waistRot_.y));
	matRot = MMult(matRot, MGetRotZ(waistRot_.z));
	mix = MMult(mix, matRot);
	mix = MMult(mix, MGetTranslate(fPos));	// ���W
	// ���������s���Ώۃt���[���ɃZ�b�g������
	MV1SetFrameUserLocalMatrix(transform_.modelId, 3, mix);

	//TransformUpdate();
}

void EnemyGeorge::Move(void)
{

	if (damageCareTime_.Update()) {
		ChangeAction(ACTION::WALK);
		ChangeActionUp(ACTION::WALK);
		WaistRotate(0.0f);

		transform_.pos = VAdd(transform_.pos, VScale(transform_.quaRot.GetForward(), 2.0f));

		if (Utility::Distance(transform_.pos, targetPos_) <= 2.0f) {
			transform_.pos = targetPos_;
			ChangeState(STATE::IDLE);
		}
	}
	else {
		ChangeAction(ACTION::RUN);
		ChangeActionUp(ACTION::RUN);
		WaistRotate(0.0f);

		transform_.pos = VAdd(transform_.pos, VScale(transform_.quaRot.GetForward(), 7.0f));

		if (Utility::Distance(transform_.pos, targetPos_) <= 7.0f) {
			transform_.pos = targetPos_;
			ChangeState(STATE::IDLE);
		}
	}

}

void EnemyGeorge::Shot(void)
{
	if (GameManager::GetInstance().GetPlayerInfo(0).isHide){
		isNoticeDamage_ = false;
		ChangeAction(ACTION::IDLE);
		ChangeActionUp(ACTION::IDLE);
		ChangeState(STATE::IDLE);
		return;
	}


	auto follow = *followPos_[0];
	auto now = transform_.pos;
	auto target = Quaternion::LookRotation(VNorm(VSub(follow, now)));

	follow.y = 0.0f;
	now.y = 0.0f;
	transform_.quaRot = Quaternion::LookRotation(VNorm(VSub(follow, now)));
	transform_.quaRot.Mult(Quaternion::Euler({ 0.0f,Utility::Deg2RadF(30.0f),0.0f }));

	//transform_.Update();
	//follow = *followPos_[0];
	now = transform_.pos;
	now.y += 350.0f;
	follow = *followPos_[0];

	target = Quaternion::LookRotation(VNorm(VSub(follow, now)));


	WaistRotate(-target.ToEuler().x);


	if (isNotice_) {
		ChangeAction(ACTION::SHOT);
		ChangeActionUp(ACTION::SHOT);
		ChangeGunState(GunBase::STATE::SHOT);
		if (!gun_->IsRotate() && preActUp_ == ACTION::SHOT) {
			gun_->Shot();
		}
	}
	else {
		ChangeAction(ACTION::IDLE);
		ChangeActionUp(ACTION::IDLE);
		ChangeGunState(GunBase::STATE::NONE);
	}
	

	if (searchViewTime_.Update()) {
		isNoticeDamage_ = false;
		ChangeState(STATE::IDLE);
	}
}

void EnemyGeorge::Search(void)
{
	ChangeAction(ACTION::IDLE);
	ChangeActionUp(ACTION::IDLE);

	auto goal = Quaternion::ToEuler(
		Quaternion::LookRotation(VNorm(VSub(*followPos_[0], transform_.pos)))
	);
	LazyRotationY(goal.y);

	if (searchHearingTime_.Update()) {
		if (isNoticeDamage_) {
			isNoticeDamage_ = false;
			//isEscape_ = true;
			return;
		}

		isNoticeDamage_ = false;
		ChangeState(STATE::IDLE);
	}
}

void EnemyGeorge::LazyRotationX(float goalRotX)
{
	// ��]����
	float degNowAngleX = Utility::Rad2DegF(waistRot_.x);
	float degGoalAngleX = Utility::Rad2DegF(goalRotX);

	// 0�x�`360�x�ȓ��Ɋp�x�������߂�
	degGoalAngleX = static_cast<float>(Utility::DegIn360(degGoalAngleX));
	degNowAngleX = static_cast<float>(Utility::DegIn360(degNowAngleX));

	// ��]�����Ȃ����̉�]�������擾����(���v���:1�A�����v���:-1)
	int aroundDirX = Utility::DirNearAroundDeg(degNowAngleX, degGoalAngleX);

	// ���B�������p�x�ɉ�]��������
	if (degNowAngleX <= degGoalAngleX + 5.0f &&
		degNowAngleX >= degGoalAngleX - 5.0f)
	{
		waistRot_.x = goalRotX;
	}
	else
	{
		waistRot_.x += ROTATE_POW * static_cast<float>(aroundDirX);
	}


	// 0�x�`360�x�ȓ��Ɋp�x�������߂�
	waistRot_.x = static_cast<float>(Utility::RadIn2PI(waistRot_.x));
}
