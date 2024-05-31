#include "../Object/ObjectBase.h"
#include "../Object/Common/ColliderModel.h"
#include "../Object/Common/ColliderSphere.h"
#include "../Object/Common/ColliderCapsule.h"
#include "../Object/Common/ColliderLine.h"
#include "../Utility/Utility.h"
#include "CollisionManager.h"

CollisionManager* CollisionManager::instance_ = nullptr;

void CollisionManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new CollisionManager();
	}
	instance_->Init();
}

CollisionManager& CollisionManager::GetInstance(void)
{
	return *instance_;
}

void CollisionManager::Init(void)
{
}

void CollisionManager::Destroy(void)
{
	for (auto obj : colliders_)
	{
		obj.second.clear();
	}

	colliders_.clear();

	delete instance_;
}

void CollisionManager::ClearColliders(void)
{
	for (auto obj : colliders_)
	{
		obj.second.clear();
	}

	colliders_.clear();
}

void CollisionManager::SetCollider(std::weak_ptr<Collider> obj)
{

	// �I�u�W�F�N�g�^�C�v���擾
	auto type = obj.lock()->objType;

	// �}�b�v�z��̃L�[���g���Ă��̒��̗v�f���𐔂���
	if (colliders_.count(type) <= 0)
	{
		// �v�f0�Ȃ珉���ݒ�
		std::vector<std::weak_ptr<Collider>> temp;
		colliders_.emplace(type, temp);
	}

	// �}�b�v�z��̉ϒ��z��̒��ɃI�u�W�F�N�g���i�[
	colliders_.at(type).emplace_back(obj);
}

void CollisionManager::DeleteColliderNonParent(void)
{
	for (auto cols : colliders_)
	{
		std::erase_if(cols.second, 
			[](std::weak_ptr<Collider> col) {return col.lock()->parent_.expired(); });
	}
}

void CollisionManager::DeleteCollider(std::weak_ptr<Collider> col)
{
	// ����̃R���C�_�[���폜
	auto& cols = colliders_.at(col.lock()->objType);

	std::erase_if(cols, [&](const std::weak_ptr<Collider> c)
					{return c.lock() == col.lock(); });
}

void CollisionManager::CollisionSingle(void)
{
	// �e�����Ȃ��Ȃ����R���C�_�[�������I�ɍ폜
	DeleteColliderNonParent();

	// �v���C���[�ƃ}�b�v�̏Փ�
	CollisionPlayerMap();

	// �G�l�~�[�ƃ}�b�v
	CollisionEnemyMap();

	// �J�����ƃ}�b�v�̏Փ˔���
	//CollisionCameraMap();

	// �O���l�[�h�ƃ}�b�v�̏Փ�
	CollisionGrenadeMap();


	// �����ƃ}�b�v�̏Փ�
	CollisionExplosionMap();

	// �����ƃv���C���[
	CollisionExplosionPlayer();

	// �X���[�N�ƃv���C���[
	CollisionSmokePlayer();

	// �����ƃG�l�~�[
	CollisionExplosionEnemy();

	CollisionExplosionShield();

	// �v���C���[�̒e�ƃV�[���h
	CollisionPlayerBulletShield();



	// �v���C���[�̒e�ƃG�l�~�[
	CollisionPlayerBulletEnemy();

	//// �v���C���[�ƃv���C���[�̒e
	CollisionPlayerBullet();

	// �v���C���[�ƃG�l�~�[�̒e
	CollisionPlayerEnemyBullet();

	// �v���C���[�̒e�ƃ}�b�v
	CollisionPlayerBulletMap();

	// �G�l�~�[�̒e�ƃ}�b�v
	CollisionEnemyBulletMap();

}

void CollisionManager::CollisionMulti(void)
{
	// �e�����Ȃ��Ȃ����R���C�_�[�������I�ɍ폜
	DeleteColliderNonParent();

	// �v���C���[�ƃ}�b�v�̏Փ�
	CollisionPlayerMap();

	// �O���l�[�h�ƃ}�b�v�̏Փ�
	CollisionGrenadeMap();

	// �����ƃ}�b�v�̏Փ�
	CollisionExplosionMap();

	// �����ƃv���C���[
	CollisionExplosionPlayer();

	// �v���C���[�ƃv���C���[�̒e
	CollisionPlayerBullet();

	// �v���C���[�̒e�ƃ}�b�v
	CollisionPlayerBulletMap();
}



void CollisionManager::CollisionPlayerMap(void)
{
	if (colliders_.count(Collider::OBJECT_TYPE::PLAYER_CAP) <= 0) return;

	auto& objsPC = colliders_.at(Collider::OBJECT_TYPE::PLAYER_CAP);

	int no = 0;

	for (auto pc : objsPC)
	{
		// �ړ���
		VECTOR moveXZpow = VSub(pc.lock()->pos,pc.lock()->prePos);
		moveXZpow.y = 0.0f;
		VECTOR moveYpow = VSub(pc.lock()->pos, pc.lock()->prePos);
		moveYpow.x = moveYpow.z = 0.0f;

		// �v���C���[�ƃ}�b�v�̏Փ�(��)
		CollisionPlayerMapSide(moveXZpow, pc.lock(), no);

		// �v���C���[�ƃ}�b�v�̏Փ�(�V��)
		CollisionPlayerMapHead(pc.lock(), no);

		// �v���C���[�ƃ}�b�v�̏Փ�(�n��)
		CollisionPlayerMapFoot(pc.lock(), no);

		// �v���C���[�ƃ}�b�v�̏Փ�(�΂ߏ�)
		CollisionPlayerMapSlopeUp(pc.lock(), no);

		// �v���C���[�ƃ}�b�v�̏Փ�(�΂߉�)
		CollisionPlayerMapSlopeDown(pc.lock(), no);

		no++;
	}
}

void CollisionManager::CollisionPlayerMapSide(const VECTOR movePow, std::weak_ptr<Collider> player, int no)
{
	if (colliders_.count(Collider::OBJECT_TYPE::MAP) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP_UNBREAK) <= 0) return;

	//auto objsPC = objects_.at(Collider::OBJECT_TYPE::PLAYER_CAP);
	auto& objsM = colliders_.at(Collider::OBJECT_TYPE::MAP);
	auto& objsMU = colliders_.at(Collider::OBJECT_TYPE::MAP_UNBREAK);

	// �J�v�Z���ƃ��f��
	//for (auto pc : objsPC)
	//{
		auto tempPC = std::dynamic_pointer_cast<ColliderCapsule>(player.lock());

		// ���ړ�����W
		VECTOR movePosA = VAdd(tempPC->GetPrePosA(), movePow);
		movePosA.y -= tempPC->GetPreRadius();
		VECTOR movePosB = VAdd(tempPC->GetPrePosB(), movePow);
		movePosB.y += tempPC->GetPreRadius();

		// �}�b�v�I�u�W�F�N�g
		for (auto m : objsM)
		{
			auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

			// ���ړ�������������Ԃ̃J�v�Z���ƏՓ˔��肳����
			// �Փ˂��Ă��Ȃ��ꍇ�������s��Ȃ�
			auto info = MV1CollCheck_Capsule(tempM->GetModelId(), -1,
				movePosA, movePosB, tempPC->GetRadius());

			int hitNum = info.HitNum;

			if (hitNum <= 0)
			{
				// �����蔻�茋�ʃ|���S���z��̌�n��������
				MV1CollResultPolyDimTerminate(info);
				continue;
			}


			for (int i = 0; i < hitNum; i++)
			{
				// �����|���S��
				if (info.Dim[i].Normal.y != 0.0f) continue;

				int pHit = HitCheck_Capsule_Triangle(
					tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius(),
					info.Dim[i].Position[0], info.Dim[i].Position[1], info.Dim[i].Position[2]);

				if (pHit)
				{
					tempPC->parent_.lock()->OnHit(info.Dim[i]);
				}
			}

			// �����蔻�茋�ʃ|���S���z��̌�n��������
			MV1CollResultPolyDimTerminate(info);
		}


		// �}�b�v�ǒn��
		for (auto mu : objsMU)
		{
			auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

			auto info = MV1CollCheck_Capsule(tempMU->GetModelId(), -1,
				movePosA, movePosB, tempPC->GetRadius());


			int hitNum = info.HitNum;

			if (hitNum <= 0)
			{
				// �����蔻�茋�ʃ|���S���z��̌�n��������
				MV1CollResultPolyDimTerminate(info);
				continue;
			}


			for (int i = 0; i < hitNum; i++)
			{

				// �����|���S��
				if (info.Dim[i].Normal.y != 0.0f) continue;

				int pHit = HitCheck_Capsule_Triangle(
					tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius(),
					info.Dim[i].Position[0], info.Dim[i].Position[1], info.Dim[i].Position[2]);

				if (pHit)
				{
					tempPC->parent_.lock()->OnHit(info.Dim[i]);
				}

			}

			// �����蔻�茋�ʃ|���S���z��̌�n��������
			MV1CollResultPolyDimTerminate(info);
		}
	//}
}

