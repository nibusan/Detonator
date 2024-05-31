#pragma once
#include "memory"
#include "vector"

class ObjectBase;

// �����̃G�l�~�[�𐶐�����ꍇ�g�p
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
	/// ��������G�l�~�[��ݒ�
	/// </summary>
	/// <param name="id">�G�l�~�[ID</param>
	/// <param name="array">�z��</param>
	/// <param name="num">�ő吶����</param>
	/// <param name="time">�����Ԋu</param>

	void AddFollow(VECTOR* followPos);
	void Update(void);
	void Relese(void);


private:

	// ��������G�l�~�[���i�[����z��
	std::vector<std::shared_ptr<ObjectBase>>& array_;

	// �Ǐ]�Ώ�
	std::vector<VECTOR*> followPos_;

	// ��������G�l�~�[ID
	ENEMY_ID id_;

	// �ő吶����
	int createNum_;

	// �ŏ��̐�����
	int firstNum_;

	// �����Ԋu
	float createTime_;
	float createTimeCnt_;

	void Create(ENEMY_ID id , bool isFirst = false);
	void InitEnemy(std::shared_ptr<ObjectBase> e, ENEMY_ID id);

	std::shared_ptr<ObjectBase> CreateEnemy(ENEMY_ID id, bool isFirst);

	std::shared_ptr<ObjectBase> CreateEnemyNeuvio(bool isFirst);
	std::shared_ptr<ObjectBase> CreateEnemyGeorge(bool isFirst);

};

