#pragma once
#include <Vector>
#include "../ObjectBase.h"


class ActorBase : public ObjectBase
{

public:

	ActorBase(void);
	~ActorBase(void);

	// �A�j���[�V�����I����̏���
	virtual void AnimEnd(int id);

	// �A�j���[�V�����ʒm
	virtual void AnimNotice(int id);

	// �ϋv�l����
	virtual void Damage(float damage);

	// ���S������
	virtual void ProcessDead(void);

	const ID& GetLastAtkID(void) const;
	const float& GetHP(void) const;
	const float& GetHPMAX(void) const;

protected:
	

	void Init_Object(void) override;
	void Update_Object(void) override;
	void Draw_Object(void) override;
	void Release_Object(void) override;

	virtual void Init_Actor(void) = 0;
	virtual void Update_Actor(void) = 0;
	virtual void Draw_Actor(void) = 0;
	virtual void Release_Actor(void) = 0;

	// �Ō�ɍU�����Ă���Object��ID
	ID lastAtkID_;


	// �ϋv�l
	float hp_;
	float hpMax_;
};