void CollisionManager::CollisionPlayerMapSide2(const VECTOR movePow, std::weak_ptr<Collider> player, int no)
{
	if (colliders_.count(Collider::OBJECT_TYPE::MAP) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP_UNBREAK) <= 0) return;

	//auto objsPC = objects_.at(Collider::OBJECT_TYPE::PLAYER_CAP);
	auto& objsM = colliders_.at(Collider::OBJECT_TYPE::MAP);
	auto& objsMU = colliders_.at(Collider::OBJECT_TYPE::MAP_UNBREAK);

	// �J�v�Z���ƃ��f��
	//for (auto pc : objsPC)
	//{
	auto tempPC = std::dynamic_pointer_cast<ColliderCapsule>(player.lock());

	// ���ړ�����W
	//VECTOR movePosA = VAdd(tempPC->GetPrePosA(), movePow);
	//movePosA.y -= tempPC->GetPreRadius();
	//VECTOR movePosB = VAdd(tempPC->GetPrePosB(), movePow);
	//movePosB.y += tempPC->GetPreRadius();

	// �}�b�v�I�u�W�F�N�g
	for (auto m : objsM)
	{
		auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

		// ���ړ�������������Ԃ̃J�v�Z���ƏՓ˔��肳����
		// �Փ˂��Ă��Ȃ��ꍇ�������s��Ȃ�
		auto info = MV1CollCheck_Capsule(tempM->GetModelId(), -1,
			tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius());

		int hitNum = info.HitNum;

		if (hitNum <= 0)
		{
			// �����蔻�茋�ʃ|���S���z��̌�n��������
			MV1CollResultPolyDimTerminate(info);
			continue;
		}


		for (int i = 0; i < hitNum; i++)
		{
			// �����|���S��
			if (info.Dim[i].Normal.y != 0.0f) continue;

			int pHit = HitCheck_Capsule_Triangle(
				tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius(),
				info.Dim[i].Position[0], info.Dim[i].Position[1], info.Dim[i].Position[2]);

			if (pHit)
			{
				tempPC->parent_.lock()->OnHit(info.Dim[i]);
			}
		}

		// �����蔻�茋�ʃ|���S���z��̌�n��������
		MV1CollResultPolyDimTerminate(info);
	}


	// �}�b�v�ǒn��
	for (auto mu : objsMU)
	{
		auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

		auto info = MV1CollCheck_Capsule(tempMU->GetModelId(), -1,
			tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius());


		int hitNum = info.HitNum;

		if (hitNum <= 0)
		{
			// �����蔻�茋�ʃ|���S���z��̌�n��������
			MV1CollResultPolyDimTerminate(info);
			continue;
		}


		for (int i = 0; i < hitNum; i++)
		{

			// �����|���S��
			if (info.Dim[i].Normal.y != 0.0f) continue;

			int pHit = HitCheck_Capsule_Triangle(
				tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius(),
				info.Dim[i].Position[0], info.Dim[i].Position[1], info.Dim[i].Position[2]);

			if (pHit)
			{
				tempPC->parent_.lock()->OnHit(info.Dim[i]);
			}

		}

		// �����蔻�茋�ʃ|���S���z��̌�n��������
		MV1CollResultPolyDimTerminate(info);
	}
}

void CollisionManager::CollisionPlayerMapHead(std::weak_ptr<Collider> player, int no)
{
	if (colliders_.count(Collider::OBJECT_TYPE::PLAYER_TOPLINE) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP_UNBREAK) <= 0) return;

	auto& objsPL = colliders_.at(Collider::OBJECT_TYPE::PLAYER_TOPLINE);
	auto& objsM = colliders_.at(Collider::OBJECT_TYPE::MAP);
	auto& objsMU = colliders_.at(Collider::OBJECT_TYPE::MAP_UNBREAK);

	// �@���x�N�g����
	// {0.0f,1.0f,0.0f}�̂���

	// �������@�J�v�Z��������
	// Y == 1�@���@0 < Y < 1
	// ���ʁ��΂�
	// �Փ˂��Ă��镨�̒���
	// �v���C���[�ɍł��߂����ʂ̃|���S����T������

	// �J�v�Z���ƃ��f��
	//for (auto pc : objsPC)
	//{
		auto tempPC = std::dynamic_pointer_cast<ColliderCapsule>(player.lock());

		// �ł��Ⴂ���ʃ|���S��
		MV1_COLL_RESULT_POLY low = {};
		bool isHit = false;

		// �}�b�v�I�u�W�F�N�g
		for (auto m : objsM)
		{
			auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

			auto info = MV1CollCheck_Capsule(tempM->GetModelId(), -1,
				tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius());


			int hitNum = info.HitNum;

			if (hitNum <= 0)
			{
				// �����蔻�茋�ʃ|���S���z��̌�n��������
				MV1CollResultPolyDimTerminate(info);
				continue;
			}


			for (int i = 0; i < hitNum; i++)
			{

				if (info.Dim[i].Normal.y != -1) continue;

				if (!isHit)
				{
					low = info.Dim[i];
					isHit = true;
				}

				if (info.Dim[i].HitPosition.y < low.HitPosition.y)
				{
					low = info.Dim[i];
				}
			}

			// �����蔻�茋�ʃ|���S���z��̌�n��������
			MV1CollResultPolyDimTerminate(info);
		}

		// �}�b�v�ǒn��
		for (auto mu : objsMU)
		{
			auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

			auto info = MV1CollCheck_Capsule(tempMU->GetModelId(), -1,
				tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius());


			int hitNum = info.HitNum;

			if (hitNum <= 0)
			{
				// �����蔻�茋�ʃ|���S���z��̌�n��������
				MV1CollResultPolyDimTerminate(info);
				continue;
			}


			for (int i = 0; i < hitNum; i++)
			{

				if (info.Dim[i].Normal.y != -1) continue;

				if (!isHit)
				{
					low = info.Dim[i];
					isHit = true;
				}

				if (info.Dim[i].HitPosition.y < low.HitPosition.y)
				{
					low = info.Dim[i];
				}
			}

			// �����蔻�茋�ʃ|���S���z��̌�n��������
			MV1CollResultPolyDimTerminate(info);
		}

		// ���ʃ|���S��
		if (isHit)
		{
			// ��������
			for (int n = 0; n < MAP_COL_MAX; n++)
			{
				int pHit = HitCheck_Capsule_Triangle(
					tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius(),
					low.Position[0], low.Position[1], low.Position[2]);

				if (pHit)
				{
					VECTOR dir = { 0.0f,-1.0f,0.0f };
					// �v���C���[�������Ԃ�
					tempPC->parent_.lock()->OnHit(dir);

					// �Փ˂��Ă����������x����
					continue;
				}

				// �Փ˂��Ȃ��Ȃ�����񐔃��[�v�𔲂���	
				break;
			}
		}
	//}

	// �����ƃ��f��
	//for (auto pl : objsPL)
	//{
		auto tempPL = std::dynamic_pointer_cast<ColliderLine>(objsPL[no].lock());

		low = {};
		isHit = false;

		// �}�b�v�I�u�W�F�N�g
		for (auto m : objsM)
		{
			auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

			auto info = MV1CollCheck_LineDim(tempM->GetModelId(), -1,
				tempPL->GetPosA(), tempPL->GetPosB());
			if (info.HitNum <= 0)
			{
				// �����蔻�茋�ʃ|���S���z��̌�n��������
				MV1CollResultPolyDimTerminate(info);

				continue;
			}

			for (int i = 0; i < info.HitNum; i++)
			{
				if (info.Dim[i].Normal.y != -1) continue;

				if (!isHit)
				{
					low = info.Dim[i];
					isHit = true;
				}

				if (info.Dim[i].HitPosition.y < low.HitPosition.y)
				{
					low = info.Dim[i];
				}
			}

			// �����蔻�茋�ʃ|���S���z��̌�n��������
			MV1CollResultPolyDimTerminate(info);
		}


		// �}�b�v�I�u�W�F�N�g
		for (auto mu : objsMU)
		{
			auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

			auto info = MV1CollCheck_LineDim(tempMU->GetModelId(), -1,
				tempPL->GetPosA(), tempPL->GetPosB());
			if (info.HitNum <= 0)
			{
				// �����蔻�茋�ʃ|���S���z��̌�n��������
				MV1CollResultPolyDimTerminate(info);

				continue;
			}

			for (int i = 0; i < info.HitNum; i++)
			{
				if (info.Dim[i].Normal.y != 1) continue;

				if (!isHit)
				{
					low = info.Dim[i];
					isHit = true;
				}

				if (info.Dim[i].HitPosition.y < low.HitPosition.y)
				{
					low = info.Dim[i];
				}
			}

			// �����蔻�茋�ʃ|���S���z��̌�n��������
			MV1CollResultPolyDimTerminate(info);
		}

		if (isHit)
		{
			// �v���C���[���҂�����Ɉړ�

			// �v���C���[�̍������}�C�i�X����
			float height = tempPL->parent_.lock()->GetTransform().height;
			low.HitPosition.y -= height;

			tempPL->parent_.lock()->OnHit(low);
		}
	//}

}

void CollisionManager::CollisionPlayerMapFoot(std::weak_ptr<Collider> player, int no)
{
	if (colliders_.count(Collider::OBJECT_TYPE::PLAYER_DOWNLINE) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP_UNBREAK) <= 0) return;

	auto& objsPL = colliders_.at(Collider::OBJECT_TYPE::PLAYER_DOWNLINE);
	auto& objsM = colliders_.at(Collider::OBJECT_TYPE::MAP);
	auto& objsMU = colliders_.at(Collider::OBJECT_TYPE::MAP_UNBREAK);

	// �@���x�N�g����
	// {0.0f,1.0f,0.0f}�̂���


	// �������@�J�v�Z��������
	// Y == 1�@���@0 < Y < 1
	// ���ʁ��΂�
	// �Փ˂��Ă��镨�̒���
	// �v���C���[�ɍł��߂����ʂ̃|���S����T������

	// �J�v�Z���ƃ��f��
	auto tempPC = std::dynamic_pointer_cast<ColliderCapsule>(player.lock());
	if (!tempPC->isCollision) return;

	// �ł��������ʃ|���S��
	MV1_COLL_RESULT_POLY high = {};
	bool isHit = false;

	// �}�b�v�I�u�W�F�N�g
	for (auto m : objsM)
	{
		auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

		auto info = MV1CollCheck_Capsule(tempM->GetModelId(), -1,
			tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius());


		int hitNum = info.HitNum;

		if (hitNum <= 0)
		{
			// �����蔻�茋�ʃ|���S���z��̌�n��������
			MV1CollResultPolyDimTerminate(info);
			continue;
		}


		for (int i = 0; i < hitNum; i++)
		{

			if (info.Dim[i].Normal.y != 1) continue;

			if (!isHit)
			{
				high = info.Dim[i];
				isHit = true;
			}

			if (high.HitPosition.y < info.Dim[i].HitPosition.y)
			{
				high = info.Dim[i];
			}
		}

		// �����蔻�茋�ʃ|���S���z��̌�n��������
		MV1CollResultPolyDimTerminate(info);
	}

	// �}�b�v�ǒn��
	for (auto mu : objsMU)
	{
		auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

		auto info = MV1CollCheck_Capsule(tempMU->GetModelId(), -1,
			tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius());


		int hitNum = info.HitNum;

		if (hitNum <= 0)
		{
			// �����蔻�茋�ʃ|���S���z��̌�n��������
			MV1CollResultPolyDimTerminate(info);
			continue;
		}


		for (int i = 0; i < hitNum; i++)
		{

			if (info.Dim[i].Normal.y != 1) continue;

			if (!isHit)
			{
				high = info.Dim[i];
				isHit = true;
			}

			if (high.HitPosition.y < info.Dim[i].HitPosition.y)
			{
				high = info.Dim[i];
			}
		}

		// �����蔻�茋�ʃ|���S���z��̌�n��������
		MV1CollResultPolyDimTerminate(info);
	}

	// ���ʃ|���S��
	if (isHit)
	{
		// �������
		for (int n = 0; n < MAP_COL_MAX; n++)
		{
			int pHit = HitCheck_Capsule_Triangle(
				tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius(),
				high.Position[0], high.Position[1], high.Position[2]);

			if (pHit)
			{
				VECTOR dir = { 0.0f,1.0f,0.0f };
				// �v���C���[�������Ԃ�
				tempPC->parent_.lock()->OnHit(dir);

				// �Փ˂��Ă����������x����
				continue;
			}

			// �Փ˂��Ȃ��Ȃ�����񐔃��[�v�𔲂���	
			break;
		}
	}
	else
	{
		return;
	}

	// �����ƃ��f��
	auto tempPL = std::dynamic_pointer_cast<ColliderLine>(objsPL[no].lock());

	MV1_COLL_RESULT_POLY hit = {};
	isHit = false;

	// �}�b�v�I�u�W�F�N�g
	for (auto m : objsM)
	{
		auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

		auto info = MV1CollCheck_LineDim(tempM->GetModelId(), -1,
			tempPL->GetPosA(), tempPL->GetPosB());
		if (info.HitNum <= 0)
		{
			// �����蔻�茋�ʃ|���S���z��̌�n��������
			MV1CollResultPolyDimTerminate(info);

			continue;
		}

		for (int i = 0; i < info.HitNum; i++)
		{
			if (info.Dim[i].Normal.y != 1) continue;

			if (!isHit)
			{
				hit = info.Dim[i];
				isHit = true;
			}

			if (hit.HitPosition.y < info.Dim[i].HitPosition.y)
			{
				hit = info.Dim[i];
			}
		}

		// �����蔻�茋�ʃ|���S���z��̌�n��������
		MV1CollResultPolyDimTerminate(info);
	}


	// �}�b�v�I�u�W�F�N�g
	for (auto mu : objsMU)
	{
		auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

		auto info = MV1CollCheck_LineDim(tempMU->GetModelId(), -1,
			tempPL->GetPosA(), tempPL->GetPosB());
		if (info.HitNum <= 0)
		{
			// �����蔻�茋�ʃ|���S���z��̌�n��������
			MV1CollResultPolyDimTerminate(info);

			continue;
		}

		for (int i = 0; i < info.HitNum; i++)
		{
			if (info.Dim[i].Normal.y != 1) continue;

			if (!isHit)
			{
				hit = info.Dim[i];
				isHit = true;
			}

			if (hit.HitPosition.y < info.Dim[i].HitPosition.y)
			{
				hit = info.Dim[i];
			}
		}

		// �����蔻�茋�ʃ|���S���z��̌�n��������
		MV1CollResultPolyDimTerminate(info);
	}

	if (isHit)
	{
		// �v���C���[���}�b�v��Ɉړ�
		tempPL->parent_.lock()->OnHit(hit);
	}

}


