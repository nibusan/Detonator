#include "../../Utility/Utility.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/EffectManager.h"
#include "../../Manager/CollisionManager.h"
#include "../../Manager/AudioManager.h"
#include "../../Manager/Camera/GameCamera.h"
#include "../Common/ColliderSphere.h"
#include "../Common/ColliderLine.h"
#include "../../Object/Actor/Player/Player.h"
#include "GrenadeHand.h"

GrenadeHand::GrenadeHand(const Transform& follow, int frameNo, std::weak_ptr<ActorBase> parent) : GrenadeBase(follow,frameNo, parent)
{
}

void GrenadeHand::OnHit(MV1_COLL_RESULT_POLY info)
{
	// �e�X�g�R�[�h
	if (info.Normal.y == 1.0f)
	{
		// �v���C���[�ɍł��߂����ʏ�̍��W
		VECTOR nearPos = Plane_Point_MinLength_Position(info.Position[0],
			info.Normal, transform_.prePos);

		auto col = std::dynamic_pointer_cast<ColliderSphere>(collider_);
		auto r = col->GetRadius();
		nearPos.y += r;

		transform_.pos = nearPos;

		float pow = transform_.GetGravityAccel();
		transform_.Jump(pow * 1.5f);

		pow_ /= 2.0f;
		if (pow_ <= 0.5f) pow_ = 0.5f;
	}

	if (info.Normal.y == -1.0f)
	{
		// �v���C���[�ɍł��߂����ʏ�̍��W
		VECTOR nearPos = Plane_Point_MinLength_Position(info.Position[0],
			info.Normal, transform_.prePos);

		auto col = std::dynamic_pointer_cast<ColliderSphere>(collider_);
		auto r = col->GetRadius();
		nearPos.y -= r;

		transform_.pos = nearPos;

		transform_.ResetGravityAccel();

	}


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

	if (pow_ > 0.5f) {
		AudioManager::GetInstance().Play(
			AudioList::AUDIO_TYPE::GRENADE_HIT,
			transform_.pos,
			2500.0f,
			false,
			0.7f
		);
	}

}

void GrenadeHand::OnHit(VECTOR normal, VECTOR hitPos)
{
	if (normal.y == 0.0f)
	{
		// ����
		// ���˃x�N�g��
		auto to = transform_.pos;
		to.y = 0.0f;
		auto from = transform_.prePos;
		from.y = 0.0f;

		VECTOR inVec = VNorm(VSub(to, from));
		auto rVec = Utility::RefrectVector(inVec, normal);


		moveDir_ = VNorm(rVec);

		// pos����prePos�ւ̃x�N�g��(���K��)
		auto vecP = VNorm(VSub(transform_.prePos, transform_.pos));

		// nowToPreVec�����ɖʂƂ҂����荇���悤��
		// �����Ԃ�

		// nowToPreVec�̑傫�������߂�
		//float mag = Utility::Dot(normal, vecP) / (RADIUS_DEF);
		auto degS = Utility::AngleDeg({0.0f,1.0f,0.0f}, { 1.0f,-1.0f,0.0f });
		auto deg = Utility::AngleDeg(normal, vecP);

		auto rad = Utility::Deg2RadF(deg);
		auto c = cosf(rad);

		float mag = RADIUS_DEF / c;

		transform_.pos = VAdd(hitPos, VScale(vecP, mag));

		//transform_.pos = VAdd(hitPos, VScale(normal, RADIUS_DEF));
		//transform_.pos = hitPos;



		//transform_.quaRot = Quaternion::LookRotation(moveDir_);

		pow_ /= 1.5f;
		if (pow_ <= 0.5f) pow_ = 0.5f;
	}

	if (0.0f < normal.y && normal.y < 1.0f)
	{
		// ����
		// ���˃x�N�g��
		auto to = transform_.pos;
		to.y = 0.0f;
		auto from = transform_.prePos;
		from.y = 0.0f;

		auto n = normal;
		n.y = 0.0f;
		n = VNorm(n);
		VECTOR inVec = VNorm(VSub(to, from));
		auto rVec = Utility::RefrectVector(inVec, n);


		moveDir_ = VNorm(rVec);

		// pos����prePos�ւ̃x�N�g��(���K��)
		auto vecP = VNorm(VSub(transform_.prePos, transform_.pos));

		// nowToPreVec�����ɖʂƂ҂����荇���悤��
		// �����Ԃ�

		// nowToPreVec�̑傫�������߂�
		//float mag = Utility::Dot(normal, vecP) / (RADIUS_DEF);

		auto deg = Utility::AngleDeg(normal, vecP);

		auto rad = Utility::Deg2RadF(deg);
		auto c = cosf(rad);

		float mag = RADIUS_DEF / c;

		transform_.pos = VAdd(hitPos, VScale(vecP, mag));

		//transform_.pos = VAdd(hitPos, VScale(normal, RADIUS_DEF));
		//transform_.pos = hitPos;

		float pow = transform_.GetGravityAccel();
		if (pow < 0.0f) pow = 0.0f;
		
		transform_.Jump(pow * 2.0f);

		//transform_.quaRot = Quaternion::LookRotation(moveDir_);

		pow_ /= 3.0f;
		if (pow_ <= 1.0f) pow_ = 1.0f;
	}

	if (-1.0f < normal.y && normal.y < 0.0f)
	{
		// ����
		// ���˃x�N�g��
		auto to = transform_.pos;
		to.y = 0.0f;
		auto from = transform_.prePos;
		from.y = 0.0f;

		auto n = normal;
		n.y = 0.0f;
		n = VNorm(n);
		VECTOR inVec = VNorm(VSub(to, from));
		auto rVec = Utility::RefrectVector(inVec, n);


		moveDir_ = VNorm(rVec);

		// pos����prePos�ւ̃x�N�g��(���K��)
		auto vecP = VNorm(VSub(transform_.prePos, transform_.pos));

		// nowToPreVec�����ɖʂƂ҂����荇���悤��
		// �����Ԃ�

		// nowToPreVec�̑傫�������߂�
		//float mag = Utility::Dot(normal, vecP) / (RADIUS_DEF);

		auto deg = Utility::AngleDeg(normal, vecP);

		auto rad = Utility::Deg2RadF(deg);
		auto c = cosf(rad);

		float mag = RADIUS_DEF / c;

		transform_.pos = VAdd(hitPos, VScale(vecP, mag));

		//transform_.pos = VAdd(hitPos, VScale(normal, RADIUS_DEF));
		//transform_.pos = hitPos;

		transform_.ResetGravityAccel();

		//transform_.quaRot = Quaternion::LookRotation(moveDir_);

		pow_ /= 10.0f;
		if (pow_ <= 0.5f) pow_ = 0.5f;
	}
	//// �Ǐ]
	//ChangeState(STATE::END);

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
	//VECTOR posB = VAdd(transform_.pos, VScale(preToNow, sphere->GetRadius()));
	VECTOR posB = VAdd(transform_.pos, VScale(preToNow, sphere->GetRadius()));
	line->Update(posA, posB, transform_.rot);

	if (pow_ > 0.5f) {

		AudioManager::GetInstance().Play(
			AudioList::AUDIO_TYPE::GRENADE_HIT,
			transform_.pos,
			2500.0f,
			false,
			1.0f,
			100 / 2
		);
	}
}

