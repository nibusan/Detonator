#pragma once
#include <unordered_map>
#include "../Object/Common/Collider.h"

class CollisionManager
{
public:

	// �}�b�v���ʂƍs���Փ˔����
	static constexpr int MAP_COL_MAX = (30);

	// �C���X�^���X�̐���
	static void CreateInstance(void);

	// �C���X�^���X�̎擾
	static CollisionManager& GetInstance(void);

	void Init(void);

	// ���\�[�X�̔j��
	void Destroy(void);

	// �R���C�_�[�z��N���A
	void ClearColliders(void);

	// �R���C�_�[�ǉ�
	void SetCollider(std::weak_ptr<Collider> col);

	// �e�����Ȃ��Ȃ����R���C�_�[�������I�ɍ폜
	void DeleteColliderNonParent(void);

	// ����̃R���C�_�[���폜
	void DeleteCollider(std::weak_ptr<Collider> col);

	// �Փ˔���(�V���O���v���C�p)
	void CollisionSingle(void);
	// �Փ˔���(�}���`�v���C�p)
	void CollisionMulti(void);

	// �v���C���[�ƃ}�b�v�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[
	// ������4�񕪗]���Ƀ}�b�v�Ƃ̏Փ˔�����s���Ă���̂ŏC���\��
	// �@�v���C���[�ƃ}�b�v�̏Փ˔���ŏՓ˃|���S���z����擾
	// �A�Փ˃|���S���z���@���x�N�g�����Ƃɂ킯��
	// �B�e����
	void CollisionPlayerMap(void);

	// �v���C���[�ƃ}�b�v(��)
	void CollisionPlayerMapSide(const VECTOR movePow, std::weak_ptr<Collider> player, int no);
	void CollisionPlayerMapSide2(const VECTOR movePow, std::weak_ptr<Collider> player, int no);

	// �v���C���[�ƃ}�b�v(�V��)
	void CollisionPlayerMapHead(std::weak_ptr<Collider> player, int no);

	// �v���C���[�ƃ}�b�v(�n��)
	void CollisionPlayerMapFoot(std::weak_ptr<Collider> player, int no);

	// �v���C���[�ƃ}�b�v(�΂�)
	void CollisionPlayerMapSlopeUp(std::weak_ptr<Collider> player, int no);	// �@���x�N�g����
	void CollisionPlayerMapSlopeDown(std::weak_ptr<Collider> player, int no);	// �@���x�N�g����

	// �[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[


	// �v���C���[�ƃv���C���[�̒e
	void CollisionPlayerBullet(void);

	// �v���C���[�ƃG�l�~�[�̒e
	void CollisionPlayerEnemyBullet(void);

	// �v���C���[�̒e�ƃG�l�~�[
	void CollisionPlayerBulletEnemy(void);

	// �v���C���[�̒e�ƃ}�b�v
	void CollisionPlayerBulletMap(void);

	// �v���C���[�̒e�ƃV�[���h
	void CollisionPlayerBulletShield(void);

	// �G�l�~�[�ƃ}�b�v
	void CollisionEnemyMap(void);

	// �G�l�~�[�̒e�ƃ}�b�v
	void CollisionEnemyBulletMap(void);

	// �G�l�~�[�̖ڐ��ƃ}�b�v
	void CollisionEnemyEyeMap(void);
	bool IsCollisionEnemyEyeMap(std::weak_ptr<Collider> obj);

	// �J�����ƃ}�b�v
	void CollisionCameraMap(void);

	// �O���l�[�h�ƃ}�b�v
	void CollisionGrenadeMap(void);

	// �����ƃ}�b�v�̏Փ�
	void CollisionExplosionMap(void);

	// �����ƃv���C���[
	void CollisionExplosionPlayer(void);
	void CollisionSmokePlayer(void);

	// �����ƃG�l�~�[
	void CollisionExplosionEnemy(void);

	// �v���C���[�̒e�ƃV�[���h
	void CollisionExplosionShield(void);

	// �ː��ƃ}�b�v�̏Փ˔���
	bool CollisionRayMap(std::weak_ptr<Collider> obj);


	// �v�Z��̒l��Ԃ��[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[

	// �v���C���[���王�_�����Ƀ��C���΂��A
	// �Փ˂����I�u�W�F�N�g�̒�����ł��߂��_��Ԃ�
	// ���ꂪ�e�̒����_�ɂȂ�
	VECTOR GetGunImpactPos(void);


	// �[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[	




private:

	// �ÓI�C���X�^���X
	static CollisionManager* instance_;

	// �f�t�H���g�R���X�g���N�^��private�ɂ��āA
	// �O�����琶���ł��Ȃ��l�ɂ���
	CollisionManager(void);

	// �R�s�[�R���X�g���N�^�����l
	CollisionManager(const CollisionManager&);

	// �f�X�g���N�^�����l
	~CollisionManager(void) = default;

	// weak�ł�������
	// �Փ˔���R���C�_�[
	std::unordered_map<Collider::OBJECT_TYPE, std::vector<std::weak_ptr<Collider>>> colliders_;

};

