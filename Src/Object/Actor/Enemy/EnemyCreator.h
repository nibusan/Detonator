#pragma once
#include "memory"
#include "vector"

class ObjectBase;

// 複数体エネミーを生成する場合使用
class EnemyCreator
{
public:

	enum class ENEMY_ID
	{
		NEUVIO,
		GEORGE,
	};

	EnemyCreator(std::vector<std::shared_ptr<ObjectBase>>& array);
	~EnemyCreator() = default;

	void Init(ENEMY_ID id, int num = 1, float time = 10.0f, int firstNum = 0);

	/// <summary>
	/// 生成するエネミーを設定
	/// </summary>
	/// <param name="id">エネミーID</param>
	/// <param name="array">配列</param>
	/// <param name="num">最大生成数</param>
	/// <param name="time">生成間隔</param>

	void AddFollow(VECTOR* followPos);
	void Update(void);
	void Relese(void);


private:

	// 生成するエネミーを格納する配列
	std::vector<std::shared_ptr<ObjectBase>>& array_;

	// 追従対象
	std::vector<VECTOR*> followPos_;

	// 生成するエネミーID
	ENEMY_ID id_;

	// 最大生成数
	int createNum_;

	// 最初の生成数
	int firstNum_;

	// 生成間隔
	float createTime_;
	float createTimeCnt_;

	void Create(ENEMY_ID id , bool isFirst = false);
	void InitEnemy(std::shared_ptr<ObjectBase> e, ENEMY_ID id);

	std::shared_ptr<ObjectBase> CreateEnemy(ENEMY_ID id, bool isFirst);

	std::shared_ptr<ObjectBase> CreateEnemyNeuvio(bool isFirst);
	std::shared_ptr<ObjectBase> CreateEnemyGeorge(bool isFirst);

};

