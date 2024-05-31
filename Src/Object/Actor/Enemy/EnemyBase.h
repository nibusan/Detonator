#pragma once
#include "../../ObjectBase.h"
#include "../ActorBase.h"

class EnemyBase : public ActorBase
{

public:
	EnemyBase(void);
	~EnemyBase(void);


protected:

	//�Ƃ肠����������������X�V�����̊֐������u�����Ă邯��
	//���̃N���X�̔h���N���X�Ŏ������Ă���I
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