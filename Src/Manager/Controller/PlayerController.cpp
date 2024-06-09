#include <algorithm>
#include "../../Application.h"
#include "../../Utility/Utility.h"
#include "../SceneManager.h"
#include "../Camera/GameCamera.h"
#include "PlayerController.h"

using namespace std;

PlayerController::PlayerController(int key)
	: inputManager_(InputManager::GetInstance()), netManager_(NetManager::GetInstance()),gameManager_(GameManager::GetInstance())
{
	camera_ = dynamic_pointer_cast<GameCamera>(SceneManager::GetInstance().GetCamera());

	key_ = key;
	rotPow_ = {};
	isMoveF_ = false;
	isMoveB_ = false;
	isMoveL_ = false;
	isMoveR_ = false;
	isDash_ = false;
	isCrouch_ = false;
	isRotate_ = true;
	isJump_ = false;
	isPutShot_ = false;
	isADS_ = false;
	isReload_ = false;
	isGrenade_ = false;
	isGrenadeChange_ = false;
	isGrenadeInitiation_ = false;
}

PlayerController::~PlayerController(void)
{
}

void PlayerController::Init(void)
{
}

void PlayerController::Update(void)
{
	if (key_ != netManager_.GetSelf().key)return;

	AuditShot();
	AuditRotate();
	AuditMove();
	AuditGrenade();

	randSeed_ = GetRand(999999);

	//cameraInfo_.pos = camera_.lock()->GetPos();
	//cameraInfo_.targetPos = camera_.lock()->GetTargetPos();
	//cameraInfo_.rot = camera_.lock()->GetRot().ToEuler();

	NetSetAction();
}

void PlayerController::NetSetAction(void)
{
	if (key_ == -1 || key_ != netManager_.GetSelf().key) return;

	netManager_.SetRandSeed(randSeed_);
	//netManager_.SetCameraInfo(cameraInfo_);

	if (isPutShot_) {
		netManager_.SetAction(PLAYER_ACTION::SHOT);
		//netManager_.SetBulletDir(bulletVec_);
	}
	if (isADS_) {
		netManager_.SetAction(PLAYER_ACTION::ADS);
	}
	if (isReload_) {
		netManager_.SetAction(PLAYER_ACTION::RELOAD);
	}
	if (isMoveF_) {
		netManager_.SetAction(PLAYER_ACTION::MOVE_FORWARD);
	}
	if (isMoveB_) {
		netManager_.SetAction(PLAYER_ACTION::MOVE_BACK);
	}
	if (isMoveL_) {
		netManager_.SetAction(PLAYER_ACTION::MOVE_LEFT);
	}
	if (isMoveR_) {
		netManager_.SetAction(PLAYER_ACTION::MOVE_RIGHT);
	}
	if (isDash_) {
		netManager_.SetAction(PLAYER_ACTION::MOVE_RUN);
	}
	if (isJump_) {
		netManager_.SetAction(PLAYER_ACTION::JUMP);
	}
	if (isCrouch_) {
		netManager_.SetAction(PLAYER_ACTION::CROUCH);
	}
	if (isRotate_) {
		netManager_.SetAction(PLAYER_ACTION::ROTATE);
		netManager_.SetRotPow(rotPow_);
	}
	if (isGrenade_) {
		netManager_.SetAction(PLAYER_ACTION::GRENADE);
	}
	if (isGrenadeChange_) {
		netManager_.SetAction(PLAYER_ACTION::GRENADE_CHANGE);
	}
	if (isGrenadeInitiation_) {
		netManager_.SetAction(PLAYER_ACTION::GRENADE_INITIATION);
	}
}

void PlayerController::NetUpdate(void)
{
	if (key_ == -1) return;

	randSeed_ = netManager_.GetRandSeed(key_, randSeed_);
	//cameraInfo_ = netManager_.GetCameraInfo(key_, cameraInfo_);

	NetAuditShot();
	NetAuditRotate();
	NetAuditMove();
	NetAuditGrenade();

	isMe_ = (key_ == netManager_.GetSelf().key);
}

//void InputController::SetBulletRandLength(float length)
//{
//	randLength_ = length;
//}
//
//void InputController::SetBulletRandRadius(float radius)
//{
//	randRadius_ = radius;
//}
//
//void InputController::SetGunForward(VECTOR dir)
//{
//	gunForward_ = dir;
//}
//
//void InputController::SetBarrelPos(VECTOR pos)
//{
//	barrelPos_ = pos;
//}

