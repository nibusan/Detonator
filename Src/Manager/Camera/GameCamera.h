#pragma once
#include "CameraBase.h"
#include "../../Net/NetManager.h"

class GameCamera : public CameraBase
{
public:

	static constexpr int MAX_PLAYER = (4);

	// カメラスピード：NEAR
	static constexpr float SPEED = 20.0f;

	// 補完時間
	static constexpr float LERP_TIME_DEF = (1.0f);
	
	// カメラの初期座標
	static constexpr VECTOR DEFAULT_CAMERA_POS = { 0.0f,20.0f,13.0f };

	// カメラ位置から注視点までの相対座標
	static constexpr VECTOR RELATIVE_C2T_POS = { 0.0f, 0.0f, 1000.0f };
	
	// FPS時カメラ位置から注視点までの相対座標
	static constexpr VECTOR RELATIVE_POS_FPS = { 0.0f, 0.0f, 1000.0f };

	// ADS時カメラ位置から注視点までの相対座標
	static constexpr VECTOR RELATIVE_POS_ADS = { 0.0f, 0.0f, 1000.0f };

	// DEATH時カメラ位置から注視点までの相対座標
	static constexpr VECTOR RELATIVE_POS_DEATH = { 0.0f, 150.0f, -300.0f };

	// カメラFOV
	static constexpr float FOV_DEFAULT = (1.0f);

	// カメラシェイク時に基準となる距離
	// 追従対象と震源地との距離がこの値よりも
	// 大きくなるほどShakePowは小さく
	// 小さくなるほどShakePowは大きくなる
	// つまり、爆心地に近いほど揺れが大きくなる
	static constexpr float SHAKE_DIS = (100.0f);

	// カメラモード
	enum class MODE
	{
		NONE,
		FPS,			// 一人称モード
		ADS,			// ADS(照準)
		DEATH,			// デスカメラ
		FIXED_POINT,	// 定点カメラ
		DEBUG,			// デバッグ用カメラ
	};

	struct MultiCameraInfo 
	{
		std::weak_ptr<ObjectBase> follow_;
		MODE mode_ = MODE::NONE;
		VECTOR pos_ = {};
		Quaternion rot_ = {};
		VECTOR targetPos_ = {};
		VECTOR gPos_ = {};
		Quaternion gRot_ = {};
		VECTOR gTargetPos_ = {};
		// シェイク
		float shakeTime_ = 0.0f;
		float shakePow_ = 0.0f;
		// 遷移時間
		float lerpTime_ = 0.0f;
		float cntLerpTime_ = 0.0f;
	};

	GameCamera(void);
	~GameCamera(void);

	void Init(void) override;
	void Update(void);
	void NetUpdate(void) override;

	// カメラ処理
	void SetBeforeDraw(void) override;
	void SetBeforeDrawFPS(void);
	void SetBeforeDrawADS(void);
	void SetBeforeDrawDeath(void);
	void SetBeforeDrawFixedPoint(void);
	void SetBeforeDrawDebug(void);

	void SetBeforeDrawMulti(void);
	void SetBeforeDrawFPSMulti(int key);
	void SetBeforeDrawADSMulti(int key);
	void SetBeforeDrawDeathMulti(int key);
	void SetBeforeDrawFixedPointMulti(int key);

	// シェイク
	void UpdateSheke(void);
	void UpdateShekeMulti(void);
	void Shake(float time, float pow, VECTOR pos);
	void ShakeMulti(int key, float time, float pow, VECTOR pos);

	void Draw(void);
	void Release(void);

	VECTOR GetTargetPos(void) const;
	const MODE& GetMode(void)const;
	const MODE& GetMode(int key)const;

	VECTOR GetCameraDir(void) const;

	// カメラモードの変更(補完させたい場合値を渡す必要がある)
	void ChangeMode(MODE mode, float time = 0.0f);
	void ChangeMode(MODE mode, std::weak_ptr<ObjectBase> follow, float time = 0.0f , float fav = FOV_DEFAULT);
	void ChangeMode(MODE mode, VECTOR fixedPos, VECTOR fixedRot, float time = 0.0f);

	void ChangeMode(int key, MODE mode, float time = 0.0f);
	void ChangeMode(int key, MODE mode, std::weak_ptr<ObjectBase> follow, float time = 0.0f);

	// 追従対象の設定
	void SetFollowTarget(std::weak_ptr<ObjectBase> follow);
	void SetFollowTarget(int key, std::weak_ptr<ObjectBase> follow);

	// 更新後のカメラ情報関連ーーーーーーーーーーーーーーーーーーーーーーー

	VECTOR GetLatestPos(int key);
	VECTOR GetLatestTargetPos(int key);

	// カメラ処理
	void SetBeforeDrawLatest(int key, VECTOR& pos, Quaternion& rot, VECTOR& targetPos);
	void SetBeforeDrawLatestFPS(int key, VECTOR& gPos, Quaternion& gRot, VECTOR& gTargetPos);
	void SetBeforeDrawLatestADS(int key, VECTOR& gPos, Quaternion& gRot, VECTOR& gTargetPos);
	void SetBeforeDrawLatestDeath(int key, VECTOR& gPos, Quaternion& gRot, VECTOR& gTargetPos);
	void SetBeforeDrawLatestFixedPoint(int key, VECTOR& gPos, Quaternion& gRot, VECTOR& gTargetPos);
	void SetBeforeDrawLatestDebug(int key, VECTOR& gPos, Quaternion& gRot, VECTOR& gTargetPos);

	// 補完
	void GotoGoalLatest(int key, VECTOR& pos, Quaternion& rot, VECTOR& targetPos,
		VECTOR& gPos, Quaternion& gRot, VECTOR& gTargetPos);

	void UpdateShekeLatest(int key, VECTOR& pos);

	// ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー


	const MultiCameraInfo& GetCameraInfo(int no) const;

private:

	NetManager& netManager_;
	MultiCameraInfo multiInfo_[MAX_PLAYER];

	int playerNum_;
	int key_;

	// 追従
	std::weak_ptr<ObjectBase> follow_;
	//std::weak_ptr<ObjectBase> follows_[MAX_PLAYER];

	// カメラモード
	MODE mode_;
	// MODE modes_[MAX_PLAYER];

	// カメラFOV
	float fov_;

	// シェイク
	float shakeTime_;
	float shakePow_;


	// ゴールーーーーーーーーーーーーーー

	// カメラの位置(目標)
	VECTOR gPos_;

	// カメラの注視点(目標)
	VECTOR gTargetPos_;

	// カメラの上方向(目標)
	VECTOR gCameraUp_;

	// カメラの回転(目標)
	Quaternion gRot_;

	// カメラの角度(rad)(目標)
	VECTOR gAngles_;

	// カメラスケール(目標)
	float gFov_;

	// 補完用
	float lerpTime_;
	float cntLerpTime_;


	// カメラ移動回転(目標値へ)
	void GotoGoal(void);
	void GotoGoalMulti(void);

	// カメラを初期位置に戻す
	void SetDefault(void);
};

