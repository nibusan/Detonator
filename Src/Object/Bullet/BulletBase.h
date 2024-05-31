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

	// ���ˏ���
	void ReadyBullet(VECTOR pos, VECTOR dir);

	// �e���폜(��\��)
	void ResetBullet(void);

	const float& GetDamage(void) const;
	void SetDamage(float damage);



	void UpdateAfterCollision(void) override;

	const std::weak_ptr<ActorBase>& GetParent(void) const;

	// �e���Ǝ��̐ݒ�
	virtual void SetParam(void) = 0;

	// �Փ˂����Ƃ��̏���
	void OnHit(std::weak_ptr<Collider> obj) override;
	void OnHit(VECTOR hitPos) override;


protected:

	// �R���C�_�[
	std::shared_ptr<Collider> collider_;

	// ���f���t�@�C����
	std::string fileName_;

	// ���L���̃|�C���^
	std::weak_ptr<ActorBase> parent_;

	//int effectHandle_;

	// �ړ��p
	// �X�s�[�h
	float speed_;

	// ����
	VECTOR dir_;

	// ���̑��p�����[�^
	// ��������
	float aliveTime_;
	float aliveTimeCnt_;

	// �_���[�W
	float damage_;

	int team_;
	float controlPosZ_;
		
	void Init_Object(void) override;
	void Update_Object(void) override;
	void Draw_Object(void) override;
	void Release_Object(void) override;

	// �ړ�����
	void Move(void);

	// �������ԏ���
	void AliveTime(void);

};