void CollisionManager::CollisionPlayerMapSlopeUp(std::weak_ptr<Collider> player, int no)
{
	if (colliders_.count(Collider::OBJECT_TYPE::PLAYER_DOWNLINE) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP_UNBREAK) <= 0) return;

	//auto objsPC = objects_.at(Collider::OBJECT_TYPE::PLAYER_CAP);
	auto& objsPL = colliders_.at(Collider::OBJECT_TYPE::PLAYER_DOWNLINE);
	auto& objsM = colliders_.at(Collider::OBJECT_TYPE::MAP);
	auto& objsMU = colliders_.at(Collider::OBJECT_TYPE::MAP_UNBREAK);


	// �������@�J�v�Z��������
	// Y == 1�@���@0 < Y < 1
	// ���ʁ��΂�
	// �Փ˂��Ă��镨�̒���
	// �v���C���[�ɍł��߂����ʂ̃|���S����T������

	// �J�v�Z���ƃ��f��
	auto tempPC = std::dynamic_pointer_cast<ColliderCapsule>(player.lock());

	// �ł��߂��΂�(0 < y < 1)�|���S��
	MV1_COLL_RESULT_POLY nea = {};
	bool isHit = false;

	// �}�b�v�I�u�W�F�N�g
	for (auto m : objsM)
	{
		auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

		auto info = MV1CollCheck_Capsule(tempM->GetModelId(), -1,
			tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius());


		int hitNum = info.HitNum;

		if (hitNum <= 0)
		{
			// �����蔻�茋�ʃ|���S���z��̌�n��������
			MV1CollResultPolyDimTerminate(info);
			continue;
		}


		for (int i = 0; i < hitNum; i++)
		{
			// 0 < y < 1
			if (info.Dim[i].Normal.y <= 0.0f || 1.0f <= info.Dim[i].Normal.y) continue;

			if (!isHit)
			{
				nea = info.Dim[i];
				isHit = true;
			}

			// �v���C���[�ƌ��݂̍ŋߒl�Ƃ̋���
			float dis = Triangle_Point_MinLength(
				nea.Position[0], nea.Position[1], nea.Position[2], tempPC->pos);

			// �v���C���[�ƐV�����l�Ƃ̋���
			float disNew = Triangle_Point_MinLength(
				info.Dim[i].Position[0], info.Dim[i].Position[1], info.Dim[i].Position[2], tempPC->pos);

			if (disNew < dis)
			{
				nea = info.Dim[i];
			}
		}

		// �����蔻�茋�ʃ|���S���z��̌�n��������
		MV1CollResultPolyDimTerminate(info);
	}

	// �}�b�v�ǒn��
	for (auto mu : objsMU)
	{
		auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

		auto info = MV1CollCheck_Capsule(tempMU->GetModelId(), -1,
			tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius());


		int hitNum = info.HitNum;

		if (hitNum <= 0)
		{
			// �����蔻�茋�ʃ|���S���z��̌�n��������
			MV1CollResultPolyDimTerminate(info);
			continue;
		}


		for (int i = 0; i < hitNum; i++)
		{

			// 0 < y < 1
			if (info.Dim[i].Normal.y <= 0.0f || 1.0f <= info.Dim[i].Normal.y) continue;

			if (!isHit)
			{
				nea = info.Dim[i];
				isHit = true;
			}

			// �v���C���[�ƌ��݂̍ŋߒl�Ƃ̋���
			float dis = Triangle_Point_MinLength(
				nea.Position[0], nea.Position[1], nea.Position[2], tempPC->pos);
				
			// �v���C���[�ƐV�����l�Ƃ̋���
			float disNew = Triangle_Point_MinLength(
				info.Dim[i].Position[0], info.Dim[i].Position[1], info.Dim[i].Position[2], tempPC->pos);

			if (disNew < dis)
			{
				nea = info.Dim[i];
			}
		}

		// �����蔻�茋�ʃ|���S���z��̌�n��������
		MV1CollResultPolyDimTerminate(info);
	}

	// ���ʃ|���S��
	if (isHit)
	{
		// �������
		for (int n = 0; n < MAP_COL_MAX; n++)
		{
			int pHit = HitCheck_Capsule_Triangle(
				tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius(),
				nea.Position[0], nea.Position[1], nea.Position[2]);

			if (pHit)
			{
				VECTOR dir = { 0.0f,1.0f,0.0f };
				// �v���C���[�������Ԃ�
				tempPC->parent_.lock()->OnHit(dir);

				// �Փ˂��Ă����������x����
				continue;
			}

			// �Փ˂��Ȃ��Ȃ�����񐔃��[�v�𔲂���	
			break;
		}
	}
	else
	{
		return;
	}

	// �����ƃ��f��
	auto tempPL = std::dynamic_pointer_cast<ColliderLine>(objsPL[no].lock());

	nea = {};
	isHit = false;

	// �}�b�v�I�u�W�F�N�g
	for (auto m : objsM)
	{
		auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

		auto info = MV1CollCheck_LineDim(tempM->GetModelId(), -1,
			tempPL->GetPosA(), tempPL->GetPosB());
		if (info.HitNum <= 0)
		{
			// �����蔻�茋�ʃ|���S���z��̌�n��������
			MV1CollResultPolyDimTerminate(info);

			continue;
		}

		for (int i = 0; i < info.HitNum; i++)
		{
			// 0 < y < 1
			if (info.Dim[i].Normal.y <= 0.0f || 1.0f <= info.Dim[i].Normal.y) continue;

			if (!isHit)
			{
				nea = info.Dim[i];
				isHit = true;
			}

			// �v���C���[�ƌ��݂̍ŋߒl�Ƃ̋���
			float dis = Triangle_Point_MinLength(
				nea.Position[0], nea.Position[1], nea.Position[2], tempPL->pos);

			// �v���C���[�ƐV�����l�Ƃ̋���
			float disNew = Triangle_Point_MinLength(
				info.Dim[i].Position[0], info.Dim[i].Position[1], info.Dim[i].Position[2], tempPL->pos);

			if (disNew < dis)
			{
				nea = info.Dim[i];
			}
		}

		// �����蔻�茋�ʃ|���S���z��̌�n��������
		MV1CollResultPolyDimTerminate(info);
	}


	// �}�b�v�I�u�W�F�N�g
	for (auto mu : objsMU)
	{
		auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

		auto info = MV1CollCheck_LineDim(tempMU->GetModelId(), -1,
			tempPL->GetPosA(), tempPL->GetPosB());
		if (info.HitNum <= 0)
		{
			// �����蔻�茋�ʃ|���S���z��̌�n��������
			MV1CollResultPolyDimTerminate(info);

			continue;
		}

		for (int i = 0; i < info.HitNum; i++)
		{
			// 0 < y < 1
			if (info.Dim[i].Normal.y <= 0.0f || 1.0f <= info.Dim[i].Normal.y) continue;

			if (!isHit)
			{
				nea = info.Dim[i];
				isHit = true;
			}

			// �v���C���[�ƌ��݂̍ŋߒl�Ƃ̋���
			float dis = Triangle_Point_MinLength(
				nea.Position[0], nea.Position[1], nea.Position[2], tempPL->pos);

			// �v���C���[�ƐV�����l�Ƃ̋���
			float disNew = Triangle_Point_MinLength(
				info.Dim[i].Position[0], info.Dim[i].Position[1], info.Dim[i].Position[2], tempPL->pos);

			if (disNew < dis)
			{
				nea = info.Dim[i];
			}
		}

		// �����蔻�茋�ʃ|���S���z��̌�n��������
		MV1CollResultPolyDimTerminate(info);
	}

	if (isHit)
	{
		// �v���C���[���}�b�v��Ɉړ�
		tempPL->parent_.lock()->OnHit(nea);
	}


}

