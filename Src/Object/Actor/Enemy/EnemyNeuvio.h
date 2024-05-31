#pragma once
#include <unordered_map>
#include "../../Bullet/BulletBase.h"
#include "../../../Manager/AudioManager.h"
#include "EnemyBase.h"

class EnemyNeuvio : public EnemyBase
{

public:

	// �G�@���ʂ钆�p�n�_
	enum class POINT
	{
		A,
		B,
		C,
	};

	// �G�̍s��
	enum class ACTION
	{
		FIXED_TURRET,	// �Œ�C��
		FOLLOW,			// �ǔ�
		PASS,			// �X���[
		RUSH,			// �ːi
	};

	// ����̍L��
	static constexpr float VIEW_RANGE = (1300.0f);

	// ����p
	static constexpr float VIEW_ANGLE = (30.0f);

	// ���o�̍L��
	static constexpr float HEARING_RANGE = (1300.0f);

	// �G�̑��x
	static constexpr float ENEMY_SPEED = 3.0f;

	// �G�̍���
	static constexpr float ENEMY_HIGH = 500.0f;

	// ���p�n�_�̔��a
	static constexpr float POINT_RADIUS = 10.0f;

	// ��]��(deg)
	static constexpr float SPEED_ROT_ENEMY = 3.0f;

	// ��]��(rad)
	static constexpr float SPEED_ROT_RAD_ENEMY = SPEED_ROT_ENEMY * (DX_PI_F / 180.0f);

	// �v���C���[�Ƃ̋���
	static constexpr float PLAYER_DISTANCE = 700.0f;

	// �����蔻��p���a
	static constexpr float COL_RAD = 60.0f;

	// �����蔻��p���΍��W
	static constexpr VECTOR LOCAL_COL_POS = {0.0f,50.0f,-20.0f};

	// �e���˗p���΍��W
	static constexpr VECTOR LOCAL_SHOT_POS = { 0.0f,-50.0f,0.0f };

	// �G�@���ʂ钆�p�n�_
	static constexpr VECTOR POINT_A = { 0.0f,500.0f,0.0f };
	static constexpr VECTOR POINT_B = { 500.0f,500.0f,500.0f };
	static constexpr VECTOR POINT_C = { -500.0f,500.0f,500.0f };

	// �ǂɏՓ˂������̉���|�C���g
	static constexpr VECTOR POINT_AVOID = { 0.0f,300.0f,0.0f };

	// �G���Ƃ̃X�R�A
	static constexpr int FIXED_TURRET_SCORE = 2000;
	static constexpr int FOLLOW_SCORE = 3000;
	static constexpr int PASS_SCORE = 500;
	static constexpr int RUSH_SCORE = 2000;

	// �G���X�|�[������
	static constexpr float F_ENEMY_RESPAWN_POS_X = 2400.0f;
	static constexpr float B_ENEMY_RESPAWN_POS_X = -1900.0f;
	static constexpr float F_ENEMY_RESPAWN_POS_Y = 400.0f;
	static constexpr float B_ENEMY_RESPAWN_POS_Y = 100.0f;
	static constexpr float F_ENEMY_RESPAWN_POS_Z = 5000.0f;
	static constexpr float B_ENEMY_RESPAWN_POS_Z = -2300.0f;

	EnemyNeuvio(std::vector<VECTOR*> followPos,bool isFirst);
	~EnemyNeuvio(void);

	// �Փ˂����Ƃ��̏���
	void OnHit(std::weak_ptr<Collider> obj) override;
	void OnHit(MV1_COLL_RESULT_POLY info) override;

	// �G���Z�b�g���̏�����
	void EnemyReset(void);


	 
	
	//void CreateEnemy(std::shared_ptr<ObjectBase> enemys);

private:

	AudioManager& audioManager_;

	// �e
	std::vector<std::shared_ptr<BulletBase>> bullets_;

	std::unordered_map<POINT, VECTOR> points_;

	std::shared_ptr<Collider> collider_;

	std::shared_ptr<Collider> colliderEye_;

	int soundHandle_;

	// �G�@���ʂ钆�p�n�_
	POINT point_;

	// �G�̍s��
	ACTION action_;

	// �Ǐ]�Ώ�
	std::vector<VECTOR*> followPos_;
	VECTOR followRushPos_;

	// �ŏ����ǂ���
	bool isFirst_;
	bool isFirstCol_;

	// �G�@���ʂ钆�p�n�_
	VECTOR pointA_;
	VECTOR pointB_;
	VECTOR pointC_;

	// �ːi���̌���
	VECTOR rushDir_;

	// �A�j���[�V�����ԍ�
	int animAttachNo_;

	// �e�̔��ˈʒu
	VECTOR barrelPos_;

	// �e�̃��f��ID
	int bulletModelId_;

	// �o�����t���[���ԍ�(�e�̔��ˈʒu)
	int barrelFrameNo_;

	// ���ˊԊu
	float shotDelay_;
	float shotDelayCnt_;

	float deg_;

	// ��]�����ǂ���
	bool isRotate_;

	// ���m�t���O
	bool isNotice_;
	bool isNoticeHearing_;
	bool isNoticeBack_;

	// �Ǐ]�t���O
	bool isFollow_;

	// �ːi�t���O
	bool isRush_;

	// �����蔻��t���O
	bool isCollision_;

	// �f�o�b�O�p
	bool debugFlg_;

	void Init_Enemy(void)override;
	void Update_Enemy(void)override;
	void Draw_Enemy(void)override;
	void Release_Enemy(void)override;

	// �ړ�����
	void ProcessMove(void);
	void GoToPoint(VECTOR start,VECTOR end,POINT point);

	// ��]����
	void Rotation(VECTOR end);
	void LazyRotation(float goalRotY, float goalRotX);

	// ���`���
	VECTOR Lerp(VECTOR start, VECTOR end, float speed);

	// �s������
	void EnemyAction(void);
	void GoToPoint(void);

	// ���[�V����
	void Motion(void);

	// �ǔ�����
	void Follow(void);

	// �T�[�`
	void Search(void);

	// �ːi
	void Rush(void);

	// �����蔻�萧��
	void SyncCollision(void);

	// �e����
	void CreateBullet(void);

	// �e�𔭎ˏ�Ԃɐݒ�
	void SetBullet(std::shared_ptr<BulletBase> bullet);

	// ���ˊԊu����
	void ShotDelay(void);

	// �e���ˏ���
	void ProcessShot(void);

	// ���S������
	void ProcessDead(void)override;

	// �x�N�g����Z
	VECTOR VMult(VECTOR vec1, VECTOR vec2);

	// �f�o�b�O
	void DrawDebug(void);

};

