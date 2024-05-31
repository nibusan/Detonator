#include "../../../Manager/SceneManager.h"
#include "../../../Manager/GameManager.h"
#include "EnemyNeuvio.h"
#include "EnemyGeorge.h"
#include "EnemyCreator.h"

EnemyCreator::EnemyCreator(std::vector<std::shared_ptr<ObjectBase>>& array) : array_(array) 
{

}

void EnemyCreator::Init(ENEMY_ID id, int num, float time, int firstNum)
{
	// �ő吶����
	createNum_ = num;

	// �����Ԋu
	createTime_ = time;
	createTimeCnt_ = 0.0f;

	id_ = id;

	// �ŏ��̐�����
	firstNum_ = firstNum;
}

void EnemyCreator::AddFollow(VECTOR* followPos)
{
	followPos_.emplace_back(followPos);
}

void EnemyCreator::Update(void)
{

	while (firstNum_-- > 0)
	{
		Create(id_, firstNum_);
	}

	if (createTimeCnt_ <= 0.0f)
	{
		// �G�l�~�[����
		Create(id_);

		createTimeCnt_ = createTime_;
	}
	else
	{
		createTimeCnt_ -= SceneManager::GetInstance().GetDeltaTime();
		if (createTimeCnt_ <= 0.0f) createTimeCnt_ = 0.0f;
	}
}

void EnemyCreator::Relese(void)
{
	array_.clear();
	followPos_.clear();
	createTimeCnt_ = 0.0f;
}

void EnemyCreator::Create(ENEMY_ID id, bool isFirst)
{

	//auto& info = GameManager::GetInstance();

	//if (info.GetTimeToStart() >= 5.0f)return;
	
	// �쐬�t���O
	bool isCreate = false;

	// �ė��p�\�Ȃ��̂�T��
	for (auto e : array_)
	{
		// ��\����Ԃ��ǂ���
		if (!e->GetTransform().isHide) continue;

		// �G�l�~�[��������
		InitEnemy(e, id);
		

		isCreate = true;
	}

	if (isCreate ||
		array_.size() >= createNum_) return;

	// �V���ɍ쐬

	// �G�l�~�[���쐬
	auto temp = CreateEnemy(id,isFirst);

	// �z��Ɋi�[
	array_.emplace_back(temp);

}

void EnemyCreator::InitEnemy(std::shared_ptr<ObjectBase> e, ENEMY_ID id)
{

	switch (id)
	{
	case EnemyCreator::ENEMY_ID::NEUVIO:
		std::dynamic_pointer_cast<EnemyNeuvio>(e)->EnemyReset();
		break;
	case EnemyCreator::ENEMY_ID::GEORGE:
		std::dynamic_pointer_cast<EnemyGeorge>(e)->EnemyReset();
		break;
	}

}

std::shared_ptr<ObjectBase> EnemyCreator::CreateEnemy(ENEMY_ID id, bool isFirst)
{
	switch (id)
	{
	case EnemyCreator::ENEMY_ID::NEUVIO:

		return CreateEnemyNeuvio(isFirst);

		break;
	case EnemyCreator::ENEMY_ID::GEORGE:

		return CreateEnemyGeorge(isFirst);

		break;
	}

	return nullptr;
}

std::shared_ptr<ObjectBase> EnemyCreator::CreateEnemyNeuvio(bool isFirst)
{
	std::shared_ptr<ObjectBase> base = std::make_shared<EnemyNeuvio>(followPos_,isFirst);
	base->Init();

	return base;
}

std::shared_ptr<ObjectBase> EnemyCreator::CreateEnemyGeorge(bool isFirst)
{
	std::shared_ptr<ObjectBase> base = std::make_shared<EnemyGeorge>(followPos_, isFirst);
	base->Init();

	return base;
}