void CollisionManager::CollisionPlayerMapSlopeDown(std::weak_ptr<Collider> player, int no)
{
	if (colliders_.count(Collider::OBJECT_TYPE::MAP) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP_UNBREAK) <= 0) return;

	//auto objsPC = objects_.at(Collider::OBJECT_TYPE::PLAYER_CAP);
	auto& objsM = colliders_.at(Collider::OBJECT_TYPE::MAP);
	auto& objsMU = colliders_.at(Collider::OBJECT_TYPE::MAP_UNBREAK);


	// �������@�J�v�Z��������
	// Y == 1�@���@0 < Y < 1
	// ���ʁ��΂�
	// �Փ˂��Ă��镨�̒���
	// �v���C���[�ɍł��߂����ʂ̃|���S����T������

	// �J�v�Z���ƃ��f��
	//for (auto pc : objsPC)
	//{
		auto tempPC = std::dynamic_pointer_cast<ColliderCapsule>(player.lock());

		// �ł��߂��΂�(-1 < y < 0)�|���S��
		MV1_COLL_RESULT_POLY nea = {};
		bool isHit = false;

		// �}�b�v�I�u�W�F�N�g
		for (auto m : objsM)
		{
			auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

			auto info = MV1CollCheck_Capsule(tempM->GetModelId(), -1,
				tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius());

			int hitNum = info.HitNum;

			if (hitNum <= 0)
			{
				// �����蔻�茋�ʃ|���S���z��̌�n��������
				MV1CollResultPolyDimTerminate(info);
				continue;
			}


			for (int i = 0; i < hitNum; i++)
			{

				if (-1.0f >= info.Dim[i].Normal.y || 0.0f <= info.Dim[i].Normal.y) continue;

				if (!isHit)
				{
					nea = info.Dim[i];
					isHit = true;
				}

				// �v���C���[�ƌ��݂̍ŋߒl�Ƃ̋���
				float dis = Triangle_Point_MinLength(
					nea.Position[0], nea.Position[1], nea.Position[2], tempPC->pos);

				// �v���C���[�ƐV�����l�Ƃ̋���
				float disNew = Triangle_Point_MinLength(
					info.Dim[i].Position[0], info.Dim[i].Position[1], info.Dim[i].Position[2], tempPC->pos);

				if (disNew < dis)
				{
					nea = info.Dim[i];
				}
			}

			// �����蔻�茋�ʃ|���S���z��̌�n��������
			MV1CollResultPolyDimTerminate(info);
		}

		// �}�b�v�ǒn��
		for (auto mu : objsMU)
		{
			auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

			auto info = MV1CollCheck_Capsule(tempMU->GetModelId(), -1,
				tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius());


			int hitNum = info.HitNum;

			if (hitNum <= 0)
			{
				// �����蔻�茋�ʃ|���S���z��̌�n��������
				MV1CollResultPolyDimTerminate(info);
				continue;
			}


			for (int i = 0; i < hitNum; i++)
			{

				//-1 < y < 0
				if (-1.0f >= info.Dim[i].Normal.y || 0.0f <= info.Dim[i].Normal.y) continue;

				if (!isHit)
				{
					nea = info.Dim[i];
					isHit = true;
				}

				// �v���C���[�ƌ��݂̍ŋߒl�Ƃ̋���
				float dis = Triangle_Point_MinLength(
					nea.Position[0], nea.Position[1], nea.Position[2], tempPC->pos);

				// �v���C���[�ƐV�����l�Ƃ̋���
				float disNew = Triangle_Point_MinLength(
					info.Dim[i].Position[0], info.Dim[i].Position[1], info.Dim[i].Position[2], tempPC->pos);

				if (disNew < dis)
				{
					nea = info.Dim[i];
				}
			}

			// �����蔻�茋�ʃ|���S���z��̌�n��������
			MV1CollResultPolyDimTerminate(info);
		}

		// ���ʃ|���S��
		if (isHit)
		{
			// �ړ��O������
			for (int n = 0; n < MAP_COL_MAX; n++)
			{
				int pHit = HitCheck_Capsule_Triangle(
					tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius(),
					nea.Position[0], nea.Position[1], nea.Position[2]);

				if (pHit)
				{

					VECTOR dir = VNorm(
						VSub(tempPC->prePos, tempPC->pos));


					// �v���C���[�������Ԃ�
					tempPC->parent_.lock()->OnHit(dir);

					// �Փ˂��Ă����������x����
					continue;
				}

				// �Փ˂��Ȃ��Ȃ�����񐔃��[�v�𔲂���	
				break;
			}
		}
	//}
}

void CollisionManager::CollisionPlayerBullet(void)
{
	if (colliders_.count(Collider::OBJECT_TYPE::PLAYER_CAP) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::PLAYER_BULLET) <= 0) return;

	auto& objsPC = colliders_.at(Collider::OBJECT_TYPE::PLAYER_CAP);
	auto& objsPB = colliders_.at(Collider::OBJECT_TYPE::PLAYER_BULLET);

	for (auto pc : objsPC)
	{
		// �Փ˔�����s��Ȃ����̂̓X�L�b�v
		if (!pc.lock()->isCollision) continue;

		auto tempPC = std::dynamic_pointer_cast<ColliderCapsule>(pc.lock());

		for (auto pb : objsPB)
		{
			// �Փ˔�����s��Ȃ����̂̓X�L�b�v
			if (!pb.lock()->isCollision) continue;

			auto tempPB = std::dynamic_pointer_cast<ColliderLine>(pb.lock());

			auto isHit = HitCheck_Capsule_Capsule(tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius(),
				tempPB->GetPosA(), tempPB->GetPosB(), 0.1f);

			if (!isHit) continue;

			// ���̃v���C���[�ƏՓ˂��Ă����ꍇ
			// �ׂ����Փˏ������s��

			isHit = false;

			// ��
			if (colliders_.count(Collider::OBJECT_TYPE::PLAYER_HEAD) > 0)
			{
				auto objsH = colliders_.at(Collider::OBJECT_TYPE::PLAYER_HEAD);

				for (auto h : objsH)
				{
					auto tempH = std::dynamic_pointer_cast<ColliderSphere>(h.lock());

					isHit = HitCheck_Sphere_Capsule(tempH->pos, tempH->GetRadius(),
						tempPB->GetPosA(), tempPB->GetPosB(), 0.1f);

					if (!isHit) continue;

					// �Փˌ㏈��
					isHit = true;

					pc.lock()->parent_.lock()->OnHit(h, pb);
					pb.lock()->parent_.lock()->OnHit(pc);

					break;
				}
			}

			if (isHit) break;

			// ��
			if (colliders_.count(Collider::OBJECT_TYPE::PLAYER_BODY) > 0)
			{
				auto objsB = colliders_.at(Collider::OBJECT_TYPE::PLAYER_BODY);

				for (auto b : objsB)
				{
					auto tempB = std::dynamic_pointer_cast<ColliderCapsule>(b.lock());

					isHit = HitCheck_Capsule_Capsule(tempB->GetPosA(), tempB->GetPosB(), tempB->GetRadius(),
						tempPB->GetPosA(), tempPB->GetPosB(), 0.1f);

					if (!isHit) continue;

					// �Փˌ㏈��
					isHit = true;

					pc.lock()->parent_.lock()->OnHit(b, pb);
					pb.lock()->parent_.lock()->OnHit(pc);

					break;
				}
			}

			if (isHit) break;

			// �葫
			if (colliders_.count(Collider::OBJECT_TYPE::PLAYER_LIMBS) > 0)
			{
				auto objsL = colliders_.at(Collider::OBJECT_TYPE::PLAYER_LIMBS);

				for (auto l : objsL)
				{
					auto tempL = std::dynamic_pointer_cast<ColliderCapsule>(l.lock());

					isHit = HitCheck_Capsule_Capsule(tempL->GetPosA(), tempL->GetPosB(), tempL->GetRadius(),
						tempPB->GetPosA(), tempPB->GetPosB(), 0.1f);

					if (!isHit) continue;

					// �Փˌ㏈��
					isHit = true;

					pc.lock()->parent_.lock()->OnHit(l, pb);
					pb.lock()->parent_.lock()->OnHit(pc);

					break;
				}
			}

			if (isHit) break;
		}
	}
}

void CollisionManager::CollisionPlayerEnemyBullet(void)
{
	if (colliders_.count(Collider::OBJECT_TYPE::PLAYER_CAP) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::ENEMY_BULLET) <= 0) return;

	auto& objsPC = colliders_.at(Collider::OBJECT_TYPE::PLAYER_CAP);
	auto& objsEB = colliders_.at(Collider::OBJECT_TYPE::ENEMY_BULLET);

	for (auto pc : objsPC)
	{
		// �Փ˔�����s��Ȃ����̂̓X�L�b�v
		if (!pc.lock()->isCollision) continue;

		auto tempPC = std::dynamic_pointer_cast<ColliderCapsule>(pc.lock());

		for (auto eb : objsEB)
		{
			// �Փ˔�����s��Ȃ����̂̓X�L�b�v
			if (!eb.lock()->isCollision) continue;

			auto tempEB = std::dynamic_pointer_cast<ColliderLine>(eb.lock());

			auto isHit = HitCheck_Capsule_Capsule(tempPC->GetPosA(), tempPC->GetPosB(), tempPC->GetRadius(),
												  tempEB->GetPosA(), tempEB->GetPosB(), 0.1f);

			if (!isHit) continue;
			
			// ���̃v���C���[�ƏՓ˂��Ă����ꍇ
			// �ׂ����Փˏ������s��

			isHit = false;

			// ��
			if (colliders_.count(Collider::OBJECT_TYPE::PLAYER_HEAD) > 0)
			{
				auto objsH = colliders_.at(Collider::OBJECT_TYPE::PLAYER_HEAD);
				
				for (auto h : objsH)
				{
					auto tempH = std::dynamic_pointer_cast<ColliderSphere>(h.lock());

					isHit = HitCheck_Sphere_Capsule(tempH->pos, tempH->GetRadius(),
						tempEB->GetPosA(), tempEB->GetPosB(), 0.1f);

					if (!isHit) continue;

					// �Փˌ㏈��
					isHit = true;

					pc.lock()->parent_.lock()->OnHit(h, eb);
					eb.lock()->parent_.lock()->OnHit(pc);

					break;
				}
			}

			if (isHit) break;

			// ��
			if (colliders_.count(Collider::OBJECT_TYPE::PLAYER_BODY) > 0)
			{
				auto objsB = colliders_.at(Collider::OBJECT_TYPE::PLAYER_BODY);

				for (auto b : objsB)
				{
					auto tempB = std::dynamic_pointer_cast<ColliderCapsule>(b.lock());

					isHit = HitCheck_Capsule_Capsule(tempB->GetPosA(), tempB->GetPosB(), tempB->GetRadius(),
						tempEB->GetPosA(), tempEB->GetPosB(), 0.1f);

					if (!isHit) continue;

					// �Փˌ㏈��
					isHit = true;

					pc.lock()->parent_.lock()->OnHit(b, eb);
					eb.lock()->parent_.lock()->OnHit(pc);

					break;
				}
			}

			if (isHit) break;

			// �葫
			if (colliders_.count(Collider::OBJECT_TYPE::PLAYER_LIMBS) > 0)
			{
				auto objsL = colliders_.at(Collider::OBJECT_TYPE::PLAYER_LIMBS);

				for (auto l : objsL)
				{
					auto tempL = std::dynamic_pointer_cast<ColliderCapsule>(l.lock());

					isHit = HitCheck_Capsule_Capsule(tempL->GetPosA(), tempL->GetPosB(), tempL->GetRadius(),
						tempEB->GetPosA(), tempEB->GetPosB(), 0.1f);

					if (!isHit) continue;

					// �Փˌ㏈��
					isHit = true;

					pc.lock()->parent_.lock()->OnHit(l, eb);
					eb.lock()->parent_.lock()->OnHit(pc);

					break;
				}
			}

			if (isHit) break;
		}
	}
}