bool PlayerController::IsPutShot(void) const
{
	return isPutShot_;
}

bool PlayerController::IsReload(void) const
{
	return isReload_;
}

bool PlayerController::IsADS(void) const
{
	return isADS_;
}

bool PlayerController::IsRotate(void) const
{
	return isRotate_;
}

bool PlayerController::IsGrenade(void) const
{
	return isGrenade_;
}

bool PlayerController::IsGrenadeChange(void) const
{
	return isGrenadeChange_;
}

bool PlayerController::IsGrenadeInitiation(void) const
{
	return isGrenadeInitiation_;
}

bool PlayerController::IsMe(void) const
{
	return isMe_;
}

//const VECTOR InputController::GetBulletVec(void) const
//{
//	return bulletVec_;
//}

bool PlayerController::IsMoveF(void) const
{
	return isMoveF_;
}

bool PlayerController::IsMoveB(void) const
{
	return isMoveB_;
}

bool PlayerController::IsMoveL(void) const
{
	return isMoveL_;
}

bool PlayerController::IsMoveR(void) const
{
	return isMoveR_;
}

bool PlayerController::IsPutDash(void) const
{
	return isDash_;
}

bool PlayerController::IsPutCrouch(void) const
{
	return isCrouch_;
}

bool PlayerController::IsPutJump(void) const
{
	return isJump_;
}

//const VECTOR& InputController::GetMoveDir(void) const
//{
//	return moveDir_;
//}

const Vector2<float>& PlayerController::GetRotatePow(void) const
{
	return rotPow_;
}

//const float& InputController::GetMoveSpeed(void) const
//{
//	return moveSpeed_;
//}
//
//const VECTOR& InputController::GetTargetPos(VECTOR targetPos)
//{
//	//targetPos_ = targetPos;
//
//	//if (key_ != -1) {
//	//	targetPos_ = netManager_.GetTargetPos(key_, targetPos_);
//	//}
//
//	//if (isRotate_ && key_ == netManager_.GetSelf().key) {
//	//	netManager_.SetTargetPos(targetPos_);
//	//}
//
//	return targetPos_;
//}

const int& PlayerController::GetRandSeed(void) const
{
	return randSeed_;
}

//const CAMERA_INFO& PlayerController::GetCameraInfo(void) const
//{
//	return cameraInfo_;
//}

void PlayerController::AuditShot(void)
{
	isPutShot_ = inputManager_.IsClickMouseLeft();

	isADS_ = inputManager_.IsClickMouseRight();

	isReload_ = inputManager_.IsTrgDown(KEY_INPUT_R);
	
	if (key_ != -1 && key_ != netManager_.GetSelf().key)return;



	// テスト
	//bulletVec_ = VNorm(VSub(eP, sP));

}

void PlayerController::AuditRotate(void)
{
	// マウス回転量
	float dpi = gameManager_.GetDPI();
	if (isADS_)dpi /= 2.0f;

	// マウス位置
	Vector2<int> mousePos;

	// 画面の中心位置
	Vector2 center = { Application::SCREEN_SIZE_X / 2,Application::SCREEN_SIZE_Y / 2 };

	// マウス位置の取得
	GetMousePoint(&mousePos.x, &mousePos.y);

	// カメラ回転の計算(マウスカーソル位置と画面の中心の差分を計算し、回転量/FPSを乗算する)
	// これが回転量
	//float rotPowY = float(clamp(mousePos.x - center.x, -120, 120)) * dpi / GetFPS();	// マウス横移動
	//float rotPowX = float(clamp(mousePos.y - center.y, -120, 120)) * dpi / GetFPS();	// マウス縦移動
	float rotPowY = float(clamp(mousePos.x - center.x, -120, 120)) * dpi / 60.0f;	// マウス横移動
	float rotPowX = float(clamp(mousePos.y - center.y, -120, 120)) * dpi / 60.0f;	// マウス縦移動
	//float rotPowY = 0.0f;
	//float rotPowX = 0.0f;

	//if (CheckHitKey(KEY_INPUT_UP)) 
	//{
	//	rotPowX = 10.0f * DX_PI_F / 180.0f;
	//}
	//if (CheckHitKey(KEY_INPUT_DOWN))
	//{
	//	rotPowX = -10.0f * DX_PI_F / 180.0f;
	//}

	//if (CheckHitKey(KEY_INPUT_LEFT))
	//{
	//	rotPowY = -10.0f * DX_PI_F / 180.0f;
	//}
	//if (CheckHitKey(KEY_INPUT_RIGHT))
	//{
	//	rotPowY = 10.0f * DX_PI_F / 180.0f;
	//}

	
	//
	//float x = mousePos.x - center.x;
	//if (x == 0.0f)
	//{
	//}
	//else if (x > 5.0f)
	//{
	//	rotPowY = 10.0f * DX_PI_F / 180.0f;
	//}
	//else if (x < -5.0f)
	//{
	//	rotPowY = -10.0f * DX_PI_F / 180.0f;
	//}

	//float y = mousePos.y - center.y;
	//if (y == 0.0f)
	//{
	//}
	//else if (y > 0.0f)
	//{
	//	rotPowX = -10.0f * DX_PI_F / 180.0f;
	//}
	//else
	//{
	//	rotPowX = 10.0f * DX_PI_F / 180.0f;
	//}


	rotPow_ = { rotPowX ,rotPowY };

	isRotate_ = (rotPow_.x != 0.0f || rotPow_.y);


}

