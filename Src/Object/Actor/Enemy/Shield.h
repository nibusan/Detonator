#pragma once
#include "../ActorBase.h"

class Armour;

class Shield : public ActorBase
{
public:

	Shield(std::weak_ptr<Armour>follow,VECTOR pos, VECTOR rot);
	~Shield();

	
	void SetPos(VECTOR pos);
	void SetRot(VECTOR rot);

	// 衝突したときの処理
	void OnHit(std::weak_ptr<Collider> obj) override;

private:

	std::weak_ptr<Armour>follow_;
	std::shared_ptr<Collider> collider_;

	void Init_Actor(void) override;
	void Update_Actor(void) override;
	void Draw_Actor(void) override;
	void Release_Actor(void) override;

	// コライダー座標更新
	virtual void UpdateCollider(void);

	// 死亡時処理
	void ProcessDead(void) override;
};