void CollisionManager::CollisionPlayerBulletEnemy(void)
{
	if (colliders_.count(Collider::OBJECT_TYPE::PLAYER_BULLET) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::ENEMY) <= 0) return;

	auto& objsPB = colliders_.at(Collider::OBJECT_TYPE::PLAYER_BULLET);
	auto& objsE = colliders_.at(Collider::OBJECT_TYPE::ENEMY);

	for (auto pb : objsPB)
	{
		// �Փ˔�����s��Ȃ����̂̓X�L�b�v
		if (!pb.lock()->isCollision) continue;

		auto tempPB = std::dynamic_pointer_cast<ColliderLine>(pb.lock());

		// �G�l�~�[
		for (auto e : objsE)
		{
			// �Փ˔�����s��Ȃ����̂̓X�L�b�v
			if (!e.lock()->isCollision) continue;

			auto tempE = std::dynamic_pointer_cast<ColliderSphere>(e.lock());

			auto isHit = HitCheck_Line_Sphere(tempPB->GetPosA(), tempPB->GetPosB(),
											  tempE->pos, tempE->GetRadius());

			if (isHit)
			{
				// �Փˌ㏈��
				pb.lock()->parent_.lock()->OnHit(e.lock());
				e.lock()->parent_.lock()->OnHit(pb.lock());
			}
		}
	}

}

void CollisionManager::CollisionPlayerBulletMap(void)
{
	if (colliders_.count(Collider::OBJECT_TYPE::PLAYER_BULLET) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP) <= 0 || 
		colliders_.count(Collider::OBJECT_TYPE::MAP_UNBREAK) <= 0) return;

	auto& objsPB = colliders_.at(Collider::OBJECT_TYPE::PLAYER_BULLET);
	auto& objsM = colliders_.at(Collider::OBJECT_TYPE::MAP);
	auto& objsMU = colliders_.at(Collider::OBJECT_TYPE::MAP_UNBREAK);

	//for (auto pb : objsPB)
	//{
	//	// �Փ˔�����s��Ȃ����̂̓X�L�b�v
	//	if (!pb.lock()->isCollision) continue;

	//	auto tempPB = std::dynamic_pointer_cast<ColliderLine>(pb.lock());

	//	// �N�_�ɍł��߂��Փ˓_
	//	VECTOR nea = {};
	//	bool isHit = false;

	//	// �}�b�v
	//	for (auto m : objsM)
	//	{


	//		auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

	//		auto info = MV1CollCheck_LineDim(tempM->GetModelId(), -1,
	//			tempPB->GetPosA(), tempPB->GetPosB());

	//		if (info.HitNum <= 0)
	//		{
	//			// �����蔻�茋�ʃ|���S���z��̌�n��������
	//			MV1CollResultPolyDimTerminate(info);
	//			continue;
	//		}

	//		for (int i = 0; i < info.HitNum; i++)
	//		{
	//			auto hit = info.Dim[i];

	//			if (!isHit)
	//			{
	//				nea = info.Dim[i].HitPosition;
	//				isHit = true;
	//				continue;
	//			}

	//			// ��r
	//			auto disA = Utility::Distance(tempPB->GetPosA(), nea);
	//			auto disB = Utility::Distance(tempPB->GetPosA(), info.Dim[i].HitPosition);

	//			if (disA > disB)
	//			{
	//				nea = info.Dim[i].HitPosition;
	//			}
	//		}

	//		// �����蔻�茋�ʃ|���S���z��̌�n��������
	//		MV1CollResultPolyDimTerminate(info);
	//	}

	//	// �}�b�v(�j��s�\)
	//	for (auto mu : objsMU)
	//	{

	//		auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

	//		auto info = MV1CollCheck_LineDim(tempMU->GetModelId(), -1,
	//			tempPB->GetPosA(), tempPB->GetPosB());


	//		if (info.HitNum <= 0)
	//		{
	//			// �����蔻�茋�ʃ|���S���z��̌�n��������
	//			MV1CollResultPolyDimTerminate(info);
	//			continue;
	//		}

	//		for (int i = 0; i < info.HitNum; i++)
	//		{
	//			auto hit = info.Dim[i];

	//			if (!isHit)
	//			{
	//				nea = info.Dim[i].HitPosition;
	//				isHit = true;
	//				continue;
	//			}

	//			// ��r
	//			auto disA = Utility::Distance(tempPB->GetPosA(), nea);
	//			auto disB = Utility::Distance(tempPB->GetPosA(), info.Dim[i].HitPosition);

	//			if (disA > disB)
	//			{
	//				nea = info.Dim[i].HitPosition;
	//			}
	//		}

	//		// �����蔻�茋�ʃ|���S���z��̌�n��������
	//		MV1CollResultPolyDimTerminate(info);

	//	}

	//	if (!isHit) continue;

	//	// �Փˌ㏈��
	//	pb.lock()->parent_.lock()->OnHit(nea);
	//}

	for (auto pb : objsPB)
	{
		// �Փ˔�����s��Ȃ����̂̓X�L�b�v
		if (!pb.lock()->isCollision) continue;

		auto tempPB = std::dynamic_pointer_cast<ColliderLine>(pb.lock());

		bool isHit = false;

		// �}�b�v
		for (auto m : objsM)
		{


			auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

			auto info = MV1CollCheck_Line(tempM->GetModelId(), -1,
				tempPB->GetPosA(), tempPB->GetPosB());

			if (!info.HitFlag)
			{
				//// �����蔻�茋�ʃ|���S���z��̌�n��������
				//MV1CollResultPolyDimTerminate(info);
				continue;
			}

			// �Փˌ㏈��
			pb.lock()->parent_.lock()->OnHit(info.HitPosition);
			isHit = true;
			break;
			//// �����蔻�茋�ʃ|���S���z��̌�n��������
			//MV1CollResultPolyDimTerminate(info);
		}

		if (isHit) break;

		// �}�b�v(�j��s�\)
		for (auto mu : objsMU)
		{

			auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

			auto info = MV1CollCheck_Line(tempMU->GetModelId(), -1,
				tempPB->GetPosA(), tempPB->GetPosB());


			if (!info.HitFlag)
			{
				//// �����蔻�茋�ʃ|���S���z��̌�n��������
				//MV1CollResultPolyDimTerminate(info);
				continue;
			}

			// �Փˌ㏈��
			pb.lock()->parent_.lock()->OnHit(info.HitPosition);
			isHit = true;
			break;
			//// �����蔻�茋�ʃ|���S���z��̌�n��������
			//MV1CollResultPolyDimTerminate(info);

		}

	}
}

void CollisionManager::CollisionPlayerBulletShield(void)
{
	if (colliders_.count(Collider::OBJECT_TYPE::SHIELD) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::PLAYER_BULLET) <= 0) return;

	auto& objsS = colliders_.at(Collider::OBJECT_TYPE::SHIELD);
	auto& objsPB = colliders_.at(Collider::OBJECT_TYPE::PLAYER_BULLET);

	for (auto s : objsS)
	{
		// �Փ˔�����s��Ȃ����̂̓X�L�b�v
		if (!s.lock()->isCollision) continue;

		auto tempS = std::dynamic_pointer_cast<ColliderCapsule>(s.lock());

		for (auto pb : objsPB)
		{
			// �Փ˔�����s��Ȃ����̂̓X�L�b�v
			if (!pb.lock()->isCollision) continue;

			auto tempPB = std::dynamic_pointer_cast<ColliderLine>(pb.lock());

			auto isHit = HitCheck_Capsule_Capsule(tempS->GetPosA(), tempS->GetPosB(), tempS->GetRadius(),
				tempPB->GetPosA(), tempPB->GetPosB(), 0.1f);


			if (!isHit) continue;

			// �Փˌ㏈��
			pb.lock()->parent_.lock()->OnHit(s.lock());
			s.lock()->parent_.lock()->OnHit(pb.lock());

		}
	}
}

void CollisionManager::CollisionEnemyMap(void)
{
	if (colliders_.count(Collider::OBJECT_TYPE::ENEMY) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP_UNBREAK) <= 0) return;


	auto& objsE = colliders_.at(Collider::OBJECT_TYPE::ENEMY);
	auto& objsM = colliders_.at(Collider::OBJECT_TYPE::MAP);
	auto& objsMU = colliders_.at(Collider::OBJECT_TYPE::MAP_UNBREAK);

	for (auto e : objsE)
	{
		if (!e.lock()->isCollision) continue;

		auto tempE = std::dynamic_pointer_cast<ColliderSphere>(e.lock());

		MV1_COLL_RESULT_POLY hit = {};
		bool isHit = false;

		// �}�b�v�I�u�W�F�N�g
		for (auto m : objsM)
		{
			auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

			auto info = MV1CollCheck_Sphere(tempM->GetModelId(), -1,
				tempE->pos, tempE->GetRadius());
			if (info.HitNum <= 0)
			{
				// �����蔻�茋�ʃ|���S���z��̌�n��������
				MV1CollResultPolyDimTerminate(info);

				continue;
			}

			// �Փˏ���
			tempE->parent_.lock()->OnHit(tempM);

			// �����蔻�茋�ʃ|���S���z��̌�n��������
			MV1CollResultPolyDimTerminate(info);
		}


		// �}�b�v�I�u�W�F�N�g
		for (auto mu : objsMU)
		{
			auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

			auto info = MV1CollCheck_Sphere(tempMU->GetModelId(), -1,
				tempE->pos, tempE->GetRadius());
			if (info.HitNum <= 0)
			{
				// �����蔻�茋�ʃ|���S���z��̌�n��������
				MV1CollResultPolyDimTerminate(info);

				continue;
			}

			// �Փˏ���
			tempE->parent_.lock()->OnHit(tempMU);

			// �����蔻�茋�ʃ|���S���z��̌�n��������
			MV1CollResultPolyDimTerminate(info);
		}

	}
}

