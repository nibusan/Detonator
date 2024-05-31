#pragma once
#include <unordered_map>
#include "memory"
#include"../../../Manager/Controller/PlayerController.h"
#include"../../../Manager/GameManager.h"
#include"../../../Manager/LoadOut.h"
#include "../../ObjectBase.h"
#include"../../../Object/Gun/GunType.h"
#include"../../../Object/Grenade/GrenadeType.h"
#include "../ActorBase.h"
#include "../../Gun/GunBase.h"
#include "../../Grenade/GrenadeBase.h"
#include "../../../Common/Count.h"

class AnimatorController;
class Recoil;

class Player : public ActorBase
{

public:


	enum class ACTION
	{
		IDLE,		// 待機
		WALK_FORWARD,	// 歩く(前)
		WALK_LEFT,		// 歩く(左)
		WALK_RIGHT,		// 歩く(右)
		WALK_BACK,		// 歩く(後)
		RUN,		// 走る
		//JUMP_UP,		// ジャンプ開始
		JUMP_NOW,		// ジャンプ中
		SHOT,		// 撃つ
		RELOAD,		// リロード
		GRENADE,		// グレネード
		//DAMAGE,		// ダメージ
		DEAD,		// 死ぬ
		// 移動しながら関連
		
		// しゃがみ関連ーーーーーーーーーーーーーーー
		CROUCH,				// しゃがむ
		CROUCH_WALK_FORWARD,// しゃがむ歩く(前)
		CROUCH_LEFT,		// しゃがむ歩く(左)
		CROUCH_RIGHT,		// しゃがむ歩く(右)
		CROUCH_BACK,		// しゃがむ歩く(後)
		CROUCH_RELOAD,		// しゃがむリロード
		CROUCH_SHOT,		// しゃがむ撃つ

	};

	// プレイヤー衝突用部位
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
		EYE,	// 視線	
		RAY,	// 射線
		MAX,
	};


	// 移動スピードMAX
	static constexpr float WARK_SPEED_MAX = (350.0f);

	// 加速(accelerator=アクセレレーター)
	static constexpr float MOVE_ACC = (40.0f);

	//// 減速(decelerate=ディセラレイト)
	//static constexpr float MOVE_DEC = (0.2f);

	// 加速度
	static constexpr float ACCELERATION = (0.1f);

	// グレネードを投げる力
	static constexpr float GRENADE_POW = (20.0f);

	// 自動回復するまでの時間
	static constexpr float AUTO_HEAL_TIME = (7.0f);

	// 1秒間に回復するHP
	static constexpr float HEAL_HP = (20.0f);

	// HP上限
	static constexpr float MAX_HP = (100.0f);

	// リスポーンするまでの時間
	static constexpr float RESPAWN_COOL_TIME = (7.0f);

	// デスカメラに切り替えるまでの時間
	static constexpr float CHANGE_DEATHCAMERA_TIME = (2.0f);

	// 無敵時間
	static constexpr float INVINCIBLE_TIME = (7.0f);


	static constexpr int GRENADE_MAX = (5);

	// 足音を鳴らす間隔(MAXスピード時)
	static constexpr float SOUND_FOOT = (0.35f);

	Player(LOADOUT load0ut, VECTOR resPos, VECTOR resRot,
		int key = -1, ID id = ID::PLAYER_1, int team = -1, float moveControlPos = 0.0f, float actionControlPos = 0.0f);
	~Player(void);

	// 耐久値減少
	void Damage(float damage) override;

	// グレネードを増加
	void GetGrenade(GRENADE_TYPE type , int num = 1);

	// カメラ同期用Posを取得
	VECTOR GetSyncCameraPos(void)const;

	// カメラ同期用Rotを取得
	const VECTOR& GetSyncCameraRot(void)const;
	VECTOR* GetCameraDir(void);

	// 銃の座標を取得
	const VECTOR& GetGunPos(void)const;
	const Quaternion& GetGunRot(void)const;
	
	const bool& IsHide(void) const;

	// アニメーション終了後処理
	void AnimEnd(int id) override;

	// アニメーション通知
	void AnimNotice(int id)override;

	// 反動を加える
	void AddRecoilPower(float pow);

	// ADS中か
	const bool IsADS(void);
	const bool IsMe(void);
	const bool IsAct(void);

	//const VECTOR& GetCenterPos(void) const;
	VECTOR* GetHeadPos(void) const;
	VECTOR* GetCenterPos(void) const;
	const VECTOR& GetLatestTargetPos(void) const;

	const int& GetKey(void) const;

	//VECTOR GetBulletVec(void) const;
	int GetRandSeed(void) const;

	const int& GetTeam(void) const;
	const float& GetControlPosZ(void) const;

	void UpdateAfterCollision(void) override;

	std::vector<std::shared_ptr<GrenadeBase>>& GetGrenades(void);
	std::shared_ptr<GunBase> GetGun(void);

	// 衝突したときの処理
	void OnHit(std::weak_ptr<Collider> obj) override;
	void OnHit(std::weak_ptr<Collider> objA, std::weak_ptr<Collider> objB) override;
	void OnHit(MV1_COLL_RESULT_POLY info) override;
	void OnHit(HITRESULT_LINE info) override;
	void OnHit(VECTOR normal) override;

