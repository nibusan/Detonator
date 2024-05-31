#pragma once
#include <functional>
#include"../../Net/NetManager.h"
#include"../../Common/Count.h"
#include "../ObjectBase.h"
#include "GrenadeType.h"

class Collider;
class ActorBase;

class GrenadeBase : public ObjectBase
{
public:

	static constexpr float RADIUS_DEF = (10.0f);



	enum class STATE
	{
		NONE,
		FOLLOW,
		BLAST,
		END
	};

	GrenadeBase(const Transform& follow, int frameNo, std::weak_ptr<ActorBase> parent);
	~GrenadeBase(void);

	// 投げられる
	void Throw(VECTOR dir, float pow);

	// 爆発し終わったかどうか
	bool IsEnd(void);
	bool IsHold(void);

	// 衝突したときの処理
	virtual void OnHit(MV1_COLL_RESULT_POLY info) override;


	// タイプ
	GRENADE_TYPE GetType(void);

	// 手に持つ
	void Hold(void);

	const float& GetDamage(void) const;
	const std::weak_ptr<ActorBase>& GetParent(void) const;

	// 爆発
	virtual void Blast(void) = 0;

protected:

	NetManager& netManager_;

	// 所有元のポインタ
	std::weak_ptr<ActorBase> parent_;


	// エフェクトID
	int effectID_;

	STATE state_;

	// 追従対象
	const Transform& follow_;

	// 追従フレーム番号
	int followFrameNo_;

	// 相対座標
	VECTOR relativePos_;

	// モデルファイル名
	std::string fileName_;

	// タイプ
	GRENADE_TYPE type_;

	// コライダー
	std::shared_ptr<Collider> collider_;
	std::shared_ptr<Collider> colliderLine_;

	// 移動量(XZ方向のみ)
	// Y方向はtransformで重力を設定しているため
	float pow_;
	VECTOR moveDir_;

	// 状態管理(更新ステップ)
	std::function<void(void)> stateUpdate_;

	float damage_;

	Count blastEffect_;

	void Init_Object(void) override;
	void Update_Object(void) override;
	void Draw_Object(void) override;
	void Release_Object(void) override;

	// 各自独自の設定
	virtual void SetParam(void) = 0;

	void ChangeState(STATE state);

	// 移動
	virtual void UpdateNone(void);

	// 対象に追従
	virtual void UpdateFollow(void);


	virtual void UpdateBlast(void);

	virtual void UpdateEnd(void);

	// それぞれの初期化
	virtual void Reset(void) = 0;
};