void CollisionManager::CollisionEnemyBulletMap(void)
{
	if (colliders_.count(Collider::OBJECT_TYPE::ENEMY_BULLET) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP_UNBREAK) <= 0) return;

	auto& objsEB = colliders_.at(Collider::OBJECT_TYPE::ENEMY_BULLET);
	auto& objsM = colliders_.at(Collider::OBJECT_TYPE::MAP);
	auto& objsMU = colliders_.at(Collider::OBJECT_TYPE::MAP_UNBREAK);

	//for (auto eb : objsEB)
	//{
	//	// �Փ˔�����s��Ȃ����̂̓X�L�b�v
	//	if (!eb.lock()->isCollision) continue;

	//	auto tempEB = std::dynamic_pointer_cast<ColliderLine>(eb.lock());

	//	// �N�_�ɍł��߂��Փ˓_
	//	VECTOR nea = {};
	//	bool isHit = false;

	//	// �}�b�v
	//	for (auto m : objsM)
	//	{
	//		auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

	//		auto info = MV1CollCheck_LineDim(tempM->GetModelId(), -1,
	//			tempEB->GetPosA(), tempEB->GetPosB());

	//		if (info.HitNum <= 0)
	//		{
	//			// �����蔻�茋�ʃ|���S���z��̌�n��������
	//			MV1CollResultPolyDimTerminate(info);
	//			continue;
	//		}

	//		for (int i = 0; i < info.HitNum; i++)
	//		{
	//			auto hit = info.Dim[i];

	//			if (!isHit)
	//			{
	//				nea = info.Dim[i].HitPosition;
	//				isHit = true;
	//				continue;
	//			}

	//			// ��r
	//			auto disA = Utility::Distance(tempEB->GetPosA(), nea);
	//			auto disB = Utility::Distance(tempEB->GetPosA(), info.Dim[i].HitPosition);

	//			if (disA > disB)
	//			{
	//				nea = info.Dim[i].HitPosition;
	//			}
	//		}

	//		// �����蔻�茋�ʃ|���S���z��̌�n��������
	//		MV1CollResultPolyDimTerminate(info);
	//	}

	//	// �}�b�v(�j��s�\)
	//	for (auto mu : objsMU)
	//	{

	//		auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

	//		auto info = MV1CollCheck_LineDim(tempMU->GetModelId(), -1,
	//			tempEB->GetPosA(), tempEB->GetPosB());


	//		if (info.HitNum <= 0)
	//		{
	//			// �����蔻�茋�ʃ|���S���z��̌�n��������
	//			MV1CollResultPolyDimTerminate(info);
	//			continue;
	//		}

	//		for (int i = 0; i < info.HitNum; i++)
	//		{
	//			auto hit = info.Dim[i];

	//			if (!isHit)
	//			{
	//				nea = info.Dim[i].HitPosition;
	//				isHit = true;
	//				continue;
	//			}

	//			// ��r
	//			auto disA = Utility::Distance(tempEB->GetPosA(), nea);
	//			auto disB = Utility::Distance(tempEB->GetPosA(), info.Dim[i].HitPosition);

	//			if (disA > disB)
	//			{
	//				nea = info.Dim[i].HitPosition;
	//			}
	//		}

	//		// �����蔻�茋�ʃ|���S���z��̌�n��������
	//		MV1CollResultPolyDimTerminate(info);

	//	}

	//	if (!isHit) continue;

	//	// �Փˌ㏈��
	//	eb.lock()->parent_.lock()->OnHit(nea);
	//}

	for (auto eb : objsEB)
	{
		// �Փ˔�����s��Ȃ����̂̓X�L�b�v
		if (!eb.lock()->isCollision) continue;

		auto tempEB = std::dynamic_pointer_cast<ColliderLine>(eb.lock());

		bool isHit = false;

		// �}�b�v
		for (auto m : objsM)
		{


			auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

			auto info = MV1CollCheck_Line(tempM->GetModelId(), -1,
				tempEB->GetPosA(), tempEB->GetPosB());

			if (!info.HitFlag)
			{
				//// �����蔻�茋�ʃ|���S���z��̌�n��������
				//MV1CollResultPolyDimTerminate(info);
				continue;
			}

			// �Փˌ㏈��
			eb.lock()->parent_.lock()->OnHit(info.HitPosition);
			isHit = true;
			break;
			//// �����蔻�茋�ʃ|���S���z��̌�n��������
			//MV1CollResultPolyDimTerminate(info);
		}

		if (isHit) break;

		// �}�b�v(�j��s�\)
		for (auto mu : objsMU)
		{

			auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

			auto info = MV1CollCheck_Line(tempMU->GetModelId(), -1,
				tempEB->GetPosA(), tempEB->GetPosB());


			if (!info.HitFlag)
			{
				//// �����蔻�茋�ʃ|���S���z��̌�n��������
				//MV1CollResultPolyDimTerminate(info);
				continue;
			}

			// �Փˌ㏈��
			eb.lock()->parent_.lock()->OnHit(info.HitPosition);
			isHit = true;
			break;
			//// �����蔻�茋�ʃ|���S���z��̌�n��������
			//MV1CollResultPolyDimTerminate(info);

		}

	}
}

void CollisionManager::CollisionEnemyEyeMap(void)
{
	if (colliders_.count(Collider::OBJECT_TYPE::ENEMY_EYE) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP_UNBREAK) <= 0) return;

	auto& objsEE = colliders_.at(Collider::OBJECT_TYPE::ENEMY_EYE);
	auto& objsM = colliders_.at(Collider::OBJECT_TYPE::MAP);
	auto& objsMU = colliders_.at(Collider::OBJECT_TYPE::MAP_UNBREAK);

	for (auto ee : objsEE)
	{
		auto tempEE = std::dynamic_pointer_cast<ColliderLine>(ee.lock());

		MV1_COLL_RESULT_POLY hit = {};
		bool isHit = false;

		// �}�b�v�I�u�W�F�N�g
		for (auto m : objsM)
		{
			auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

			auto info = MV1CollCheck_LineDim(tempM->GetModelId(), -1,
				tempEE->GetPosA(), tempEE->GetPosB());
			if (info.HitNum <= 0)
			{
				// �����蔻�茋�ʃ|���S���z��̌�n��������
				MV1CollResultPolyDimTerminate(info);

				continue;
			}

			for (int i = 0; i < info.HitNum; i++)
			{
				auto hit = HitCheck_Line_Triangle(tempEE->GetPosA(), tempEE->GetPosB(),
					info.Dim[i].Position[0], info.Dim[i].Position[1], info.Dim[i].Position[2]);

				if (hit.HitFlag)
				{
					// 
					tempEE->parent_.lock()->OnHit();
				}
			}

			// �����蔻�茋�ʃ|���S���z��̌�n��������
			MV1CollResultPolyDimTerminate(info);
		}


		// �}�b�v�I�u�W�F�N�g
		for (auto mu : objsMU)
		{
			auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

			auto info = MV1CollCheck_LineDim(tempMU->GetModelId(), -1,
				tempEE->GetPosA(), tempEE->GetPosB());
			if (info.HitNum <= 0)
			{
				// �����蔻�茋�ʃ|���S���z��̌�n��������
				MV1CollResultPolyDimTerminate(info);

				continue;
			}

			for (int i = 0; i < info.HitNum; i++)
			{
				auto hit = HitCheck_Line_Triangle(tempEE->GetPosA(), tempEE->GetPosB(),
					info.Dim[i].Position[0], info.Dim[i].Position[1], info.Dim[i].Position[2]);

				if (hit.HitFlag)
				{
					// 
					tempEE->parent_.lock()->OnHit();
				}
			}

			// �����蔻�茋�ʃ|���S���z��̌�n��������
			MV1CollResultPolyDimTerminate(info);
		}

	}
}

bool CollisionManager::IsCollisionEnemyEyeMap(std::weak_ptr<Collider> obj)
{
	if (colliders_.count(Collider::OBJECT_TYPE::MAP) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP_UNBREAK) <= 0) return false;

	auto& objsM = colliders_.at(Collider::OBJECT_TYPE::MAP);
	auto& objsMU = colliders_.at(Collider::OBJECT_TYPE::MAP_UNBREAK);
	auto tempEE = std::dynamic_pointer_cast<ColliderLine>(obj.lock());

	// �}�b�v�I�u�W�F�N�g
	for (auto m : objsM)
	{
		auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

		auto info = MV1CollCheck_LineDim(tempM->GetModelId(), -1,
			tempEE->GetPosA(), tempEE->GetPosB());
		if (info.HitNum <= 0)
		{
			// �����蔻�茋�ʃ|���S���z��̌�n��������
			MV1CollResultPolyDimTerminate(info);

			continue;
		}

		for (int i = 0; i < info.HitNum; i++)
		{
			auto hit = HitCheck_Line_Triangle(tempEE->GetPosA(), tempEE->GetPosB(),
				info.Dim[i].Position[0], info.Dim[i].Position[1], info.Dim[i].Position[2]);

			if (hit.HitFlag)
			{
				// �����蔻�茋�ʃ|���S���z��̌�n��������
				MV1CollResultPolyDimTerminate(info);

				return true;
			}
		}

		// �����蔻�茋�ʃ|���S���z��̌�n��������
		MV1CollResultPolyDimTerminate(info);
	}


	// �}�b�v�I�u�W�F�N�g
	for (auto mu : objsMU)
	{
		auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

		auto info = MV1CollCheck_LineDim(tempMU->GetModelId(), -1,
			tempEE->GetPosA(), tempEE->GetPosB());
		if (info.HitNum <= 0)
		{
			// �����蔻�茋�ʃ|���S���z��̌�n��������
			MV1CollResultPolyDimTerminate(info);

			continue;
		}

		for (int i = 0; i < info.HitNum; i++)
		{
			auto hit = HitCheck_Line_Triangle(tempEE->GetPosA(), tempEE->GetPosB(),
				info.Dim[i].Position[0], info.Dim[i].Position[1], info.Dim[i].Position[2]);

			if (hit.HitFlag)
			{
				// �����蔻�茋�ʃ|���S���z��̌�n��������
				MV1CollResultPolyDimTerminate(info);

				return true;
			}
		}

		// �����蔻�茋�ʃ|���S���z��̌�n��������
		MV1CollResultPolyDimTerminate(info);
	}

	return false;
}

