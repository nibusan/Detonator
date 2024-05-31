#pragma once
#include "../../ObjectBase.h"
#include "../ActorBase.h"

class EnemyBase : public ActorBase
{

public:
	EnemyBase(void);
	~EnemyBase(void);


protected:

	//とりあえず初期化処理や更新処理の関数を仮置きしてるけど
	//このクラスの派生クラスで実装してくれ！
	void Init_Actor(void) override;
	void Update_Actor(void) override;
	void Draw_Actor(void) override;
	void Release_Actor(void) override;

	virtual void Init_Enemy(void) = 0;
	virtual void Update_Enemy(void) = 0;
	virtual void Draw_Enemy(void) = 0;
	virtual void Release_Enemy(void) = 0;

	virtual void Update_EnemyAfterCollision(void);

};