void PlayerController::AuditMove(void)
{
	isMoveF_ = inputManager_.IsNew(KEY_INPUT_W);
	isMoveB_ = inputManager_.IsNew(KEY_INPUT_S);
	isMoveL_ = inputManager_.IsNew(KEY_INPUT_A);
	isMoveR_ = inputManager_.IsNew(KEY_INPUT_D);
	isDash_ = inputManager_.IsNew(KEY_INPUT_LSHIFT);
	isJump_ = inputManager_.IsTrgDown(KEY_INPUT_SPACE);
	isCrouch_ = inputManager_.IsNew(KEY_INPUT_LCONTROL);
}

void PlayerController::AuditGrenade(void)
{
	isGrenade_ = false;
	isGrenadeChange_ = false;

	if (inputManager_.IsTrgUp(KEY_INPUT_G)) {
		if (60 > pushGCnt_ && pushGCnt_ > 0) {
			isGrenade_ = true;
		}
		pushGCnt_ = 0;
	}

	if (inputManager_.IsNew(KEY_INPUT_G)) {
		pushGCnt_++;
		if (pushGCnt_ % 60 >= 60 - 1) {
			isGrenadeChange_ = true;
		}
	}
	else {
		pushGCnt_ = 0;
	}

	isGrenadeInitiation_ = inputManager_.IsNew(KEY_INPUT_B);
}

void PlayerController::NetAuditShot(void)
{
	isPutShot_ = netManager_.IsAction(key_, PLAYER_ACTION::SHOT, isPutShot_);
	isADS_ = netManager_.IsAction(key_, PLAYER_ACTION::ADS, isADS_);
	isReload_ = netManager_.IsAction(key_, PLAYER_ACTION::RELOAD, isReload_);
	//bulletVec_ = netManager_.GetBulletDir(key_, bulletVec_);
}

void PlayerController::NetAuditRotate(void)
{
	isRotate_ = netManager_.IsAction(key_, PLAYER_ACTION::ROTATE, isRotate_);
	rotPow_ = netManager_.GetRotatePow(key_, rotPow_);
}

void PlayerController::NetAuditMove(void)
{
	isMoveF_ = netManager_.IsAction(key_, PLAYER_ACTION::MOVE_FORWARD, isMoveF_);
	isMoveB_ = netManager_.IsAction(key_, PLAYER_ACTION::MOVE_BACK, isMoveB_);
	isMoveL_ = netManager_.IsAction(key_, PLAYER_ACTION::MOVE_LEFT, isMoveL_);
	isMoveR_ = netManager_.IsAction(key_, PLAYER_ACTION::MOVE_RIGHT, isMoveR_);
	isDash_ = netManager_.IsAction(key_, PLAYER_ACTION::MOVE_RUN, isDash_);
	isJump_ = netManager_.IsAction(key_, PLAYER_ACTION::JUMP, isJump_);
	isCrouch_ = netManager_.IsAction(key_, PLAYER_ACTION::CROUCH, isCrouch_);
}

void PlayerController::NetAuditGrenade(void)
{
	isGrenade_ = netManager_.IsAction(key_, PLAYER_ACTION::GRENADE, isGrenade_);
	isGrenadeChange_ = netManager_.IsAction(key_, PLAYER_ACTION::GRENADE_CHANGE, isGrenadeChange_);
	isGrenadeInitiation_ = netManager_.IsAction(key_, PLAYER_ACTION::GRENADE_INITIATION, isGrenadeInitiation_);
}
