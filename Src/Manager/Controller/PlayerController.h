#pragma once
//#include "../Common/Vector2.h"
#include "../InputManager.h"
#include "../SceneManager.h"
#include "../GameManager.h"
#include "../../Net/NetManager.h"

class Quaternion;
class GameCamera;

using PAD_NO = InputManager::JOYPAD_NO;
using PAD_BTN = InputManager::JOYPAD_BTN;
using PAD_STATE = InputManager::JOYPAD_IN_STATE;


class PlayerController
{

public:

	PlayerController(int key = -1);
	~PlayerController(void);

	void Init(void);
	void Update(void);
	void NetSetAction(void);
	void NetUpdate(void);

	//void SetBulletRandLength(float length);
	//void SetBulletRandRadius(float radius);
	//void SetGunForward(VECTOR dir);
	//void SetBarrelPos(VECTOR pos);


	bool IsMoveF(void) const;
	bool IsMoveB(void) const;
	bool IsMoveL(void) const;
	bool IsMoveR(void) const;
	bool IsPutDash(void) const;
	bool IsPutCrouch(void) const;
	bool IsPutJump(void) const;
	bool IsPutShot(void) const;
	bool IsReload(void) const;
	bool IsADS(void) const;
	bool IsRotate(void) const;
	bool IsGrenade(void) const;
	bool IsGrenadeChange(void) const;
	bool IsGrenadeInitiation(void) const;

	bool IsMe(void) const;

	//const VECTOR GetBulletVec(void) const;

	//const VECTOR& GetMoveDir(void) const;
	const Vector2<float>& GetRotatePow(void) const;
	//const float& GetMoveSpeed(void) const;

	//const VECTOR& GetTargetPos(VECTOR targetPos);

	const int& GetRandSeed(void) const;
	//const CAMERA_INFO& GetCameraInfo(void) const;
	//const double& GetMoveRotate(void) const;

private:

	int key_;

	InputManager& inputManager_;
	GameManager& gameManager_;
	NetManager& netManager_;
	std::weak_ptr<GameCamera> camera_;
	//const CameraBase& camera_;

	Vector2<float> rotPow_;
	//VECTOR targetPos_;
	//float moveSpeed_;
	
	bool isMoveF_;
	bool isMoveB_;
	bool isMoveL_;
	bool isMoveR_;
	bool isDash_;
	bool isCrouch_;
	bool isRotate_;
	bool isJump_;
	bool isPutShot_;
	bool isADS_;
	bool isReload_;
	bool isGrenade_;
	bool isGrenadeChange_;
	bool isGrenadeInitiation_;

	bool isMe_;

	// Gキーを連続で押していたフレーム数
	int pushGCnt_;

	// 乱数の種
	int randSeed_;

	// カメラ情報
	//CAMERA_INFO cameraInfo_;

	//// 弾方向
	//VECTOR bulletVec_;
	//VECTOR barrelPos_;
	//VECTOR gunForward_;
	//float randLength_;
	//float randRadius_;

	//VECTOR moveDir_;
	//double moveRotate_;

	void AuditShot(void);
	void AuditRotate(void);
	void AuditMove(void);
	void AuditGrenade(void);

	void NetAuditShot(void);
	void NetAuditRotate(void);
	void NetAuditMove(void);
	void NetAuditGrenade(void);
};
