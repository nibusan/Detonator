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
	// 最大生成数
	createNum_ = num;

	// 生成間隔
	createTime_ = time;
	createTimeCnt_ = 0.0f;

	id_ = id;

	// 最初の生成数
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
		// エネミー生成
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
	
	// 作成フラグ
	bool isCreate = false;

	// 再利用可能なものを探索
	for (auto e : array_)
	{
		// 非表示状態かどうか
		if (!e->GetTransform().isHide) continue;

		// エネミーを初期化
		InitEnemy(e, id);
		

		isCreate = true;
	}

	if (isCreate ||
		array_.size() >= createNum_) return;

	// 新たに作成

	// エネミーを作成
	auto temp = CreateEnemy(id,isFirst);

	// 配列に格納
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
