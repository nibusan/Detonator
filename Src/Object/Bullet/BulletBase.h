#pragma once
#include "../ObjectBase.h"
#include "../Common/Recoil.h"

class Collider;
class ActorBase;

class BulletBase : public ObjectBase
{

public:

	BulletBase(int modelId, std::weak_ptr<ActorBase> parent);
	~BulletBase(void);

	// 発射準備
	void ReadyBullet(VECTOR pos, VECTOR dir);

	// 弾を削除(非表示)
	void ResetBullet(void);

	const float& GetDamage(void) const;
	void SetDamage(float damage);



	void UpdateAfterCollision(void) override;

	const std::weak_ptr<ActorBase>& GetParent(void) const;

	// 各自独自の設定
	virtual void SetParam(void) = 0;

	// 衝突したときの処理
	void OnHit(std::weak_ptr<Collider> obj) override;
	void OnHit(VECTOR hitPos) override;


protected:

	// コライダー
	std::shared_ptr<Collider> collider_;

	// モデルファイル名
	std::string fileName_;

	// 所有元のポインタ
	std::weak_ptr<ActorBase> parent_;

	//int effectHandle_;

	// 移動用
	// スピード
	float speed_;

	// 方向
	VECTOR dir_;

	// その他パラメータ
	// 生存時間
	float aliveTime_;
	float aliveTimeCnt_;

	// ダメージ
	float damage_;

	int team_;
	float controlPosZ_;
		
	void Init_Object(void) override;
	void Update_Object(void) override;
	void Draw_Object(void) override;
	void Release_Object(void) override;

	// 移動処理
	void Move(void);

	// 生存時間処理
	void AliveTime(void);

};