void GrenadeHand::SetParam(void)
{
	// ���f���t�@�C����
	fileName_ = "Model_Grenade_HandGrenade1";

	// �傫��
	transform_.scl = { 0.9f,0.9f,0.9f };

	// �^�C�v
	type_ = GRENADE_TYPE::HAND;

	blast_.Init(BLAST_TIME, SceneManager::GetInstance().GetDeltaTime());
	blast_.Set();

	// ���΍��W
	relativePos_ = { 0.0f,0.0f,0.0f };

	transform_.localPos = { 0.0f,-10.0f,0.0f };

	damage_ = 500.0f;

	blastEffect_.Init(SceneManager::GetInstance().GetDeltaTime(), SceneManager::GetInstance().GetDeltaTime());
	blastEffect_.Set();
}

void GrenadeHand::Blast(void)
{
	EffectManager::EFFECT temp;
	temp.pos = transform_.pos;
	temp.rot = {};
	temp.scl = { 60.0f,60.0f,60.0f };
	temp.speed = 2.0f;
	temp.type = EffectManager::EFFECT_TYPE::EXPLOSION;
	effectID_ = EffectManager::GetInstance().AddEffect(temp);

	transform_.pos = VAdd(transform_.pos, VScale({0.0f,1.0f,0.0f}, 150.0f));

	AudioManager::GetInstance().Play(
		AudioList::AUDIO_TYPE::GRENADE_EXPLOSION,
		transform_.pos,
		5500.0f,
		false,
		0.8f
	);

	transform_.isHide = true;
	// �Ǐ]
	ChangeState(STATE::BLAST);

	// ���̃I�u�W�F�N�g�^�C�v�z�񂩂�v�f�������Ă���
	CollisionManager::GetInstance().DeleteCollider(collider_);

	// �I�u�W�F�N�g�^�C�v��ς��čĂуZ�b�g
	collider_->ChangeObjectType(Collider::OBJECT_TYPE::EXPLOSION);
	CollisionManager::GetInstance().SetCollider(collider_);

	// �J�����V�F�C�N
	auto camera = std::dynamic_pointer_cast<GameCamera>(SceneManager::GetInstance().GetCamera());
	auto player = std::dynamic_pointer_cast<Player>(parent_.lock());
	if (netManager_.GetSelf().key == player->GetKey()) {
		camera->Shake(1.0f, 5.0f, transform_.pos);
	}
	for (int i = 0; i < 4; i++) {
		camera->ShakeMulti(i, 1.0f, 5.0f, transform_.pos);
	}
	
}

void GrenadeHand::UpdateNone(void)
{
	// �ړ�
	VECTOR movePow = VScale(moveDir_, pow_);
	movePow.y = 0.0f;
	transform_.pos = VAdd(transform_.pos, movePow);

	// ��]
	// pow_��1�Ȃ�0.5�x��]������
	Quaternion rot = Quaternion::AngleAxis(Utility::Deg2RadF(pow_),
		Quaternion::LookRotation(moveDir_).GetRight());
	//auto rot = Quaternion::Euler({ Utility::Deg2RadF(pow_),0.0f,0.0f });
	transform_.quaRot = transform_.quaRot.Mult(rot);

	// �d��
	transform_.Gravity();

	if (blast_.Update()) {
		// ����
		Blast();
	}
}

void GrenadeHand::UpdateBlast(void)
{
	auto col = std::dynamic_pointer_cast<ColliderSphere>(collider_);
	auto r = col->GetRadius();
	r += 250.0f;
	if (r >= 250.0f) r = 250.0f;
	col->SetRadius(r);

	//auto e = EffectManager::GetInstance().GetData(effectID_);
	//if (!e.lock()->IsPlaying()) {
	//	ChangeState(STATE::END);
	//}
	if (blastEffect_.Update()) {
		ChangeState(STATE::END);
	}
}



void GrenadeHand::Reset(void)
{
	blast_.Set();
	blastEffect_.Set();
}