protected:

	int key_;

	// デフォルトハンドル
	int modelGrHandle_;
	int modelGrHandle2_;

	// 強調用ハンドル
	int colorGrHandle_;

	// インプットコントローラ
	PlayerController& controller_;
	GameManager& gameManager_;

	// アニメーターコントローラ
	std::unique_ptr<AnimatorController> animCon_;
	std::shared_ptr<Model> animModels_[(int)ACTION::CROUCH_SHOT];

	// 衝突判定用
	std::shared_ptr<Collider> colliders_[(int)BODY::MAX];

	// 銃
	std::shared_ptr<GunBase> gun_;

	// グレネード
	std::unordered_map<GRENADE_TYPE,int> greNum_;

	// グレネード本体
	std::vector<std::shared_ptr<GrenadeBase>> gres_;

	LOADOUT loadOut_;

	// 今手に持っているグレネード
	std::weak_ptr<GrenadeBase> greInHand_;

	// 反動
	std::unique_ptr<Recoil> recoil_;

	// カメラ同期用オイラー
	VECTOR cameraRot_;
	VECTOR cameraDir_;

	// 腰オイラー
	VECTOR waistRot_;

	// リスポーン位置回転
	VECTOR respawnPos_;
	Quaternion respawnQuaRot_;

	// 現在のアクション
	ACTION act_;
	ACTION preAct_;
	ACTION actUp_;
	ACTION preActUp_;

	// 移動方向
	VECTOR moveDir_;
	VECTOR preMoveDir_;

	// アクション中かどうか(移動以外)
	bool isAct_;

	// 移動速度
	float moveSpeed_;

	// ジャンプ中かどうか
	bool isJump_;

	// ハイド中かどうか
	bool isHide_;

	bool isPutShotPre_;

	// 銃の状態
	GunBase::STATE gunState_;

	// 現在選択している武器
	GUN_TYPE gunType_;

	// 現在選択しているグレネード
	GRENADE_TYPE greType_;

	// 自動回復カウント
	Count autoHeal_;

	// リスポーンカウント
	Count respawn_;

	// デスカメラ切り替え
	Count deathCamera_;

	// 無敵時間
	Count invincibleTime_;

	// 現在のチーム
	int team_;

	// 移動制御座標
	float moveControlPosZ_;
	float actionControlPosZ_;

	// 足音間隔
	float stepFootSound_;

	virtual void Init_Actor(void) override;
	virtual void Update_Actor(void) override;
	virtual void Draw_Actor(void) override;
	virtual void Release_Actor(void) override;

	// 半透明描画
	void DrawTrans(void) override;

	// アクション変更
	void ChangeAction(ACTION act);
	void ChangeActionUp(ACTION actUp);

	// 銃状態変更
	void ChangeGunState(GunBase::STATE state);

	// グレネード変更 
	void ChangeNextGrenade(void);

	// アニメーション設定
	void AnimationSetting(void);

	// コライダー設定
	void ColliderSetting(void);

	// 死亡時処理
	void ProcessDead(void) override;

	// リスポーン処理
	void Respawn(void);

	// 自動回復
	void AutoHeal(void);

	// 死亡中処理
	void DeadUpdate(void);

	// 操作
	virtual void ProcessAction(void);

	// 移動操作
	virtual void ProcessMove(void);

	// 加速(スピードを加える)
	virtual void Accele(float speed);

	// 減速(スピードを減少させる)
	virtual void Decelerate(void);

	// しゃがみ操作
	virtual void ProcessCrouch(void);

	// 回転操作(視点操作)
	virtual void ProcessRotate(void);

	// ジャンプ操作
	virtual void ProcessJump(void);
	
	// 撃つ操作
	virtual void ProcessShot(void);

	// ADS操作
	virtual void ProcessADS(void);

	// リロード操作
	virtual void ProcessReload(void);

	// グレネード操作
	virtual void ProcessGrenade(void);

	// コライダー座標更新
	virtual void UpdateCollider(void);

	// 反動処理
	virtual void UpdateRecoil(void);

	// グレネードを使う(使えたらtrueを返す)
	virtual bool UseGrenade(GRENADE_TYPE type);

	// 頭を見えなくする
	void HideHead(void);
	void DisplayHead(void);

	// 座標回転など更新
	virtual void TransformUpdate(void);

	VECTOR debugPos;

	// 加害者(最後に攻撃してきたオブジェクト)
	std::weak_ptr<ObjectBase> lastAtkObj_;

	int test_ = 0;

private:

};