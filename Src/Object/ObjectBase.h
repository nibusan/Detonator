#pragma once
#include <DxLib.h>
#include "Common/Transform.h"
#include "Common/Collider.h"
#include "../Common/Handle/Model/Model.h"
#include "ObjectID.h"


class ObjectBase : public std::enable_shared_from_this<ObjectBase>
{

public:
	ObjectBase(void);
	~ObjectBase(void);

	virtual void Init(void);
	virtual void Update(void);
	virtual void Draw(void);
	virtual void Release(void);

	// 半透明描画
	virtual void DrawTrans(void);

	virtual void UpdateAfterCollision(void);

	/// @brief オブジェクトのデータを返す
	/// @return オブジェクトデータ
	[[nodiscard]] const Transform& GetTransform(void) const;


	// 衝突したときの処理
	virtual void OnHit();
	virtual void OnHit(std::weak_ptr<Collider> obj);
	virtual void OnHit(std::weak_ptr<Collider> objA, std::weak_ptr<Collider> objB);
	virtual void OnHit(int no);
	virtual void OnHit(MV1_COLL_RESULT_POLY info);
	virtual void OnHit(HITRESULT_LINE info);
	virtual void OnHit(VECTOR normal);
	virtual void OnHit(VECTOR normal, VECTOR hitPos);

	const ID& GetID(void) const;


	// 自分自身のshared_ptrを返す
	std::shared_ptr<ObjectBase> GetPtr() { return shared_from_this(); }

protected:

	// モデルの情報(位置,大きさ,角度等)
	Transform transform_;

	std::unique_ptr<Model> model_;

	virtual void Init_Object(void) = 0;
	virtual void Update_Object(void) = 0;
	virtual void Draw_Object(void) = 0;
	virtual void Release_Object(void) = 0;

	// オブジェクトに設定するID(必ず初期化)
	ID ID_;


};