void CollisionManager::CollisionCameraMap(void)
{
	if (colliders_.count(Collider::OBJECT_TYPE::CAMERA) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP_UNBREAK) <= 0) return;

	//auto objsPC = objects_.at(Collider::OBJECT_TYPE::PLAYER_CAP);
	auto& objsC = colliders_.at(Collider::OBJECT_TYPE::CAMERA);
	auto& objsM = colliders_.at(Collider::OBJECT_TYPE::MAP);
	auto& objsMU = colliders_.at(Collider::OBJECT_TYPE::MAP_UNBREAK);

	for (auto c : objsC)
	{
		auto tempC = std::dynamic_pointer_cast<ColliderLine>(c.lock());

		MV1_COLL_RESULT_POLY hit = {};
		bool isHit = false;

		// �}�b�v�I�u�W�F�N�g
		for (auto m : objsM)
		{
			auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

			auto info = MV1CollCheck_LineDim(tempM->GetModelId(), -1,
				tempC->GetPosA(), tempC->GetPosB());
			if (info.HitNum <= 0)
			{
				// �����蔻�茋�ʃ|���S���z��̌�n��������
				MV1CollResultPolyDimTerminate(info);

				continue;
			}

			for (int i = 0; i < info.HitNum; i++)
			{
				auto hit = HitCheck_Line_Triangle(tempC->GetPosA(), tempC->GetPosB(),
					info.Dim[i].Position[0], info.Dim[i].Position[1], info.Dim[i].Position[2]);

				if (hit.HitFlag)
				{
					// �J�����ړ�
					tempC->parent_.lock()->OnHit(hit);
				}
			}

			// �����蔻�茋�ʃ|���S���z��̌�n��������
			MV1CollResultPolyDimTerminate(info);
		}


		// �}�b�v�I�u�W�F�N�g
		for (auto mu : objsMU)
		{
			auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

			auto info = MV1CollCheck_LineDim(tempMU->GetModelId(), -1,
				tempC->GetPosA(), tempC->GetPosB());
			if (info.HitNum <= 0)
			{
				// �����蔻�茋�ʃ|���S���z��̌�n��������
				MV1CollResultPolyDimTerminate(info);

				continue;
			}

			for (int i = 0; i < info.HitNum; i++)
			{
				auto hit = HitCheck_Line_Triangle(tempC->GetPosA(), tempC->GetPosB(),
					info.Dim[i].Position[0], info.Dim[i].Position[1], info.Dim[i].Position[2]);

				if (hit.HitFlag)
				{
					// �J�����ړ�
					tempC->parent_.lock()->OnHit(hit);
				}
			}

			// �����蔻�茋�ʃ|���S���z��̌�n��������
			MV1CollResultPolyDimTerminate(info);
		}

	}

}

void CollisionManager::CollisionGrenadeMap(void)
{
	if (colliders_.count(Collider::OBJECT_TYPE::GRENADE_SPHERE) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::GRENADE_LINE) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP_UNBREAK) <= 0) return;

	auto& objsGS = colliders_.at(Collider::OBJECT_TYPE::GRENADE_SPHERE);
	auto& objsGL = colliders_.at(Collider::OBJECT_TYPE::GRENADE_LINE);
	auto& objsM = colliders_.at(Collider::OBJECT_TYPE::MAP);
	auto& objsMU = colliders_.at(Collider::OBJECT_TYPE::MAP_UNBREAK);

	bool isHit = false;

	for (auto gl : objsGL)
	{

		//VECTOR nea = {};
		//VECTOR neaN = {};
		//float minDis = 0.0f;

		// �Փ˔�����s��Ȃ����̂̓X�L�b�v
		if (!gl.lock()->isCollision) continue;

		auto tempG = std::dynamic_pointer_cast<ColliderLine>(gl.lock());

		if (tempG->parent_.lock()->GetTransform().isHide) continue;

		// �}�b�v�I�u�W�F�N�g
		for (auto m : objsM)
		{
			auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

			auto info = MV1CollCheck_Line(tempM->GetModelId(), -1,
				tempG->GetPosA(), tempG->GetPosB());

			if (!info.HitFlag) continue;

			tempG->parent_.lock()->OnHit(info.Normal, info.HitPosition);
			isHit = true;
			break;
		}

		if (isHit) continue;

		// �j��s�\��
		for (auto mu : objsMU)
		{
			auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

			auto info = MV1CollCheck_Line(tempMU->GetModelId(), -1,
				tempG->GetPosA(), tempG->GetPosB());

			if (!info.HitFlag) continue;

			tempG->parent_.lock()->OnHit(info.Normal, info.HitPosition);
			isHit = true;
			break;
		}
	}

	if (!isHit) return;

	for (auto gs : objsGS)
	{
		// �Փ˔�����s��Ȃ����̂̓X�L�b�v
		if (gs.lock() == nullptr) continue;
		if (!gs.lock()->isCollision) continue;

		auto tempG = std::dynamic_pointer_cast<ColliderSphere>(gs.lock());

		if (tempG->parent_.lock()->GetTransform().isHide) continue;

		// �}�b�v�I�u�W�F�N�g
		for (auto m : objsM)
		{
			auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

			auto info = MV1CollCheck_Sphere(tempM->GetModelId(), -1,
				tempG->pos, tempG->GetRadius());
			if (info.HitNum <= 0)
			{
				// �����蔻�茋�ʃ|���S���z��̌�n��������
				MV1CollResultPolyDimTerminate(info);

				continue;
			}

			for (int i = 0; i < info.HitNum; i++)
			{
				if (tempG->parent_.lock()->GetTransform().isHide) break;

				auto hit = info.Dim[i];
				
				// �Փˌ㏈��
				tempG->parent_.lock()->OnHit(hit);
			}

			// �����蔻�茋�ʃ|���S���z��̌�n��������
			MV1CollResultPolyDimTerminate(info);
		}

		// �j��s�\��
		for (auto mu : objsMU)
		{
			auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

			auto info = MV1CollCheck_Sphere(tempMU->GetModelId(), -1,
				tempG->pos, tempG->GetRadius());
			if (info.HitNum <= 0)
			{
				// �����蔻�茋�ʃ|���S���z��̌�n��������
				MV1CollResultPolyDimTerminate(info);

				continue;
			}

			for (int i = 0; i < info.HitNum; i++)
			{
				if (tempG->parent_.lock()->GetTransform().isHide) break;

				auto hit = info.Dim[i];

				// �Փˌ㏈��
				tempG->parent_.lock()->OnHit(hit);
			}

			// �����蔻�茋�ʃ|���S���z��̌�n��������
			MV1CollResultPolyDimTerminate(info);
		}
	}




}

void CollisionManager::CollisionExplosionMap(void)
{
	if (colliders_.count(Collider::OBJECT_TYPE::EXPLOSION) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP) <= 0) return;

	auto& objsEP = colliders_.at(Collider::OBJECT_TYPE::EXPLOSION);
	auto& objsM = colliders_.at(Collider::OBJECT_TYPE::MAP);

	for (auto ep : objsEP)
	{
		if (!ep.lock()->isCollision)continue;

		auto tempEP = std::dynamic_pointer_cast<ColliderSphere>(ep.lock());

		// �}�b�v�I�u�W�F�N�g
		for (auto m : objsM)
		{
			if (!m.lock()->isCollision)continue;

			auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

			auto info = MV1CollCheck_Sphere(tempM->GetModelId(), -1,
				tempEP->pos, tempEP->GetRadius());
			if (info.HitNum <= 0)
			{
				// �����蔻�茋�ʃ|���S���z��̌�n��������
				MV1CollResultPolyDimTerminate(info);

				continue;
			}

			for (int i = 0; i < info.HitNum; i++)
			{
				auto hit = info.Dim[i];

				// �Փ˂����|���S�����܂܂��t���[�����\���ɂ���
				m.lock()->parent_.lock()->OnHit(hit.FrameIndex);
			}

			// �����蔻�茋�ʃ|���S���z��̌�n��������
			MV1CollResultPolyDimTerminate(info);
		}
	}
}

void CollisionManager::CollisionExplosionPlayer(void)
{
	if (colliders_.count(Collider::OBJECT_TYPE::EXPLOSION) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::PLAYER_CAP) <= 0) return;

	auto& objsEP = colliders_.at(Collider::OBJECT_TYPE::EXPLOSION);
	auto& objsP = colliders_.at(Collider::OBJECT_TYPE::PLAYER_CAP);

	for (auto ep : objsEP)
	{
		if (!ep.lock()->isCollision)continue;
		auto tempEP = std::dynamic_pointer_cast<ColliderSphere>(ep.lock());

		for (auto p : objsP)
		{
			if (!p.lock()->isCollision)continue;

			auto tempP = std::dynamic_pointer_cast<ColliderCapsule>(p.lock());

			auto info = HitCheck_Sphere_Capsule(tempEP->pos, tempEP->GetRadius(),
				tempP->GetPosA(), tempP->GetPosB(), tempP->GetRadius());
			if (!info) continue;
			
			p.lock()->parent_.lock()->OnHit(p, ep);
		}
	}
}

void CollisionManager::CollisionSmokePlayer(void)
{
	if (colliders_.count(Collider::OBJECT_TYPE::SMOKE) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::PLAYER_CAP) <= 0) return;

	auto& objsSP = colliders_.at(Collider::OBJECT_TYPE::SMOKE);
	auto& objsP = colliders_.at(Collider::OBJECT_TYPE::PLAYER_CAP);

	for (auto sp : objsSP)
	{
		if (!sp.lock()->isCollision)continue;
		auto tempEP = std::dynamic_pointer_cast<ColliderSphere>(sp.lock());

		// �}�b�v�I�u�W�F�N�g
		for (auto p : objsP)
		{
			if (!p.lock()->isCollision)continue;

			auto tempP = std::dynamic_pointer_cast<ColliderCapsule>(p.lock());

			auto info = HitCheck_Sphere_Capsule(tempEP->pos, tempEP->GetRadius(),
				tempP->GetPosA(), tempP->GetPosB(), tempP->GetRadius());
			if (!info) continue;

			p.lock()->parent_.lock()->OnHit(p, sp);
		}
	}
}

