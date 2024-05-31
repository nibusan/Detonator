#pragma once
#include <functional>
#include "EnemyBase.h"
#include "../AnimatorController.h"
#include "../../Gun/GunBase.h"
#include "../../../Utility/Utility.h"
#include "../../../Common/Count.h"

//class AnimatorController;

class Armour;

class EnemyGeorge : public EnemyBase
{

public:

	static constexpr int ARMOUR_NUM = (3);
	static constexpr int SHIELD_NUM = (6);
	static constexpr VECTOR ARMOUR_RELATIVE_POS[ARMOUR_NUM] = { { 0.0f,0.0f,0.0f} ,
																{ 0.0f,100.0f,0.0f } ,
																{ 0.0f,200.0f,0.0f } };
	

	// 視野の広さ
	static constexpr float VIEW_RANGE = (1400.0f);
	static constexpr float VIEW_RANGE_EXPANSION = (5000.0f);

	// 視野角
	static constexpr float VIEW_ANGLE = (30.0f);
	static constexpr float VIEW_TIME = (5.0f);

	// 聴覚の広さ
	static constexpr float HEARING_RANGE = (700.0f);
	static constexpr float HEARING_RANGE_EXPANSION = (2500.0f);
	static constexpr float HEARING_SEARCH_TIME = (3.0f);

	// 被ダメ時気にする時間
	static constexpr float DAMAGE_CARE_TIME = (5.0f);

	// 中継地点
	static constexpr int TARGET_NUM = (10);
	static constexpr float TARGET_WAIT_TIME = (2.0f);
	static constexpr VECTOR TARGET_POS[TARGET_NUM] = { { 100.0f,0.0f,-240.0f} ,
													   { 100.0f,0.0f,1180.0f } ,
													   { 750.0f,0.0f,1180.0f } ,
	{ 750.0f,0.0f,2130.0f } ,{ -970.0f,0.0f,2130.0f } ,{ -1440.0f,0.0f,1600.0f } ,
	{ -1440.0f,0.0f,1240.0f } ,
	{ -1030.0f,0.0f,850.0f } ,
	{ -1270.0f,0.0f,40.0f } ,
	{ -400.0f,0.0f,-770.0f } };

	// 回転
	static constexpr float ROTATE_POW = (1.0f / 180.0f * DX_PI_F);

	enum class ACTION
	{
		IDLE,		// 待機
		WALK,
		RUN,
		SHOT,
		RELOAD,
		DEAD
	};

	// 衝突用部位
	enum class BODY
	{
		HEAD,		// 頭
		CHEST,		// 胸
		WAIST,		// 腰
		ARM_LEFT,	// 左腕
		WRIST_LEFT,	// 左手首
		ARM_RIGHT,	// 右腕
		WRIST_RIGHT,// 右手首
		THIGH_LEFT,	// 左太もも
		SHIN_LEFT,	// 左すね
		THIGH_RIGHT,// 右太もも
		SHIN_RIGHT,	// 右すね
		CAP,		// マップとの衝突に使用
		TOPLINE, // マップとの衝突に使用(頭上)
		DOWNLINE,// マップとの衝突に使用(足元)
		EYE,
		MAX,
	};

	enum class STATE
	{
		IDLE,
		ROTATE,
		MOVE,
		//RUN,
		SHOT,
		SEARCH
	};


	EnemyGeorge(std::vector<VECTOR*> followPos, bool isFirst);
	~EnemyGeorge(void) = default;

	void Update_EnemyAfterCollision(void)override;

	// 敵リセット時の初期化
	void EnemyReset(void);

	const VECTOR& GetFollowPos(void) const;
	VECTOR GetPosForCamera(void)const;


	std::shared_ptr<GunBase> GetGun(void);

	// アニメーション通知
	void AnimNotice(int id)override;

	// 衝突したときの処理
	void OnHit()override;
	void OnHit(std::weak_ptr<Collider> obj) override;
	void OnHit(std::weak_ptr<Collider> objA, std::weak_ptr<Collider> objB) override;
	void OnHit(MV1_COLL_RESULT_POLY info) override;
	void OnHit(HITRESULT_LINE info) override;
	void OnHit(VECTOR normal) override;

private:

	// デフォルトハンドル
	int modelGrHandle_;
	int modelGrHandle2_;

	// 強調用ハンドル
	int colorGrHandle_;

	// 追従対象
	std::vector<VECTOR*> followPos_;

	// アニメーターコントローラ
	std::unique_ptr<AnimatorController> animCon_;

	// 現在のアクション
	ACTION act_;
	ACTION preAct_;
	ACTION actUp_;
	ACTION preActUp_;

	// アクション中かどうか(移動以外)
	//bool isAct_;

	// 衝突判定用
	std::shared_ptr<Collider> colliders_[(int)BODY::MAX];

	// 加害者(最後に攻撃してきたオブジェクト)
	std::weak_ptr<ObjectBase> lastAtkObj_;

	// 銃
	std::shared_ptr<GunBase> gun_;

	// 銃の状態
	GunBase::STATE gunState_;

	// シールド
	std::shared_ptr<Armour> armours_[ARMOUR_NUM];

	STATE state_;
	STATE preState_;

	// 状態管理(更新ステップ)
	std::function<void(void)> stateUpdate_;

	Count waitTime_;
	Count searchHearingTime_;
	Count searchViewTime_;
	Count damageCareTime_;
	
	int targetIndex_;
	VECTOR targetPos_;
	VECTOR targetGoalRot_;

	bool isRotate_;

	// 腰オイラー
	VECTOR waistRot_;

	// 検知フラグ
	bool isNotice_;
	bool isNoticeHearing_;
	bool isNoticeDamage_;

	int aliveShieldNum_;

	void Init_Enemy(void)override;
	void Update_Enemy(void)override;
	void Draw_Enemy(void)override;
	void Release_Enemy(void)override;

	// 座標回転など更新
	void TransformUpdate(void);

	// アニメーション設定
	void AnimationSetting(void);

	// アクション変更
	void ChangeAction(ACTION act);
	void ChangeActionUp(ACTION actUp);

	// コライダー設定
	void ColliderSetting(void);

	// コライダー座標更新
	void UpdateCollider(void);

	// 死亡時処理
	void ProcessDead(void)override;

	// 死亡中処理
	void DeadUpdate(void);

	
	void ChangeState(STATE state);

	void ChangeGunState(GunBase::STATE state);


	void SearchUpdate(void);
	void SearchExpansionUpdate(void);

	void Idle(void);
	void Rotation(void);
	void WaistRotate(float goalRotX);
	void LazyRotationX(float goalRotX);
	void LazyRotationY(float goalRotY);
	void Move(void);
	void Shot(void);
	void Search(void);


};

