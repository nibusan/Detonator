#pragma once
#include <Vector>
#include "../ObjectBase.h"


class ActorBase : public ObjectBase
{

public:

	ActorBase(void);
	~ActorBase(void);

	// アニメーション終了後の処理
	virtual void AnimEnd(int id);

	// アニメーション通知
	virtual void AnimNotice(int id);

	// 耐久値減少
	virtual void Damage(float damage);

	// 死亡時処理
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

	// 最後に攻撃してきたObjectのID
	ID lastAtkID_;


	// 耐久値
	float hp_;
	float hpMax_;
};

