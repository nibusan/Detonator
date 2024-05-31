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

	// ��������
	void Throw(VECTOR dir, float pow);

	// �������I��������ǂ���
	bool IsEnd(void);
	bool IsHold(void);

	// �Փ˂����Ƃ��̏���
	virtual void OnHit(MV1_COLL_RESULT_POLY info) override;


	// �^�C�v
	GRENADE_TYPE GetType(void);

	// ��Ɏ���
	void Hold(void);

	const float& GetDamage(void) const;
	const std::weak_ptr<ActorBase>& GetParent(void) const;

	// ����
	virtual void Blast(void) = 0;

protected:

	NetManager& netManager_;

	// ���L���̃|�C���^
	std::weak_ptr<ActorBase> parent_;


	// �G�t�F�N�gID
	int effectID_;

	STATE state_;

	// �Ǐ]�Ώ�
	const Transform& follow_;

	// �Ǐ]�t���[���ԍ�
	int followFrameNo_;

	// ���΍��W
	VECTOR relativePos_;

	// ���f���t�@�C����
	std::string fileName_;

	// �^�C�v
	GRENADE_TYPE type_;

	// �R���C�_�[
	std::shared_ptr<Collider> collider_;
	std::shared_ptr<Collider> colliderLine_;

	// �ړ���(XZ�����̂�)
	// Y������transform�ŏd�͂�ݒ肵�Ă��邽��
	float pow_;
	VECTOR moveDir_;

	// ��ԊǗ�(�X�V�X�e�b�v)
	std::function<void(void)> stateUpdate_;

	float damage_;

	Count blastEffect_;

	void Init_Object(void) override;
	void Update_Object(void) override;
	void Draw_Object(void) override;
	void Release_Object(void) override;

	// �e���Ǝ��̐ݒ�
	virtual void SetParam(void) = 0;

	void ChangeState(STATE state);

	// �ړ�
	virtual void UpdateNone(void);

	// �ΏۂɒǏ]
	virtual void UpdateFollow(void);


	virtual void UpdateBlast(void);

	virtual void UpdateEnd(void);

	// ���ꂼ��̏�����
	virtual void Reset(void) = 0;
};

