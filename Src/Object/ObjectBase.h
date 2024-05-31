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

	// �������`��
	virtual void DrawTrans(void);

	virtual void UpdateAfterCollision(void);

	/// @brief �I�u�W�F�N�g�̃f�[�^��Ԃ�
	/// @return �I�u�W�F�N�g�f�[�^
	[[nodiscard]] const Transform& GetTransform(void) const;


	// �Փ˂����Ƃ��̏���
	virtual void OnHit();
	virtual void OnHit(std::weak_ptr<Collider> obj);
	virtual void OnHit(std::weak_ptr<Collider> objA, std::weak_ptr<Collider> objB);
	virtual void OnHit(int no);
	virtual void OnHit(MV1_COLL_RESULT_POLY info);
	virtual void OnHit(HITRESULT_LINE info);
	virtual void OnHit(VECTOR normal);
	virtual void OnHit(VECTOR normal, VECTOR hitPos);

	const ID& GetID(void) const;


	// �������g��shared_ptr��Ԃ�
	std::shared_ptr<ObjectBase> GetPtr() { return shared_from_this(); }

protected:

	// ���f���̏��(�ʒu,�傫��,�p�x��)
	Transform transform_;

	std::unique_ptr<Model> model_;

	virtual void Init_Object(void) = 0;
	virtual void Update_Object(void) = 0;
	virtual void Draw_Object(void) = 0;
	virtual void Release_Object(void) = 0;

	// �I�u�W�F�N�g�ɐݒ肷��ID(�K��������)
	ID ID_;


};