void CollisionManager::CollisionExplosionEnemy(void)
{
	if (colliders_.count(Collider::OBJECT_TYPE::EXPLOSION) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::ENEMY) <= 0) return;

	auto& objsEP = colliders_.at(Collider::OBJECT_TYPE::EXPLOSION);
	auto& objsE = colliders_.at(Collider::OBJECT_TYPE::ENEMY);

	for (auto ep : objsEP)
	{
		if (!ep.lock()->isCollision)continue;
		auto tempEP = std::dynamic_pointer_cast<ColliderSphere>(ep.lock());

		// �G�l�~�[
		for (auto e : objsE)
		{
			if (!e.lock()->isCollision)continue;

			auto tempE = std::dynamic_pointer_cast<ColliderSphere>(e.lock());

			auto info = HitCheck_Sphere_Sphere(tempEP->pos, tempEP->GetRadius(),
				tempE->pos, tempE->GetRadius());
			if (!info) continue;

			e.lock()->parent_.lock()->OnHit(ep);
		}
	}
}

void CollisionManager::CollisionExplosionShield(void)
{
	if (colliders_.count(Collider::OBJECT_TYPE::EXPLOSION) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::SHIELD) <= 0) return;

	auto& objsEP = colliders_.at(Collider::OBJECT_TYPE::EXPLOSION);
	auto& objsS = colliders_.at(Collider::OBJECT_TYPE::SHIELD);

	for (auto ep : objsEP)
	{
		if (!ep.lock()->isCollision)continue;
		auto tempEP = std::dynamic_pointer_cast<ColliderSphere>(ep.lock());

		// �V�[���h
		for (auto s : objsS)
		{
			if (!s.lock()->isCollision)continue;

			auto tempS = std::dynamic_pointer_cast<ColliderCapsule>(s.lock());

			auto info = HitCheck_Sphere_Capsule(tempEP->pos, tempEP->GetRadius(),
				tempS->GetPosA(), tempS->GetPosB(), tempS->GetRadius());
			if (!info) continue;

			s.lock()->parent_.lock()->OnHit(ep);
		}
	}
}

bool CollisionManager::CollisionRayMap(std::weak_ptr<Collider> obj)
{
	if (colliders_.count(Collider::OBJECT_TYPE::MAP) <= 0 ||
		colliders_.count(Collider::OBJECT_TYPE::MAP_UNBREAK) <= 0) return false;

	auto& objsM = colliders_.at(Collider::OBJECT_TYPE::MAP);
	auto& objsMU = colliders_.at(Collider::OBJECT_TYPE::MAP_UNBREAK);


	auto tempR = std::dynamic_pointer_cast<ColliderLine>(obj.lock());

	MV1_COLL_RESULT_POLY hit = {};
	bool isHit = false;

	// �}�b�v�I�u�W�F�N�g
	for (auto m : objsM)
	{
		auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

		auto info = MV1CollCheck_LineDim(tempM->GetModelId(), -1,
			tempR->GetPosA(), tempR->GetPosB());
		if (info.HitNum <= 0)
		{
			// �����蔻�茋�ʃ|���S���z��̌�n��������
			MV1CollResultPolyDimTerminate(info);

			continue;
		}

		for (int i = 0; i < info.HitNum; i++)
		{
			auto hit = HitCheck_Line_Triangle(tempR->GetPosA(), tempR->GetPosB(),
				info.Dim[i].Position[0], info.Dim[i].Position[1], info.Dim[i].Position[2]);

			if (hit.HitFlag){
				// �����蔻�茋�ʃ|���S���z��̌�n��������
				MV1CollResultPolyDimTerminate(info);

				return true;
			}
		}

		// �����蔻�茋�ʃ|���S���z��̌�n��������
		MV1CollResultPolyDimTerminate(info);
	}


	// �}�b�v�I�u�W�F�N�g
	for (auto mu : objsMU)
	{
		auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

		auto info = MV1CollCheck_LineDim(tempMU->GetModelId(), -1,
			tempR->GetPosA(), tempR->GetPosB());
		if (info.HitNum <= 0)
		{
			// �����蔻�茋�ʃ|���S���z��̌�n��������
			MV1CollResultPolyDimTerminate(info);

			continue;
		}

		for (int i = 0; i < info.HitNum; i++)
		{
			auto hit = HitCheck_Line_Triangle(tempR->GetPosA(), tempR->GetPosB(),
				info.Dim[i].Position[0], info.Dim[i].Position[1], info.Dim[i].Position[2]);

			if (hit.HitFlag){
				// �����蔻�茋�ʃ|���S���z��̌�n��������
				MV1CollResultPolyDimTerminate(info);

				return true;
			}
		}
		// �����蔻�茋�ʃ|���S���z��̌�n��������
		MV1CollResultPolyDimTerminate(info);
	}

	return false;
}

VECTOR CollisionManager::GetGunImpactPos(void)
{
	VECTOR nea = {};

	if (colliders_.count(Collider::OBJECT_TYPE::PLAYER_EYE) <= 0) return nea;

	auto& objsPE = colliders_.at(Collider::OBJECT_TYPE::PLAYER_EYE);

	bool isHit = false;
	
	for (auto pe : objsPE)
	{
		auto tempC = std::dynamic_pointer_cast<ColliderLine>(pe.lock());

		// �}�b�v�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[
		
		if (colliders_.count(Collider::OBJECT_TYPE::MAP) > 0 &&
			colliders_.count(Collider::OBJECT_TYPE::MAP_UNBREAK) > 0)
		{
			auto objsM = colliders_.at(Collider::OBJECT_TYPE::MAP);
			auto objsMU = colliders_.at(Collider::OBJECT_TYPE::MAP_UNBREAK);

			// �}�b�v�I�u�W�F�N�g
			for (auto m : objsM)
			{
				auto tempM = std::dynamic_pointer_cast<ColliderModel>(m.lock());

				auto info = MV1CollCheck_LineDim(tempM->GetModelId(), -1,
					tempC->GetPosA(), tempC->GetPosB());
				if (info.HitNum <= 0)
				{
					// �����蔻�茋�ʃ|���S���z��̌�n��������
					MV1CollResultPolyDimTerminate(info);

					continue;
				}

				for (int i = 0; i < info.HitNum; i++)
				{
					// �傫����0�̏ꍇ�������΂�
					auto scl = tempM->GetScaleFrame(info.Dim[i].FrameIndex);
					if (Utility::EqualsVZero(scl)) continue;

					if (!isHit)
					{
						nea = info.Dim[i].HitPosition;
						isHit = true;
						continue;
					}

					// ret�ƃv���C���[�̋���
					float disA = Utility::Distance(tempC->GetPosA(), nea);

					// info.Dim[i].HitPosition�ƃv���C���[�̋���
					float disB = Utility::Distance(tempC->GetPosA(), info.Dim[i].HitPosition);

					if (disA > disB)
					{
						// ���߂�����nea�ɑ��
						nea = info.Dim[i].HitPosition;
					}

				}

				// �����蔻�茋�ʃ|���S���z��̌�n��������
				MV1CollResultPolyDimTerminate(info);
			}


			// �}�b�v�I�u�W�F�N�g
			for (auto mu : objsMU)
			{
				auto tempMU = std::dynamic_pointer_cast<ColliderModel>(mu.lock());

				auto info = MV1CollCheck_LineDim(tempMU->GetModelId(), -1,
					tempC->GetPosA(), tempC->GetPosB());
				if (info.HitNum <= 0)
				{
					// �����蔻�茋�ʃ|���S���z��̌�n��������
					MV1CollResultPolyDimTerminate(info);

					continue;
				}

				for (int i = 0; i < info.HitNum; i++)
				{
					// �傫����0�̏ꍇ�������΂�
					auto scl = tempMU->GetScaleFrame(info.Dim[i].FrameIndex);
					if (Utility::EqualsVZero(scl)) continue;

					if (!isHit)
					{
						nea = info.Dim[i].HitPosition;
						isHit = true;
						continue;
					}

					// ret�ƃv���C���[�̋���
					float disA = Utility::Distance(tempC->GetPosA(), nea);

					// info.Dim[i].HitPosition�ƃv���C���[�̋���
					float disB = Utility::Distance(tempC->GetPosA(), info.Dim[i].HitPosition);

					if (disA > disB)
					{
						// ���߂�����nea�ɑ��
						nea = info.Dim[i].HitPosition;
					}
				}

				// �����蔻�茋�ʃ|���S���z��̌�n��������
				MV1CollResultPolyDimTerminate(info);
			}
		}
		// �[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[

		// �G�l�~�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[

		//if (colliders_.count(Collider::OBJECT_TYPE::ENEMY) > 0)
		//{
		//	auto objsE = colliders_.at(Collider::OBJECT_TYPE::ENEMY);

		//	// �}�b�v�I�u�W�F�N�g
		//	for (auto e : objsE)
		//	{
		//		auto tempE = std::dynamic_pointer_cast<ColliderSphere>(e.lock());

		//		

		//		auto info = HitCheck_Line_Sphere(tempC->GetPosA(), tempC->GetPosB(),
		//			)
		//		if (info.HitNum <= 0)
		//		{
		//			// �����蔻�茋�ʃ|���S���z��̌�n��������
		//			MV1CollResultPolyDimTerminate(info);

		//			continue;
		//		}

		//		for (int i = 0; i < info.HitNum; i++)
		//		{
		//			if (!isHit)
		//			{
		//				nea = info.Dim[i].HitPosition;
		//				isHit = true;
		//				continue;
		//			}

		//			// ret�ƃv���C���[�̋���
		//			float disA = Utility::Distance(tempC->GetPosA(), nea);

		//			// info.Dim[i].HitPosition�ƃv���C���[�̋���
		//			float disB = Utility::Distance(tempC->GetPosA(), info.Dim[i].HitPosition);

		//			if (disA > disB)
		//			{
		//				// ���߂�����nea�ɑ��
		//				nea = info.Dim[i].HitPosition;
		//			}

		//		}

		//		// �����蔻�茋�ʃ|���S���z��̌�n��������
		//		MV1CollResultPolyDimTerminate(info);
		//	}
		//}
		// �[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[

		if (!isHit) return tempC->GetPosB();

		return nea;
	}


	return nea;
}

CollisionManager::CollisionManager(void)
{
}

CollisionManager::CollisionManager(const CollisionManager&)
{
}
