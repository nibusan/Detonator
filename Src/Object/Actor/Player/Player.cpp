#include "../../../Manager/InputManager.h"
#include "../../../Manager/Controller/PlayerController.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/CollisionManager.h"
#include "../../../Manager/EffectManager.h"
#include "../../../Manager/AudioManager.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Application.h"
#include "../../../Manager/Camera/GameCamera.h"
#include "../../../Utility/Utility.h"
#include "../../ObjectBase.h"
#include "../ActorBase.h"
#include "../../Gun/GunAK47.h"
#include "../../Gun/GunM4.h"
#include "../../Gun/GunM24R.h"
#include "../../Gun/GunAWM.h"
#include "../../Gun/GunBase.h"
#include "../AnimatorController.h"
#include "../../Common/ColliderCapsule.h"
#include "../../Common/ColliderSphere.h"
#include "../../Common/ColliderLine.h"
#include "../../Common/Recoil.h"
//#include "../../Grenade/GrenadeBase.h"
#include "../../Grenade/GrenadeHand.h"
#include "../../Grenade/GrenadeSmoke.h"
#include "../../Grenade/GrenadeSticky.h"
#include "Player.h"

using namespace std;

Player::Player(LOADOUT load0ut, VECTOR resPos, VECTOR resRot, int key, ID id, int team, float moveControlPos, float actionControlPos) :
	controller_(InputManager::GetInstance().GetController((int)id)),
	gameManager_(GameManager::GetInstance())
{
	key_ = key;

	// ネームID
	ID_ = id;

	// 初期化
	gunType_ = load0ut.gunType_;

	// グレネード
	for (int i = 0; i < (int)GRENADE_TYPE::MAX; i++) {
		greNum_.emplace((GRENADE_TYPE)i, 0);
	}

	loadOut_ = load0ut;
	GetGrenade(load0ut.greType_[0], 5);
	GetGrenade(load0ut.greType_[1], 5);
	if (loadOut_.greType_[0] == loadOut_.greType_[1]) {
		auto& gn = greNum_.at(loadOut_.greType_[0]);
		gn *= 2;
	}
	greType_ = load0ut.greType_[0];

	respawnPos_ = resPos;
	respawnQuaRot_ = Quaternion::Euler(resRot);

	isHide_ = false;

	team_ = team;
	moveControlPosZ_ = moveControlPos;
	actionControlPosZ_ = actionControlPos;

	isPutShotPre_ = controller_.IsPutShot();


}

Player::~Player(void)
{

}

void Player::Damage(float damage)
{

	if (hp_ <= 0.0f) return;

	hp_ -= damage;
	if (hp_ <= 0.0f)
	{
		hp_ = 0.0f;
		ProcessDead();
	}

	autoHeal_.Set();

}

void Player::GetGrenade(GRENADE_TYPE type, int num)
{
	// 所持数を増やす
	auto& gn = greNum_.at(type);
	gn += num;
	if (gn >= GRENADE_MAX)gn = GRENADE_MAX;
}

VECTOR Player::GetSyncCameraPos(void) const
{
	// 対象フレームのローカル行列(大きさ、回転、位置)を取得する
	auto mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, 5);
	auto pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す

	return pos;
}

const VECTOR& Player::GetSyncCameraRot(void) const
{
	return cameraRot_;
}

VECTOR* Player::GetCameraDir(void)
{
	return &cameraDir_;
}

const VECTOR& Player::GetGunPos(void) const
{
	//auto fMat = MV1GetFrameLocalMatrix(transform_.modelId, 35);
	//auto fPos = MGetTranslateElem(fMat);	// 行列から移動値を取り出す

	//VECTOR ret = VAdd(fPos, transform_.quaRot.PosAxis({ 5.0f,10.0f,20.0f }));

	return gun_->GetScopePos();
}

const Quaternion& Player::GetGunRot(void) const
{
	auto t = gun_->GetTransform();
	Quaternion ret = t.quaRot;

	return ret;
}

const bool& Player::IsHide(void) const
{
	return isHide_;
}

void Player::AnimEnd(int id)
{
	if (id == (int)ACTION::GRENADE)
	{
		animCon_->SetIsStop((int)ACTION::GRENADE, true);
		isAct_ = false;
	}
	else if (id == (int)ACTION::RELOAD)
	{
		isAct_ = false;
	}
}

void Player::AnimNotice(int id)
{
	if (id == (int)ACTION::GRENADE)
	{
		// 投げる方向(向いている方向のちょっと上)
		auto rot = cameraRot_;
		rot.x += Utility::Deg2RadF(-30.0f);
		Quaternion q = Quaternion::Euler(rot);

		greInHand_.lock()->Throw(q.GetForward(), GRENADE_POW);

		AudioManager::GetInstance().Play(
			AudioList::AUDIO_TYPE::PLAYER_THROW,
			transform_.pos,
			2500.0f,
			false,
			0.8f
		);

		gameManager_.AddBom((int)ID_);
	}
	else if (id == (int)ACTION::DEAD)
	{
		animCon_->SetIsStop((int)ACTION::DEAD, true);
	}
	else if (id == (int)ACTION::RELOAD)
	{
		isAct_ = false;
		ChangeGunState(GunBase::STATE::NONE);

		gun_->PlayReloadSound();

		// リロード
		gun_->ReloadGun();
	}
}

void Player::AddRecoilPower(float pow)
{
	// 反動
	recoil_->AddPower(pow);
}

const bool Player::IsADS(void)
{
	return controller_.IsADS();
}

const bool Player::IsMe(void)
{
	return controller_.IsMe();
}

const bool Player::IsAct(void)
{
	return isAct_;
}

VECTOR* Player::GetHeadPos(void) const
{
	return &colliders_[(int)BODY::HEAD]->pos;
}

VECTOR* Player::GetCenterPos(void) const
{
	return &colliders_[(int)BODY::CHEST]->pos;
}

const VECTOR& Player::GetLatestTargetPos(void) const
{
	auto camera = std::dynamic_pointer_cast<GameCamera>(SceneManager::GetInstance().GetCamera());
	return camera->GetLatestTargetPos((int)ID_);
}

const int& Player::GetKey(void) const
{
	return key_;
}

int Player::GetRandSeed(void) const
{
	return controller_.GetRandSeed();
}

const int& Player::GetTeam(void) const
{
	return team_;
}

const float& Player::GetControlPosZ(void) const
{
	return moveControlPosZ_;
}

void Player::UpdateAfterCollision(void)
{
	gun_->UpdateAfterCollision();
}

std::vector<std::shared_ptr<GrenadeBase>>& Player::GetGrenades(void)
{
	return gres_;
}

std::shared_ptr<GunBase> Player::GetGun(void)
{
	return gun_;
}

void Player::OnHit(std::weak_ptr<Collider> obj)
{

}

void Player::OnHit(std::weak_ptr<Collider> objA, std::weak_ptr<Collider> objB)
{

	float rate = 1.0f;

	switch (objA.lock()->objType)
	{
	case Collider::OBJECT_TYPE::PLAYER_HEAD:
		rate = 1.5f;
		if (objB.lock()->objType == Collider::OBJECT_TYPE::PLAYER_BULLET){
			gameManager_.AddHeadShot((int)objB.lock()->parent_.lock()->GetID());
		}
		break;
	case Collider::OBJECT_TYPE::PLAYER_BODY:
		rate = 1.0f;
		break;
	case Collider::OBJECT_TYPE::PLAYER_LIMBS:
		rate = 0.75f;
		break;
	case Collider::OBJECT_TYPE::PLAYER_CAP:
		rate = 1.0f;
		break;
	default:
		return;
		break;
	}

	float damage = 0.0f;

	if (objB.lock()->objType == Collider::OBJECT_TYPE::PLAYER_BULLET)
	{
		auto bullet = dynamic_pointer_cast<BulletBase>(objB.lock()->parent_.lock());

		auto player = dynamic_pointer_cast<Player>(bullet->GetParent().lock());
		if (player->GetTeam() == team_) return;



		damage = bullet->GetDamage() * rate;

		lastAtkID_ = bullet->GetID();

		// 加害者(最後に攻撃してきたオブジェクト)
		lastAtkObj_ = bullet->GetParent();
	}
	else if (objB.lock()->objType == Collider::OBJECT_TYPE::ENEMY_BULLET)
	{
		auto bullet = dynamic_pointer_cast<BulletBase>(objB.lock()->parent_.lock());

		damage = bullet->GetDamage() * rate;

		lastAtkID_ = bullet->GetID();

		// 加害者(最後に攻撃してきたオブジェクト)
		lastAtkObj_ = bullet->GetParent();

		//AudioManager::GetInstance().Play(AudioList::AUDIO_TYPE::ENEMY_DAMAGE,
		//	transform_.pos, transform_.rot, *GetHeadPos(), 5500.0f);
	}
	else if (objB.lock()->objType == Collider::OBJECT_TYPE::EXPLOSION)
	{
		auto gre = std::dynamic_pointer_cast<GrenadeBase>(objB.lock()->parent_.lock());
		auto type = gre->GetType();

		// 距離によってダメージ量計算
		auto s = std::dynamic_pointer_cast<ColliderSphere>(objB.lock());
		float max = s->GetRadius();
		float dis = Utility::Distance(objA.lock()->pos, objB.lock()->pos);
		rate = ((max - dis) / max);
		if (rate < 0.15f) rate = 0.15f;

		damage = gre->GetDamage() * rate;

		lastAtkID_ = gre->GetID();

		// 加害者(最後に攻撃してきたオブジェクト)
		lastAtkObj_ = gre->GetParent();
		rate = 1.0f;
	}
	else if (objB.lock()->objType == Collider::OBJECT_TYPE::SMOKE) {
		// ハイド状態にする
		isHide_ = true;
		return;
	}

	if (invincibleTime_.IsCount()) {
		damage = 0.0f;
	}
	
	Damage(damage);
	auto id = objB.lock()->parent_.lock()->GetID();
	if ((int)NetManager::GetInstance().GetSelf().playerType == (int)id) {
		GameManager::DamageData data;
		data.damage_ = damage;
		data.pos = objA.lock()->pos;
		data.pos.x += (-10 + GetRand(20));
		data.pos.y += (-10 + GetRand(20));
		data.time_ = 1.0f;
		if (rate == 1.5f)data.color_ = { 255,255,0 };
		else data.color_ = { 255,0,0 };
		gameManager_.AddDamageData(data);
	}
}

void Player::OnHit(MV1_COLL_RESULT_POLY info)
{
	// 上方向に押し戻されたならジャンプや重力をリセットする
	if (info.Normal.y == 1.0f)
	{
		if (transform_.IsRising()) return;
		
		// プレイヤーをステージ上に移動
		//transform_.pos = info.HitPosition;
		transform_.pos.y = info.Position[0].y;

		// 重力加速度をリセット
		transform_.ResetGravityAccel();


		if (isJump_)
		{
			AnimEnd(0);

			AudioManager::GetInstance().Play(
				AudioList::AUDIO_TYPE::PLAYER_JUMP_LANDING,
				transform_.pos,
				2500.0f,
				false,
				1.0f
			);
		}

		// ジャンプ中かどうか
		isJump_ = false;
	}

	// 下方向に押し戻されたならジャンプ力をリセットする
	if (info.Normal.y == -1.0f)
	{
		if (transform_.IsFalling()) return;

		// プレイヤーをステージ上に移動
		transform_.pos = info.HitPosition;

		transform_.ResetGravityAccel();
	}
	
	// 斜め上方向に押し戻されたならジャンプや重力をリセットする
	if (0.0f < info.Normal.y && info.Normal.y < 1.0f)
	{
		if (transform_.IsRising()) return;

		// プレイヤーをステージ上に移動
		transform_.pos = info.HitPosition;

		// 重力加速度をリセット
		transform_.ResetGravityAccel();


		if (isJump_)
		{
			AnimEnd(0);

			AudioManager::GetInstance().Play(
				AudioList::AUDIO_TYPE::PLAYER_JUMP_LANDING,
				transform_.pos,
				2500.0f,
				false,
				1.0f
			);
		}

		// ジャンプ中かどうか
		isJump_ = false;
	}

	// 横方向に押し戻されたなら
	if (info.Normal.y == 0.0f)
	{
		// 計算用プレイヤー座標(Yを考慮しない)
		VECTOR pPos = transform_.pos;
		pPos.y = 0.0f;

		// 計算用平面(Yを考慮しない)
		VECTOR normal = info.Normal;
		VECTOR planePos = info.Position[0];
		planePos.y = 0.0f;

		// クランプ用
		// X座標
		float Xmin = info.Position[0].x;
		float Xmax = info.Position[0].x;
		// Z座標
		float Zmin = info.Position[0].z;
		float Zmax = info.Position[0].z;

		for (auto p : info.Position)
		{
			// X座標
			if (Xmin > p.x)Xmin = p.x;
			if (Xmax < p.x)Xmax = p.x;

			// Z座標
			if (Zmin > p.z)Zmin = p.z;
			if (Zmax < p.z)Zmax = p.z;
		}

		// プレイヤーに最も近い平面上の座標
		VECTOR nearPos = Plane_Point_MinLength_Position(planePos, normal, pPos);

		// ポリゴンの直線上でクランプ
		nearPos.x = std::clamp(nearPos.x, Xmin, Xmax);
		nearPos.z = std::clamp(nearPos.z, Zmin, Zmax);


		// でバック用
		debugPos = nearPos;


		// 押し戻す方向ベクトル計算
		VECTOR vec = VNorm(VSub(pPos, nearPos));


		// プレイヤーをステージ上に移動
		// 半径取得
		auto col = std::dynamic_pointer_cast<ColliderCapsule>(colliders_[(int)BODY::CAP]);
		nearPos.y = transform_.pos.y;
		transform_.pos = VAdd(nearPos,
			VScale(vec, col->GetRadius() + 0.1f));


		//// 減速
		//Decelerate();
	}

	// 座標回転など更新
	TransformUpdate();
}

void Player::OnHit(HITRESULT_LINE info)
{
	if (transform_.IsRising()) return;

	// プレイヤーをステージ上に移動
	transform_.pos = info.Position;

	// 重力加速度をリセット
	transform_.ResetGravityAccel();


	if (isJump_)
	{
		AnimEnd(0);

		AudioManager::GetInstance().Play(
			AudioList::AUDIO_TYPE::PLAYER_JUMP_LANDING,
			transform_.pos,
			2500.0f,
			false,
			1.0f
		);
	}

	// ジャンプ中かどうか
	isJump_ = false;

	// 座標回転など更新
	TransformUpdate();
}

void Player::OnHit(VECTOR normal)
{
	// 法線方向へ押し返す
	transform_.pos = VAdd(transform_.pos, VScale(normal, 0.5f));

	//int posX = transform_.pos.x * 100.0f;
	//posX /= 10;
	//transform_.pos.x = (float)posX * 10.0f / 100.0f;


	//int posZ = transform_.pos.z * 100.0f;
	//posZ /= 10;
	//transform_.pos.z = (float)posZ * 10.0f / 100.0f;

	// 上方向に押し戻されたならジャンプや重力をリセットする
	if (normal.y == 1)
	{
		if (transform_.IsRising()) return;

		transform_.ResetGravityAccel();

		if (isJump_)
		{
			AnimEnd(0);

			AudioManager::GetInstance().Play(
				AudioList::AUDIO_TYPE::PLAYER_JUMP_LANDING,
				transform_.pos,
				2500.0f,
				false,
				1.0f
			);
		}

		// ジャンプ中かどうか
		isJump_ = false;
	}

	// 下方向に押し戻されたならジャンプ力をリセットする
	if (normal.y == -1)
	{
		transform_.ResetGravityAccel();
	}

	// 横方向に押し戻されたなら
	if (0.0f == normal.y)
	{
		// 減速(スピードを減少させる)
		Decelerate();
	}

	//// 斜め下方向に押し戻されたならジャンプや重力をリセットする
	//if (-1.0f < normal.y && normal.y < 0.0f)
	//{
	//	if (transform_.IsFalling()) return;

	//	transform_.ResetGravityAccel();
	//}

	// 座標回転など更新
	TransformUpdate();


}

void Player::Init_Actor(void)
{

	// 仮読み込み
	model_ = std::make_unique<Model>(ResourceManager::GetInstance().Get("Model_Player_Player1")->GetHandle());
	model_->SetIsAutoDeleteHandle(true);
	transform_.SetModel(model_->GetHandle());

	if (key_ != -1) {
		// デフォルトハンドル
		modelGrHandle_ = MV1GetTextureGraphHandle(transform_.modelId, 0);
		modelGrHandle2_ = MV1GetTextureGraphHandle(transform_.modelId, 3);
		// 強調用ハンドル
		if (team_ == NetManager::GetInstance().GetSelf().team) {
			colorGrHandle_ = LoadGraph("Data/Model/Player/PlayerTextures/blue.png");
		}
		else {
			colorGrHandle_ = LoadGraph("Data/Model/Player/PlayerTextures/red.png");
		}
	}


	// ローカル回転
	VECTOR localRot = { Utility::Deg2RadF(0.0f),Utility::Deg2RadF(180.0f),Utility::Deg2RadF(0.0f) };
	transform_.quaRotLocal = Quaternion::Euler(localRot);
	transform_.quaRot = respawnQuaRot_;

	// 初期座標
	transform_.pos = respawnPos_;

	transform_.isGravity = true;
	transform_.heavy = 2.0f;
	transform_.height = 180.0f;

	transform_.Update();

	// 現在のアクション
	act_ = ACTION::IDLE;
	preAct_ = act_;
	actUp_ = ACTION::IDLE;
	preActUp_ = actUp_;

	// アクション中かどうか
	isAct_ = false;

	preMoveDir_ = {};
	moveDir_ = {};

	// 移動速度
	moveSpeed_ = 0.0f;

	// アニメーション設定
	AnimationSetting();


	// コライダーの設定
	ColliderSetting();

	// コライダー座標更新
	UpdateCollider();

	// 銃のタイプ別表示
	switch (gunType_)
	{
	case GUN_TYPE::AK47:
		gun_ = std::make_shared<GunAK47>(transform_, 35, std::dynamic_pointer_cast<ActorBase>(GetPtr()));
		gun_->Init();
		break;
	case GUN_TYPE::AWM:
		gun_ = std::make_shared<GunAWM>(transform_, 35, std::dynamic_pointer_cast<ActorBase>(GetPtr()));
		gun_->Init();
		break;
	case GUN_TYPE::M4:
		gun_ = std::make_shared<GunM4>(transform_, 35, std::dynamic_pointer_cast<ActorBase>(GetPtr()));
		gun_->Init();
		break;
	case GUN_TYPE::M24R:
		gun_ = std::make_shared<GunM24R>(transform_, 35, std::dynamic_pointer_cast<ActorBase>(GetPtr()));
		gun_->Init();
		break;
	}

	// 銃の回転
	gun_->ChangeState(GunBase::STATE::NONE);
	gunState_ = GunBase::STATE::NONE;

	// カメラ同期用オイラー
	auto cameraRot = transform_.quaRot.Mult(Quaternion::Euler({ Utility::Deg2RadF(-10.0f),0.0f,0.0f }));
	cameraRot_ = Quaternion::ToEuler(cameraRot);

	// 腰オイラー
	auto fMat = MV1GetFrameLocalMatrix(transform_.modelId, 2);
	auto fRotMat = MGetRotElem(fMat);		// 行列から回転を取り出す
	waistRot_ = Quaternion::ToEuler(Quaternion::GetRotation(fRotMat));

	// 反動
	recoil_ = std::make_unique<Recoil>();

	// ジャンプ中かどうか
	isJump_ = false;

	//// 現在選択しているグレネード
	//greType_ = GRENADE_TYPE::HAND;

	// 座標回転など更新
	TransformUpdate();

	autoHeal_.Init(AUTO_HEAL_TIME, SceneManager::GetInstance().GetDeltaTime());
	respawn_.Init(RESPAWN_COOL_TIME, SceneManager::GetInstance().GetDeltaTime());
	deathCamera_.Init(CHANGE_DEATHCAMERA_TIME, SceneManager::GetInstance().GetDeltaTime());
	invincibleTime_.Init(CHANGE_DEATHCAMERA_TIME, SceneManager::GetInstance().GetDeltaTime());

	hp_ = MAX_HP;
	hpMax_ = MAX_HP;

	// 頭を見えなくする
	if (controller_.IsMe()) {
		//HideHead();
	}

	// 足音間隔
	stepFootSound_ = 0.0f;
	
	//// コントローラ
	//controller_.SetBarrelPos(gun_->GetBarrelPos());
	//controller_.SetBulletRandLength(gun_->GetRandLength());
	//controller_.SetBulletRandRadius(gun_->GetRandRadius());
	//controller_.SetGunForward(gun_->GetFoward());
}

void Player::Update_Actor(void)
{
	//AudioManager::GetInstance().Play(
	//	AudioList::AUDIO_TYPE::ENEMY_DAMAGE,
	//	{},
	//	0.0f,
	//	false,
	//	0.0f
	//);


	// 更新前情報更新
	transform_.PreUpdate();
	for (auto col : colliders_)
	{
		col->PreUpdate();
	}

	if (act_ == ACTION::DEAD)
	{
		DeadUpdate();
		return;
	}

	invincibleTime_.Update();

	// 自動回復
	AutoHeal();

	// アクション操作
	ProcessAction();

	// 反動更新
	UpdateRecoil();

	recoil_->Update();

	// 重力
	transform_.Gravity();

	// アニメーション遷移
 	animCon_->ChangeAnimation((int)act_);
 	animCon_->ChangeAnimation((int)actUp_);

	// アニメーション更新
	animCon_->Update();


	//ChangeGunState(gunState_);

	// 座標回転など更新
	TransformUpdate();


	// 銃の回転
	if (gunState_ != GunBase::STATE::SHOT) {
		gun_->ChangeState(gunState_, 0.25f);
	}
	else {
		gun_->ChangeState(gunState_, 0.5f);
	}

	// 銃座標更新
	//gun_->Update();

	gameManager_.SetMagazine((int)ID_, gun_->GetMagazine());
	gameManager_.SetGreType((int)ID_, greType_);
	gameManager_.SetGrenadeNum((int)ID_, greNum_.at(greType_));


	//// コントローラ
	//controller_.SetBarrelPos(gun_->GetBarrelPos());
	//controller_.SetGunForward(gun_->GetFoward());

	// グレネードの更新
	//for (auto& g : gres_)
	//{
	//	g->Update();
	//}

	cameraDir_ = Quaternion::Euler(cameraRot_).GetForward();
}

void Player::Draw_Actor(void)
{
	if (key_ != -1 && key_ != NetManager::GetInstance().GetSelf().key) {
		SetUseLighting(false);
		MV1SetTextureGraphHandle(transform_.modelId, 0, colorGrHandle_, 0);
		MV1SetTextureGraphHandle(transform_.modelId, 3, colorGrHandle_, 0);

		transform_.scl = { 1.05f,1.05f, 1.05f };
		auto camera = std::dynamic_pointer_cast<GameCamera>(SceneManager::GetInstance().GetCamera());
		auto dir = VNorm(VSub(transform_.pos, camera->GetPos()));
		transform_.pos = VAdd(transform_.pos, VScale(dir, 5.0f));
		transform_.pos.y -= 3.0f;
		transform_.Update();
		MV1DrawModel(transform_.modelId);

		MV1SetTextureGraphHandle(transform_.modelId, 0, modelGrHandle_, 0);
		MV1SetTextureGraphHandle(transform_.modelId, 3, modelGrHandle2_, 0);
		SetUseLighting(true);

		transform_.scl = { 1.0f,1.0f,1.0f };
		transform_.pos = VSub(transform_.pos, VScale(dir, 5.0f));
		transform_.pos.y += 3.0f;
		transform_.Update();
	}


	// モデル描画
	transform_.Draw();

	//// 衝突クラス描画
	//for (int i = (int)BODY::CAP; i < (int)BODY::MAX; i++)
	//{
	//	colliders_[i]->DrawDebug();
	//}

	// 銃
	gun_->Draw();

	// グレネード
	for (auto& g : gres_) {
		g->Draw();
	}

	// デバッグ用
	//DrawFormatString(0, 30, 0xff00ff, "座標 : %.2f,%.2f,%.2f", transform_.pos.x, transform_.pos.y, transform_.pos.z);
	//switch (greType_)
	//{
	//case GRENADE_TYPE::HAND:
	//	DrawFormatString(0, 30 + GetFontSize(), 0xff00ff, "手榴弾");
	//	break;
	//case GRENADE_TYPE::SMOKE:
	//	DrawFormatString(0, 30 + GetFontSize(), 0xff00ff, "スモーク");
	//	break;
	//case GRENADE_TYPE::STICKY:
	//	DrawFormatString(0, 30 + GetFontSize(), 0xff00ff, "粘着爆弾");
	//	break;
	//default:
	//	DrawFormatString(0, 30 + GetFontSize(), 0xff00ff, "追加分");
	//	break;
	//}

	/*if (key_ != -1) {
		auto& players = NetManager::GetInstance().GetNetUsers();
		for (auto& p : players) {
			std::string name;
			for (auto s : p.second.playerName) {
				name.push_back(s);
			}
			DrawFormatString(0, 60 + GetFontSize() * 1 + 150 * (int)p.second.playerType, 0xffffff, name.c_str());
		}
	}*/

	//DrawFormatString(0, 60 + GetFontSize() * 2 + 150 * (int)ID_, 0xff00ff,
	//	"HP = %.2f / %.2f", hp_,hpMax_);
	//DrawFormatString(0, 60 + GetFontSize() * 3 + 150 * (int)ID_, 0xff00ff,
	//	"残弾数 : %d / %d", gun_->GetMagazine(), gun_->GetMaxMagazine());
	//DrawFormatString(0, 60 + GetFontSize() * 4 + 150 * (int)ID_, 0xff00ff, 
	//	"rot : %.2f,%.2f,%.2f", cameraRot_.x, cameraRot_.y, cameraRot_.z);
	//DrawFormatString(0, 60 + GetFontSize() * 5 + 150 * (int)ID_, 0xff00ff, 
	//	"pos : %.2f,%.2f,%.2f", transform_.pos.x, transform_.pos.y, transform_.pos.z);
	//if (isHide_) {
	//	DrawFormatString(0, 60 + GetFontSize() * 6 + 150 * (int)ID_, 0xff00ff, "IsHide : true");
	//}else {
	//	DrawFormatString(0, 60 + GetFontSize() * 6 + 150 * (int)ID_, 0xff00ff, "IsHide : false");
	//}

	
	// ハイド状態リセット
	gameManager_.SetIsHide((int)ID_, isHide_);
	isHide_ = false;

	//animCon_->DrawDebug();

	//DrawSphere3D(colliders_[(int)BODY::CHEST]->pos, 30.0f, 10, 0xffffff, 0xffffff, true);
	


}

void Player::Release_Actor(void)
{
	// モデル削除(仮)
	transform_.Release();

	// アニメーション
	animCon_->Release();

	// 銃
	gun_->Release();

	//
	for (auto g : gres_){
		g->Release();
	}
	gres_.clear();

	DeleteGraph(colorGrHandle_);
	DeleteGraph(modelGrHandle_);
	DeleteGraph(modelGrHandle2_);
}

void Player::DrawTrans(void)
{
	// 銃
	gun_->DrawTrans();

	// 十字ーーーーーーーーーーーーーーーーーーーー
	if (!controller_.IsADS() && controller_.IsMe() 
		&& act_ != ACTION::DEAD && gameManager_.IsStart()
		&& !gameManager_.IsEnd()) {
		float posX = (float)Application::SCREEN_SIZE_X / 2.0f;
		float posY = (float)Application::SCREEN_SIZE_Y / 2.0f;

		DrawLine(posX, posY - 10.0f, posX, posY - 30.0f, 0xffffff);
		DrawLine(posX, posY + 10.0f, posX, posY + 30.0f, 0xffffff);
		DrawLine(posX + 10.0f, posY, posX + 30.0f, posY, 0xffffff);
		DrawLine(posX - 10.0f, posY, posX - 30.0f, posY, 0xffffff);
	}
	// ーーーーーーーーーーーーーーーーーーーーーーー
}

void Player::ChangeAction(ACTION act)
{
	//// ジャンプ中は特定の行動しか受け付けない
	//if (isJump_) {
	//	if (act == ACTION::SHOT ||
	//		act == ACTION::RELOAD ||
	//		act == ACTION::GRENADE) {
	//		act_ = act;
	//	}
	//	return;
	//}

	// ジャンプ中は変更させない
	if (isJump_) return;
	act_ = act;

	//if (preAct_ == ACTION::RELOAD)
	//{
	//	// アニメーション終了後リロード
	//	// リロード
	//	gun_->ReloadGun();
	//}
}

void Player::ChangeActionUp(ACTION actUp)
{

	// ジャンプ中は特定の行動しか受け付けない
	if (isJump_) {
		if (actUp == ACTION::SHOT ||
			actUp == ACTION::RELOAD ||
			actUp == ACTION::GRENADE) {
			actUp_ = actUp;
		}
		return;
	}

	// 最後まで再生させるアニメーションが再生されている場合
	// 変更させない
	if (isAct_) return;
	actUp_ = actUp;

	if (preActUp_ == ACTION::RELOAD)
	{
		// アニメーション終了後リロード

	}
}

void Player::ChangeGunState(GunBase::STATE state)
{
	//switch (act_)
	//{
	//case Player::ACTION::IDLE:
	//case Player::ACTION::WALK_FORWARD:
	//case Player::ACTION::WALK_LEFT:
	//case Player::ACTION::WALK_RIGHT:
	//case Player::ACTION::WALK_BACK:
	//case Player::ACTION::RUN:
	//case Player::ACTION::GRENADE:
	//case Player::ACTION::CROUCH:
	//case Player::ACTION::CROUCH_WALK_FORWARD:
	//case Player::ACTION::CROUCH_LEFT:
	//case Player::ACTION::CROUCH_RIGHT:
	//case Player::ACTION::CROUCH_BACK:

	//	gunState_ = GunBase::STATE::NONE;

	//	break;

	//case Player::ACTION::SHOT:
	//case Player::ACTION::CROUCH_SHOT:

	//	gunState_ = GunBase::STATE::SHOT;

	//	break;

	//case Player::ACTION::RELOAD:
	//case Player::ACTION::CROUCH_RELOAD:

	//	gunState_ = GunBase::STATE::RELOAD;

	//	break;
	//	
	//}
	if (isAct_) return;

	gunState_ = state;
}

void Player::ChangeNextGrenade(void)
{

	for (int i = 0; i < (int)GRENADE_TYPE::MAX;i++)
	{
		int type = (int)greType_ + 1;
		if (type >= (int)GRENADE_TYPE::MAX)type = (int)GRENADE_TYPE::HAND;
		greType_ = (GRENADE_TYPE)type;

		// グレネードの数確認
		if (greNum_.at(greType_) >= 1) break;
	}
}

void Player::AnimationSetting(void)
{
	// アニメーターコントローラ
	std::vector<int> array = { 1 };
	animCon_ = std::make_unique<AnimatorController>(std::dynamic_pointer_cast<ActorBase>(GetPtr()),transform_, array);

	// アニメーションによる座標移動無効化用座標
	VECTOR defPos = { 0.0f, 95.0f, 0.0f };

	// アニメーション
	//int anim = -1;
	// 待機
	animModels_[(int)ACTION::IDLE] = std::make_unique<Model>(ResourceManager::GetInstance().Get("Model_Player_Animation_rifle_aiming_idle")->GetHandle());
	animModels_[(int)ACTION::IDLE]->SetIsAutoDeleteHandle(true);
	animCon_->SetAnimData((int)ACTION::IDLE, animModels_[(int)ACTION::IDLE]->GetHandle(), defPos);
	// 歩く(前)
	//anim = MV1LoadModel("Data/Model/Player/Animation/walking.mv1");
	animModels_[(int)ACTION::WALK_FORWARD] = std::make_unique<Model>(ResourceManager::GetInstance().Get("Model_Player_Animation_walking")->GetHandle());
	animModels_[(int)ACTION::WALK_FORWARD]->SetIsAutoDeleteHandle(true);
	animCon_->SetAnimData((int)ACTION::WALK_FORWARD, animModels_[(int)ACTION::WALK_FORWARD]->GetHandle(), defPos);
	animCon_->SetAnimSpeed((int)ACTION::WALK_FORWARD, 90.0f);
	animCon_->SetStartTime((int)ACTION::WALK_FORWARD, 30.0f);
	// 歩く(左)
	animModels_[(int)ACTION::WALK_LEFT] = std::make_unique<Model>(ResourceManager::GetInstance().Get("Model_Player_Animation_strafe_left")->GetHandle());
	animModels_[(int)ACTION::WALK_LEFT]->SetIsAutoDeleteHandle(true);
	//anim = MV1LoadModel("Data/Model/Player/Animation/strafe left.mv1");
	animCon_->SetAnimData((int)ACTION::WALK_LEFT, animModels_[(int)ACTION::WALK_LEFT]->GetHandle(), defPos);
	animCon_->SetAnimSpeed((int)ACTION::WALK_LEFT, 80.0f);
	animCon_->SetStartTime((int)ACTION::WALK_LEFT, 40.0f);
	// 歩く(右)
	//anim = MV1LoadModel("Data/Model/Player/Animation/strafe right.mv1");
	animModels_[(int)ACTION::WALK_RIGHT] = std::make_unique<Model>(ResourceManager::GetInstance().Get("Model_Player_Animation_strafe_right")->GetHandle());
	animModels_[(int)ACTION::WALK_RIGHT]->SetIsAutoDeleteHandle(true);
	animCon_->SetAnimData((int)ACTION::WALK_RIGHT, animModels_[(int)ACTION::WALK_RIGHT]->GetHandle(), defPos);
	animCon_->SetAnimSpeed((int)ACTION::WALK_RIGHT, 85.0f);
	animCon_->SetStartTime((int)ACTION::WALK_RIGHT, 40.0f);
	// 歩く(後)
	//anim = MV1LoadModel("Data/Model/Player/Animation/walking backwards.mv1");
	animModels_[(int)ACTION::WALK_BACK] = std::make_unique<Model>(ResourceManager::GetInstance().Get("Model_Player_Animation_walking_backwards")->GetHandle());
	animModels_[(int)ACTION::WALK_BACK]->SetIsAutoDeleteHandle(true);
	animCon_->SetAnimData((int)ACTION::WALK_BACK, animModels_[(int)ACTION::WALK_BACK]->GetHandle(), defPos);
	animCon_->SetAnimSpeed((int)ACTION::WALK_BACK, 70.0f);
	// 走る
	//anim = MV1LoadModel("Data/Model/Player/Animation/rifle run.mv1");
	animModels_[(int)ACTION::RUN] = std::make_unique<Model>(ResourceManager::GetInstance().Get("Model_Player_Animation_rifle_run")->GetHandle());
	animModels_[(int)ACTION::RUN]->SetIsAutoDeleteHandle(true);
	animCon_->SetAnimData((int)ACTION::RUN, animModels_[(int)ACTION::RUN]->GetHandle(), defPos);
	//// ジャンプ開始
	//anim = MV1LoadModel("Data/Model/Player/Animation/Jump Up.mv1");
	//animCon_->SetAnimData((int)ACTION::JUMP_UP, anim, defPos, (int)ACTION::JUMP_NOW, true);
	//animCon_->SetAnimSpeed((int)ACTION::JUMP_UP, 60.0f);
	// ジャンプ中dfa
	//anim = MV1LoadModel("Data/Model/Player/Animation/Jump Loop.mv1");
	animModels_[(int)ACTION::JUMP_NOW] = std::make_unique<Model>(ResourceManager::GetInstance().Get("Model_Player_Animation_Jump_Loop")->GetHandle());
	animModels_[(int)ACTION::JUMP_NOW]->SetIsAutoDeleteHandle(true);
	animCon_->SetAnimData((int)ACTION::JUMP_NOW, animModels_[(int)ACTION::JUMP_NOW]->GetHandle(), defPos);
	animCon_->SetDetachSpeed((int)ACTION::JUMP_NOW, 8.0f);
	//// ジャンプ終了
	//anim = MV1LoadModel("Data/Model/Player/Animation/Jump Down.mv1");
	//animCon_->SetAnimData((int)ACTION::JUMP_DOWN, anim, defPos, (int)ACTION::IDLE);

	// 撃つ
	//anim = MV1LoadModel("Data/Model/Player/Animation/firing rifle.mv1");
	animModels_[(int)ACTION::SHOT] = std::make_unique<Model>(ResourceManager::GetInstance().Get("Model_Player_Animation_firing_rifle")->GetHandle());
	animModels_[(int)ACTION::SHOT]->SetIsAutoDeleteHandle(true);
	animCon_->SetAnimData((int)ACTION::SHOT, animModels_[(int)ACTION::SHOT]->GetHandle(), defPos, 1);
	// リロード
	//anim = MV1LoadModel("Data/Model/Player/Animation/reloading.mv1");
	animModels_[(int)ACTION::RELOAD] = std::make_unique<Model>(ResourceManager::GetInstance().Get("Model_Player_Animation_reloading")->GetHandle());
	animModels_[(int)ACTION::RELOAD]->SetIsAutoDeleteHandle(true);
	//animCon_->SetAnimData((int)ACTION::RELOAD, anim, defPos, 1,(int)ACTION::IDLE, false);
	animCon_->SetAnimData((int)ACTION::RELOAD, animModels_[(int)ACTION::RELOAD]->GetHandle(), defPos, 1);
	animCon_->SetNoticeTime((int)ACTION::RELOAD, 138.0f);
	animCon_->SetCallAnimEnd((int)ACTION::RELOAD, true);
	// グレネード
	//anim = MV1LoadModel("Data/Model/Player/Animation/Toss Grenade.mv1");
	animModels_[(int)ACTION::GRENADE] = std::make_unique<Model>(ResourceManager::GetInstance().Get("Model_Player_Animation_toss_grenade")->GetHandle());
	animModels_[(int)ACTION::GRENADE]->SetIsAutoDeleteHandle(true);
	//animCon_->SetAnimData((int)ACTION::GRENADE, anim, defPos, 1,(int)ACTION::IDLE,true);
	animCon_->SetAnimData((int)ACTION::GRENADE, animModels_[(int)ACTION::GRENADE]->GetHandle(), defPos, 1);
	animCon_->SetCallAnimEnd((int)ACTION::GRENADE, true);
	animCon_->SetNoticeTime((int)ACTION::GRENADE, 110.0f);
	animCon_->SetAnimSpeed((int)ACTION::GRENADE, 90.0f);
	animCon_->SetStartTime((int)ACTION::GRENADE, 50.0f);

	// 死亡
	//anim = MV1LoadModel("Data/Model/Player/Animation/Death.mv1");
	animModels_[(int)ACTION::DEAD] = std::make_unique<Model>(ResourceManager::GetInstance().Get("Model_Player_Animation_Death")->GetHandle());
	animModels_[(int)ACTION::DEAD]->SetIsAutoDeleteHandle(true);
	animCon_->SetAnimData((int)ACTION::DEAD, animModels_[(int)ACTION::DEAD]->GetHandle(), {0.0f,0.0f,0.0f});
	animCon_->SetNoticeTime((int)ACTION::DEAD, 180.0f);
	animCon_->SetAnimSpeed((int)ACTION::DEAD, 90.0f);

	// しゃがむ
	//anim = MV1LoadModel("Data/Model/Player/Animation/Rifle Kneel Idle.mv1");
	animModels_[(int)ACTION::CROUCH] = std::make_unique<Model>(ResourceManager::GetInstance().Get("Model_Player_Animation_Rifle_Kneel_Idle")->GetHandle());
	animModels_[(int)ACTION::CROUCH]->SetIsAutoDeleteHandle(true);
	animCon_->SetAnimData((int)ACTION::CROUCH, animModels_[(int)ACTION::CROUCH]->GetHandle(), {0.0f,35.0f,0.0f});

	// しゃがむ歩く(前)
	//anim = MV1LoadModel("Data/Model/Player/Animation/Crouch Walking.mv1");
	animModels_[(int)ACTION::CROUCH_WALK_FORWARD] = std::make_unique<Model>(ResourceManager::GetInstance().Get("Model_Player_Animation_Crouch_Walking")->GetHandle());
	animModels_[(int)ACTION::CROUCH_WALK_FORWARD]->SetIsAutoDeleteHandle(true);
	animCon_->SetAnimData((int)ACTION::CROUCH_WALK_FORWARD, animModels_[(int)ACTION::CROUCH_WALK_FORWARD]->GetHandle(), {0.0f,70.0f,0.0f});
	
	// しゃがむ歩く(左)
	//anim = MV1LoadModel("Data/Model/Player/Animation/Crouch Walk Strafe Left.mv1");
	animModels_[(int)ACTION::CROUCH_LEFT] = std::make_unique<Model>(ResourceManager::GetInstance().Get("Model_Player_Animation_Crouch_Walk_Strafe_Left")->GetHandle());
	animModels_[(int)ACTION::CROUCH_LEFT]->SetIsAutoDeleteHandle(true);
	animCon_->SetAnimData((int)ACTION::CROUCH_LEFT, animModels_[(int)ACTION::CROUCH_LEFT]->GetHandle(), {0.0f,70.0f,0.0f});

	// しゃがむ歩く(右)
	//anim = MV1LoadModel("Data/Model/Player/Animation/Crouch Walk Strafe Right.mv1");
	animModels_[(int)ACTION::CROUCH_RIGHT] = std::make_unique<Model>(ResourceManager::GetInstance().Get("Model_Player_Animation_Crouch_Walk_Strafe_Right")->GetHandle());
	animModels_[(int)ACTION::CROUCH_RIGHT]->SetIsAutoDeleteHandle(true);
	animCon_->SetAnimData((int)ACTION::CROUCH_RIGHT, animModels_[(int)ACTION::CROUCH_RIGHT]->GetHandle(), {0.0f,70.0f,0.0f});
	
	// しゃがむ歩く(後)
	//anim = MV1LoadModel("Data/Model/Player/Animation/Crouch Walking Backwards.mv1");
	animModels_[(int)ACTION::CROUCH_BACK] = std::make_unique<Model>(ResourceManager::GetInstance().Get("Model_Player_Animation_Crouch_Walking_Backwards")->GetHandle());
	animModels_[(int)ACTION::CROUCH_BACK]->SetIsAutoDeleteHandle(true);
	animCon_->SetAnimData((int)ACTION::CROUCH_BACK, animModels_[(int)ACTION::CROUCH_BACK]->GetHandle(), {0.0f,70.0f,0.0f});
	
	// しゃがむリロード
	//anim = MV1LoadModel("Data/Model/Player/Animation/Crouch Reload.mv1");
	animModels_[(int)ACTION::CROUCH_RELOAD] = std::make_unique<Model>(ResourceManager::GetInstance().Get("Model_Player_Animation_Crouch_Reload")->GetHandle());
	animModels_[(int)ACTION::CROUCH_RELOAD]->SetIsAutoDeleteHandle(true);
	animCon_->SetAnimData((int)ACTION::CROUCH_RELOAD, animModels_[(int)ACTION::CROUCH_RELOAD]->GetHandle(), {0.0f,70.0f,0.0f});
	
	// しゃがむ撃つ
	//anim = MV1LoadModel("Data/Model/Player/Animation/Crouch Fire Rifle.mv1");
	animModels_[(int)ACTION::CROUCH_SHOT] = std::make_unique<Model>(ResourceManager::GetInstance().Get("Model_Player_Animation_Crouch_Fire_Rifle")->GetHandle());
	animModels_[(int)ACTION::CROUCH_SHOT]->SetIsAutoDeleteHandle(true);
	animCon_->SetAnimData((int)ACTION::CROUCH_SHOT, animModels_[(int)ACTION::CROUCH_SHOT]->GetHandle(), {0.0f,70.0f,0.0f});


}

void Player::ColliderSetting(void)
{
	// 頭
	colliders_[(int)BODY::HEAD] = std::make_shared<ColliderSphere>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_HEAD,
		15.0f);
	// 衝突管理クラスにセット
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::HEAD]);

	// 胸
	colliders_[(int)BODY::CHEST] = std::make_shared<ColliderCapsule>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_BODY,
		20.0f, 20.0f);
	// 衝突管理クラスにセット
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::CHEST]);

	// 腰
	colliders_[(int)BODY::WAIST] = std::make_shared<ColliderCapsule>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_BODY,
		20.0f, 10.0f);
	// 衝突管理クラスにセット
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::WAIST]);

	// 左腕
	colliders_[(int)BODY::ARM_LEFT] = std::make_shared<ColliderCapsule>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_LIMBS,
		10.0f, 15.0f);
	// 衝突管理クラスにセット
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::ARM_LEFT]);

	// 左手首
	colliders_[(int)BODY::WRIST_LEFT] = std::make_shared<ColliderCapsule>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_LIMBS,
		10.0f, 15.0f);
	// 衝突管理クラスにセット
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::WRIST_LEFT]);

	// 右腕
	colliders_[(int)BODY::ARM_RIGHT] = std::make_shared<ColliderCapsule>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_LIMBS,
		10.0f, 15.0f);
	// 衝突管理クラスにセット
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::ARM_RIGHT]);

	// 右手首
	colliders_[(int)BODY::WRIST_RIGHT] = std::make_shared<ColliderCapsule>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_LIMBS,
		10.0f, 15.0f);
	// 衝突管理クラスにセット
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::WRIST_RIGHT]);

	// 左太もも
	colliders_[(int)BODY::THIGH_LEFT] = std::make_shared<ColliderCapsule>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_LIMBS,
		12.0f, 19.0f);
	// 衝突管理クラスにセット
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::THIGH_LEFT]);

	// 左すね
	colliders_[(int)BODY::SHIN_LEFT] = std::make_shared<ColliderCapsule>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_LIMBS,
		10.0f, 30.0f);
	// 衝突管理クラスにセット
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::SHIN_LEFT]);

	// 右太もも
	colliders_[(int)BODY::THIGH_RIGHT] = std::make_shared<ColliderCapsule>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_LIMBS,
		12.0f, 19.0f);
	// 衝突管理クラスにセット
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::THIGH_RIGHT]);

	// 右すね
	colliders_[(int)BODY::SHIN_RIGHT] = std::make_shared<ColliderCapsule>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_LIMBS,
		10.0f, 30.0f);
	// 衝突管理クラスにセット
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::SHIN_RIGHT]);

	// マップ衝突用ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー

	// カプセル
	colliders_[(int)BODY::CAP] = std::make_shared<ColliderCapsule>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_CAP,
		55.0f, 70.0f);
	// 衝突管理クラスにセット
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::CAP]);

	// 線分(頭上)
	colliders_[(int)BODY::TOPLINE] = std::make_shared<ColliderLine>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_TOPLINE);
	// 衝突管理クラスにセット
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::TOPLINE]);

	// 線分(足元)
	colliders_[(int)BODY::DOWNLINE] = std::make_shared<ColliderLine>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_DOWNLINE);
	// 衝突管理クラスにセット
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::DOWNLINE]);

	// ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー

	// 視線
	colliders_[(int)BODY::EYE] = std::make_shared<ColliderLine>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_EYE);
	// 衝突管理クラスにセット
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::EYE]);

	// 視線
	colliders_[(int)BODY::RAY] = std::make_shared<ColliderLine>(GetPtr(),
		Collider::OBJECT_TYPE::RAY);
	// 衝突管理クラスにセット
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::RAY]);
}

void Player::ProcessDead(void)
{

	if (greInHand_.lock() != nullptr &&
		greInHand_.lock()->IsHold()) {
		// 投げる方向(向いている方向のちょっと上)
		auto rot = cameraRot_;
		rot.x += Utility::Deg2RadF(-30.0f);
		Quaternion q = Quaternion::Euler(rot);
		greInHand_.lock()->Throw(q.GetForward(), GRENADE_POW / 5.0f);
	}
	

 	animCon_->Reset();
	animCon_->SetValidAnimMove(true);
	// フレーム回転リセット
	MV1ResetFrameUserLocalMatrix(transform_.modelId, 0);

	// フレーム回転リセット
	MV1ResetFrameUserLocalMatrix(transform_.modelId, 2);

	// 腰オイラー
	auto fMat = MV1GetFrameLocalMatrix(transform_.modelId, 2);
	auto fRotMat = MGetRotElem(fMat);		// 行列から回転を取り出す
	waistRot_ = Quaternion::ToEuler(Quaternion::GetRotation(fRotMat));


	animCon_->SetIsStop((int)ACTION::DEAD, false);
	isAct_ = false;
	isJump_ = false;
	ChangeAction(ACTION::DEAD);
	ChangeActionUp(ACTION::DEAD);
	// アニメーション遷移
	animCon_->ChangeAnimation((int)act_);
	animCon_->ChangeAnimation((int)actUp_);

	// 銃の回転
	gun_->ChangeState(GunBase::STATE::NONE, 0.5f);
	gun_->ReloadGun();

	deathCamera_.Set();
	respawn_.Set();

	gameManager_.Kill(ID_, lastAtkID_);



	AudioManager::GetInstance().Play(
		(AudioList::AUDIO_TYPE)((int)AudioList::AUDIO_TYPE::PLAYER_DEATH1 + GetRand(2)),
		transform_.pos,
		2500.0f,
		false,
		1.0f
	);


	//if (controller_.IsMe()) {
	//	DisplayHead();
	//}
	// キルログ
	// UIManager::なんか(ID_,lastAtkID_);

	//for (auto& col : colliders_)
	//{
	//	col->isCollision = false;
	//}
	//colliders_[(int)BODY::DOWNLINE]->isCollision = true;


	auto camera = SceneManager::GetInstance().GetCamera();
	auto gCamera = std::dynamic_pointer_cast<GameCamera>(camera);
	if (gCamera->GetMode((int)ID_) == GameCamera::MODE::ADS) {
		if (controller_.IsMe()) {
			gCamera->ChangeMode(GameCamera::MODE::FPS, GetPtr(), 0.5f);
		}
		gCamera->ChangeMode((int)ID_, GameCamera::MODE::FPS, GetPtr(), 0.5f);
	}

	gameManager_.SetMagazine((int)ID_, gun_->GetMagazine());
	gameManager_.SetGreType((int)ID_, greType_);
	gameManager_.SetGrenadeNum((int)ID_, greNum_.at(greType_));

	//camera->ChangeMode(GameCamera::MODE::DEATH,3.0f);
	//camera->SetFollowTarget(lastAtkObj_.lock());
}

void Player::Respawn(void)
{
	transform_.pos = respawnPos_;
	transform_.quaRot = respawnQuaRot_;

	// カメラ同期用オイラー
	auto cameraRot = transform_.quaRot.Mult(Quaternion::Euler({ Utility::Deg2RadF(-10.0f),0.0f,0.0f }));
	cameraRot_ = Quaternion::ToEuler(cameraRot);



	// フレーム回転リセット
	MV1ResetFrameUserLocalMatrix(transform_.modelId, 2);

	// 腰オイラー
	auto fMat = MV1GetFrameLocalMatrix(transform_.modelId, 2);
	auto fRotMat = MGetRotElem(fMat);		// 行列から回転を取り出す
	waistRot_ = Quaternion::ToEuler(Quaternion::GetRotation(fRotMat));

	isAct_ = false;

	// 現在のアクション(待機モーション)
	ChangeAction(ACTION::IDLE);
	ChangeActionUp(ACTION::IDLE);
	animCon_->SetValidAnimMove(false);

	// 銃状態変更
	ChangeGunState(GunBase::STATE::NONE);

	animCon_->Reset();

	TransformUpdate();

	//if (controller_.IsMe()) {
	//	HideHead();
	//}

	hp_ = MAX_HP;
	GetGrenade(loadOut_.greType_[0], 5);
	GetGrenade(loadOut_.greType_[1], 5);
	if (loadOut_.greType_[0] == loadOut_.greType_[1]) {
		auto& gn = greNum_.at(loadOut_.greType_[0]);
		gn *= 2;
	}

	// ハイド状態解除
	isHide_ = false;

	invincibleTime_.Set();
}

void Player::AutoHeal(void)
{
	if (hp_ >= MAX_HP) return;

	if (!autoHeal_.Update()) return;

	hp_ += SceneManager::GetInstance().GetDeltaMult(HEAL_HP);
	if (hp_ >= MAX_HP)hp_ = MAX_HP;
}

void Player::DeadUpdate(void)
{

	//// 重力
	//transform_.Gravity();

	//// アニメーション遷移
	//animCon_->ChangeAnimation((int)act_);

	//// アニメーション更新
	//animCon_->Update();


	//ChangeGunState(GunBase::STATE::NONE);

	//// 座標回転など更新
	//TransformUpdate();


	//// 銃の回転
	//gun_->ChangeState(gunState_);

	//// 銃座標更新
	//gun_->Update();

	//// グレネードの更新
	//for (auto& g : gres_)
	//{
	//	g->Update();
	//}

	//if (respawn_.Update())
	//{
	//	Respawn();
	//}

	//////////////////////

	// 自動回復
	//AutoHeal();

	// アクション操作
	//ProcessAction();

	// ハイド状態にする
	isHide_ = true;

	// 重力
	transform_.Gravity();

	// アニメーション遷移
	animCon_->ChangeAnimation((int)act_);
	//animCon_->ChangeAnimation((int)act_);
	animCon_->ChangeAnimation((int)actUp_);

	// アニメーション更新
	animCon_->Update();


	//ChangeGunState(gunState_);

	// 座標回転など更新
	TransformUpdate();


	// 銃の回転
	//if (gunState_ != GunBase::STATE::SHOT) {
	//	gun_->ChangeState(gunState_, 0.25f);
	//}
	//else {
	//	gun_->ChangeState(gunState_, 0.5f);
	//}

	// 銃座標更新
	gun_->Update();



	//// コントローラ
	//controller_.SetBarrelPos(gun_->GetBarrelPos());
	//controller_.SetGunForward(gun_->GetFoward());

	// グレネードの更新
	for (auto& g : gres_)
	{
		g->Update();
	}



	if (deathCamera_.Update())
	{
		// デスカメラ切り替え
		auto camera = dynamic_pointer_cast<GameCamera>(SceneManager::GetInstance().GetCamera());
		if (controller_.IsMe()) {
			camera->ChangeMode(GameCamera::MODE::DEATH, 3.0f);
			camera->SetFollowTarget(lastAtkObj_.lock());
			//gCamera->ChangeMode(GameCamera::MODE::ADS, GetPtr(), 0.0f);
		}
		camera->ChangeMode((int)ID_, GameCamera::MODE::DEATH, lastAtkObj_.lock(), 3.0f);
		deathCamera_.Stop();
	}

	if (respawn_.Update())
	{
		Respawn();
	}
}

void Player::ProcessAction(void)
{
	// 1フレーム前のアクション
	preAct_ = act_;
	preActUp_ = actUp_;

	// 現在のアクション(待機モーション)
	ChangeAction(ACTION::IDLE);
	ChangeActionUp(ACTION::IDLE);

	// 銃状態変更
	ChangeGunState(GunBase::STATE::NONE);

	// 回転操作(視点操作)
	ProcessRotate();

	// ジャンプ操作
	ProcessJump();

	// しゃがみ操作
	ProcessCrouch();

	// 移動操作
	ProcessMove();

	// ADS操作
	ProcessADS();

	//if (team_ == 0) {
	//	if (transform_.pos.z <= actionControlPosZ_) {
	//		return;
	//	}
	//}
	//else if (team_ == 1) {
	//	if (transform_.pos.z >= actionControlPosZ_) {
	//		return;
	//	}
	//}

	// 撃つ操作
	ProcessShot();

	// リロード操作
	ProcessReload();

	// グレネード操作
	ProcessGrenade();

}


void Player::ProcessMove(void)
{
	auto& input = InputManager::GetInstance();

	// 移動方向
	preMoveDir_ = moveDir_;
	moveDir_ = {};

	switch (act_)
	{
	case Player::ACTION::CROUCH:

		if (controller_.IsMoveF())
		{
			moveDir_ = VAdd(moveDir_, transform_.GetForward());


			// 移動量
			if (preAct_ == ACTION::CROUCH_BACK)moveSpeed_ = 0.0f;

			// 現在のアクション
			ChangeAction(ACTION::CROUCH_WALK_FORWARD);
			ChangeActionUp(ACTION::CROUCH_WALK_FORWARD);
		}
		else if (controller_.IsMoveB())
		{
			moveDir_ = VAdd(moveDir_, transform_.GetBack());

			// 移動量
			if (preAct_ == ACTION::CROUCH_WALK_FORWARD)moveSpeed_ = 0.0f;

			// 現在のアクション
			ChangeAction(ACTION::CROUCH_BACK);
			ChangeActionUp(ACTION::CROUCH_BACK);
		}


		if (controller_.IsMoveL())
		{
			moveDir_ = VAdd(moveDir_, transform_.GetLeft());

			// 移動量
			if (preAct_ == ACTION::CROUCH_RIGHT)moveSpeed_ = 0.0f;

			// 現在のアクション
			ChangeAction(ACTION::CROUCH_LEFT);
			ChangeActionUp(ACTION::CROUCH_LEFT);
		}
		else if (controller_.IsMoveR())
		{
			moveDir_ = VAdd(moveDir_, transform_.GetRight());

			// 移動量
			if (preAct_ == ACTION::CROUCH_LEFT)moveSpeed_ = 0.0f;

			// 現在のアクション
			ChangeAction(ACTION::CROUCH_RIGHT);
			ChangeActionUp(ACTION::CROUCH_RIGHT);
		}

		break;
	//case Player::ACTION::JUMP_NOW:
	//	moveDir_ = preMoveDir_;
	//	break;
	default:

		if (controller_.IsMoveF())
		{
			moveDir_ = VAdd(moveDir_, transform_.GetForward());


			// 移動量
			if (preAct_ == ACTION::WALK_BACK)moveSpeed_ = 0.0f;

			// 現在のアクション
			ChangeAction(ACTION::WALK_FORWARD);
			ChangeActionUp(ACTION::WALK_FORWARD);
			if (controller_.IsPutDash()) {
				ChangeAction(ACTION::RUN);
				ChangeActionUp(ACTION::RUN);
			}
		}
		else if (controller_.IsMoveB())
		{
			moveDir_ = VAdd(moveDir_, transform_.GetBack());

			// 移動量
			if (preAct_ == ACTION::WALK_FORWARD ||
				preAct_ == ACTION::RUN)moveSpeed_ = 0.0f;

			// 現在のアクション
			ChangeAction(ACTION::WALK_BACK);
			ChangeActionUp(ACTION::WALK_BACK);
		}


		if (controller_.IsMoveL())
		{
			moveDir_ = VAdd(moveDir_, transform_.GetLeft());

			// 移動量
			if (preAct_ == ACTION::WALK_RIGHT)moveSpeed_ = 0.0f;

			// 現在のアクション
			ChangeAction(ACTION::WALK_LEFT);
			ChangeActionUp(ACTION::WALK_LEFT);
		}
		else if (controller_.IsMoveR())
		{
			moveDir_ = VAdd(moveDir_, transform_.GetRight());

			// 移動量
			if (preAct_ == ACTION::WALK_LEFT)moveSpeed_ = 0.0f;

			// 現在のアクション
			ChangeAction(ACTION::WALK_RIGHT);
			ChangeActionUp(ACTION::WALK_RIGHT);
		}

		break;
	}
	
	// 移動量
	float movePow = moveSpeed_;

	if (!Utility::EqualsVZero(moveDir_))
	{
		// Y方向は変えない
		if (moveDir_.y > 0.0f) moveDir_.y = 0.0f;

		// 加速
		Accele(MOVE_ACC);

		switch (act_)
		{
		case Player::ACTION::WALK_FORWARD:
			movePow *= 1.0f;
			break;
		case Player::ACTION::WALK_BACK:
			movePow *= 0.5f;
			break;
		case Player::ACTION::RUN:
			movePow *= 1.4f;
			break;
		case Player::ACTION::JUMP_NOW:
			movePow *= 0.6f;
			break;
		case Player::ACTION::CROUCH_WALK_FORWARD:
		case Player::ACTION::CROUCH_BACK:
		case Player::ACTION::CROUCH_LEFT:
		case Player::ACTION::CROUCH_RIGHT:
			movePow *= 0.5f;
			break;
		default:
			movePow *= 0.5f;
			break;
		}

		transform_.pos = 
			VAdd(transform_.pos, 
				VScale(moveDir_, SceneManager::GetInstance().GetDeltaMult(movePow)));

		if (!isJump_) {
			float rate = (movePow / (WARK_SPEED_MAX * 1.2f));
			//if (rate >= 1.0f)rate = 1.0f;
			//animCon_->SetAnimSpeed((int)act_, 60.0f * rate);

			// 足音間隔
			//rate = (movePow / (WARK_SPEED_MAX * 1.2f));
			rate = clamp(rate, 0.1f, 1.0f);
			stepFootSound_ += 1.0f / 60.0f;
			if (stepFootSound_ >= SOUND_FOOT / rate) {
				AudioManager::GetInstance().Play(
					AudioList::AUDIO_TYPE::PLAYER_FOOTFALL,
					transform_.pos,
					2500.0f,
					false,
					0.65f
				);
				stepFootSound_ = 0.0f;
			}
		}
		

		if (team_ == 0) {
			if (transform_.pos.z >= moveControlPosZ_ - 30.0f) {
				transform_.pos.z = moveControlPosZ_ - 30.0f;

				EffectManager::EFFECT temp;
				temp.pos = transform_.pos;
				temp.pos.x += 10.0f;
				temp.pos.y += 150.0f;
				temp.pos.z = moveControlPosZ_ + 20.0f;
				temp.rot = { 0.0f,Utility::Deg2RadF(180.0f),0.0f };
				temp.scl = { 10.0f,10.0f,10.0f };
				temp.speed = 2.0f;
				temp.type = EffectManager::EFFECT_TYPE::BARRIERLIGHT;
				EffectManager::GetInstance().AddEffect(temp);
			}
		}
		else if (team_ == 1) {
			if (transform_.pos.z <= moveControlPosZ_ + 30.0f) {
				transform_.pos.z = moveControlPosZ_ + 30.0f;

				EffectManager::EFFECT temp;
				temp.pos = transform_.pos;
				temp.pos.x -= 10.0f;
				temp.pos.y += 150.0f;
				temp.pos.z = moveControlPosZ_ - 20.0f;
				temp.rot = {};
				temp.scl = { 10.0f,10.0f,10.0f };
				temp.speed = 2.0f;
				temp.type = EffectManager::EFFECT_TYPE::BARRIERLIGHT;
				EffectManager::GetInstance().AddEffect(temp);
			}
		}


	}
	else
	{
		// 減速
		Decelerate();
	}

	// 座標回転など更新
	TransformUpdate();
}

void Player::Accele(float speed)
{
	// 加速
	moveSpeed_ += speed;
	if (WARK_SPEED_MAX <= moveSpeed_)moveSpeed_ = WARK_SPEED_MAX;
}

void Player::Decelerate(void)
{
	// 減速
	moveSpeed_ = 0.0f;
}

void Player::ProcessCrouch(void)
{


	if (controller_.IsPutCrouch())
	{
		if (preAct_ != ACTION::CROUCH &&
			preAct_ != ACTION::CROUCH_WALK_FORWARD &&
			preAct_ != ACTION::CROUCH_BACK &&
			preAct_ != ACTION::CROUCH_RIGHT &&
			preAct_ != ACTION::CROUCH_LEFT &&
			!isJump_) {

			AudioManager::GetInstance().Play(
				AudioList::AUDIO_TYPE::PLAYER_CROUCHING,
				transform_.pos,
				2500.0f,
				false,
				0.6f
			);
		}

		// 現在のアクション
		ChangeAction(ACTION::CROUCH);
		ChangeActionUp(ACTION::CROUCH);


	}

	// 座標回転など更新
	TransformUpdate();
}

void Player::ProcessRotate(void)
{

	//// マウス回転量
	//float dpi = 0.1f;

	//// マウス位置
	//Vector2<int> mousePos;

	//// 画面の中心位置
	//Vector2 center = { Application::SCREEN_SIZE_X / 2,Application::SCREEN_SIZE_Y / 2 };

	//// マウス位置の取得
	//GetMousePoint(&mousePos.x, &mousePos.y);

	//// カメラ回転の計算(マウスカーソル位置と画面の中心の差分を計算し、回転量/FPSを乗算する)
	//// これが回転量
	//float rotPowY = float(std::clamp(mousePos.x - center.x, -120, 120)) * dpi / GetFPS();	// マウス横移動
	//float rotPowX = float(std::clamp(mousePos.y - center.y, -120, 120)) * dpi / GetFPS();	// マウス縦移動

	if (!controller_.IsRotate()) return;

	auto rotPow = controller_.GetRotatePow();
	float rotPowY = rotPow.y;
	float rotPowX = rotPow.x;

	// 回転させる3つの角度をすべてオイラー角に変換しておく(計算しやすいため)
	// プレイヤー
	VECTOR pRot = transform_.quaRot.ToEuler();

	// カメラ同期用はすでにオイラー角なので変換しなくてOK
	cameraRot_;

	//// 腰のフレーム
	//// 対象フレームのローカル行列(大きさ、回転、位置)を取得する
	//auto fMat = MV1GetFrameLocalMatrix(transform_.modelId, 2);
	//auto fScl = MGetSize(fMat);			// 行列から大きさを取り出す
	//auto fRotMat = MGetRotElem(fMat);		// 行列から回転を取り出す
	//auto fPos = MGetTranslateElem(fMat);	// 行列から移動値を取り出す
	//VECTOR fRot = Quaternion::ToEuler(Quaternion::GetRotation(fRotMat));

	// 腰のフレーム
	// 対象フレームのローカル行列(大きさ、回転、位置)を取得する
	auto fMat = MV1GetFrameLocalMatrix(transform_.modelId, 2);
	auto fScl = MGetSize(fMat);			// 行列から大きさを取り出す
	auto fPos = MGetTranslateElem(fMat);	// 行列から移動値を取り出す

	

	// Y軸回転ーーーーーーーーーーーーーーーーーーーーーーーー	
	// プレイヤー回転
	pRot.y += rotPowY;

	// カメラ同期用回転
	cameraRot_.y += rotPowY;




	// X軸回転ーーーーーーーーーーーーーーーーーーーーーーーー
	// カメラ同期用回転
	cameraRot_.x += rotPowX;

	// カメラのX軸回転制限
	cameraRot_.x = std::clamp(cameraRot_.x, Utility::Deg2RadF(-50.0f), Utility::Deg2RadF(60.0f));

	// 腰回転
	//fRot.x -= rotPowX;
	waistRot_.x -= rotPowX;

	// 腰のX軸回転制限
	//fRot.x = std::clamp(fRot.x, Utility::Deg2RadF(-70.0f), Utility::Deg2RadF(40.0f));
	waistRot_.x = std::clamp(waistRot_.x, Utility::Deg2RadF(-70.0f), Utility::Deg2RadF(40.0f));


	// 回転させたものを実物にセットーーーーーーーーーーーーー
	// プレイヤー
	transform_.quaRot = Quaternion::Euler(pRot);

	// 腰
	// 大きさ、回転、位置をローカル行列に戻す
	MATRIX mix = MGetIdent();
	mix = MMult(mix, MGetScale(fScl));		// 大きさ
	MATRIX matRot = MGetIdent();			// 回転
	matRot = MMult(matRot, MGetRotX(waistRot_.x));
	matRot = MMult(matRot, MGetRotY(waistRot_.y));
	matRot = MMult(matRot, MGetRotZ(waistRot_.z));
	//matRot = MMult(matRot, MGetRotX(fRot.x));
	//matRot = MMult(matRot, MGetRotY(fRot.y));
	//matRot = MMult(matRot, MGetRotZ(fRot.z));
	mix = MMult(mix, matRot);				
	mix = MMult(mix, MGetTranslate(fPos));	// 座標
	// 合成した行列を対象フレームにセットし直す
	MV1SetFrameUserLocalMatrix(transform_.modelId, 2, mix);

	// カメラ同期用はそのまま
	cameraRot_;

	// カメラ位置を中心にセット
	//SetMousePoint(center.x, center.y);

	TransformUpdate();
}

void Player::ProcessJump(void)
{
	if (isJump_) return;

	auto& input = InputManager::GetInstance();

	if (controller_.IsPutJump())
	{
		transform_.Jump(8.0f);

		// 現在のアクション
		ChangeAction(ACTION::JUMP_NOW);
		ChangeActionUp(ACTION::JUMP_NOW);

		// ジャンプ中かどうか
		isJump_ = true;

		AudioManager::GetInstance().Play(
			AudioList::AUDIO_TYPE::PLAYER_JUMP,
			transform_.pos,
			2500.0f,
			false,
			0.7f
		);

		// アクション中かどうか
		//isAct_ = true;
	}

	TransformUpdate();
}

void Player::ProcessShot(void)
{
	//auto& input = InputManager::GetInstance();

	if (controller_.IsPutShot() && !isAct_)
	{
		if (CollisionManager::GetInstance().CollisionRayMap(colliders_[(int)BODY::RAY])) {
			return;
		}

		if (!gun_->IsRemainMagazine() && !isPutShotPre_) {
			AudioManager::GetInstance().Play(
				AudioList::AUDIO_TYPE::GUN_EMPTY_MAGAZINE,
				transform_.pos,
				500.0f,
				false,
				1.0f
			);
			isPutShotPre_ = controller_.IsPutShot();
			return;
		}

		// ここで、反動処理もする予定
		// 反動実装は、cameraの注視点動かす感じでやるかも
		// Shakeクラス的なの作ってもよき
		//SceneManager::GetInstance().GetCamera()->GetShake()

	
		// 弾一発発射
		// 銃によって連射速度変えます
		// この処理はGun側でやる予定

		// 銃の回転中または初発の時撃たない
		if (!gun_->IsRotate() && preActUp_ == ACTION::SHOT  ||
			!gun_->IsRotate() && preAct_ == ACTION::JUMP_NOW)
		{ 
			gun_->Shot();

			if (gun_->IsShootable()) {
				// 反動
				recoil_->AddPower(gun_->GetRecoilMag());
				test_++;
			}

		}

		// 現在のアクション
		ChangeActionUp(ACTION::SHOT);
		animCon_->SetIsStop((int)ACTION::SHOT, false);

		// 銃状態変更
		ChangeGunState(GunBase::STATE::SHOT);


	}
	else
	{
		// 連続して撃った数をリセット
		gun_->ResetBurstNum();

		//recoil_->ResetPower();
	}

	isPutShotPre_ = controller_.IsPutShot();
}

void Player::ProcessADS(void)
{

	if (!gameManager_.IsStart()) return;

	if (!isAct_ && controller_.IsADS())
	{
		auto camera = SceneManager::GetInstance().GetCamera();
		auto gCamera = std::dynamic_pointer_cast<GameCamera>(camera);

		if (gCamera->GetMode((int)ID_) == GameCamera::MODE::FPS) {
			AudioManager::GetInstance().Play(
				AudioList::AUDIO_TYPE::PLAYER_ADS,
				transform_.pos,
				2500.0f,
				false,
				1.0f
			);
		}

		if (controller_.IsMe()) {
			gCamera->ChangeMode(GameCamera::MODE::ADS, GetPtr(), 0.0f, gun_->GetMagnification());
		}
		gCamera->ChangeMode((int)ID_, GameCamera::MODE::ADS, GetPtr(), 0.0f);

		// 現在のアクション
		ChangeActionUp(ACTION::SHOT);
		animCon_->SetIsStop((int)ACTION::SHOT, true);

		// 銃の回転
		ChangeGunState(GunBase::STATE::SHOT);
	}
	else
	{
		auto camera = SceneManager::GetInstance().GetCamera();
		auto gCamera = std::dynamic_pointer_cast<GameCamera>(camera);
		if (controller_.IsMe()) {
			gCamera->ChangeMode(GameCamera::MODE::FPS, GetPtr(), 0.5f);
		}
		gCamera->ChangeMode((int)ID_, GameCamera::MODE::FPS, GetPtr(), 0.5f);

		animCon_->SetIsStop((int)ACTION::SHOT, false);
	}
}

void Player::ProcessReload(void) 
{
	if (isAct_ ||
		gun_->GetMagazine() >= gun_->GetMaxMagazine()) return;

	

	auto& input = InputManager::GetInstance();

	if (controller_.IsReload())
	{
		ChangeActionUp(ACTION::RELOAD);

		// 銃状態変更
		ChangeGunState(GunBase::STATE::RELOAD);

		// アクション中かどうか
		isAct_ = true;
	}
}

void Player::ProcessGrenade(void)
{
	if (isAct_) return;

	auto& input = InputManager::GetInstance();



	if (actUp_ != ACTION::GRENADE)
	{

		if (controller_.IsGrenade())
		{
			if (!UseGrenade(greType_)) return;

			ChangeActionUp(ACTION::GRENADE);

			animCon_->SetIsStop((int)ACTION::GRENADE, false);
			animCon_->SetAnimTime((int)ACTION::GRENADE, 0.0f);
			animCon_->SetIsNotice((int)ACTION::GRENADE, false);

			// アクション中かどうか
			isAct_ = true;



		}
		else if (controller_.IsGrenadeChange())
		{
			ChangeNextGrenade();
		}
		else if (controller_.IsGrenadeInitiation())
		{
			for (auto& gre : gres_) {
				if (gre->GetType() == GRENADE_TYPE::STICKY) {
					gre->Blast();
				}
			}
		}
	}

	//if(input.IsNew(KEY_INPUT_G))pushGCnt_++;
	//else pushGCnt_ = 0;



	//if (input.IsClickMouseLeft() && isG)
	//{
	//	// 狙う
	//	isS = true;

	//	// ADS的な感じ
	//	// グレネードから放射線みたいなの出すかも
	//	// 予測線？

	//}
	//if (!input.IsClickMouseLeft() && isS)
	//{
	//	// 放したら投げる
	//	// グレネード投げる

	//	// 現在のアクション
	//	ChangeAction(ACTION::GRENADE);

	//	// 予測線通りに移動

	//	isG = false;	// グレネード準備解除
	//	isS = false;	// 狙うの解除
	//}

	//if (isG)
	//{
	//	act_ = ACTION::GRENADE;
	//}
}

void Player::UpdateCollider(void)
{
	VECTOR sendPos = {};
	Quaternion sendRot = {};

	// 頭のローカル行列を取得する
	auto mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, 5);
	auto pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す
	auto rot = MGetRotElem(mat);		// 行列から回転を取り出す
	sendPos = pos;
	sendRot = Quaternion::GetRotation(rot);
	sendPos = VAdd(sendPos,sendRot.PosAxis({ 0.0f,10.0f,0.0f }));
	colliders_[(int)BODY::HEAD]->Update(sendPos, sendRot);

	// 胸
	mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, 2);
	pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す
	rot = MGetRotElem(mat);		// 行列から回転を取り出す
	sendPos = pos;
	sendRot = Quaternion::GetRotation(rot);
	sendPos = VAdd(sendPos, sendRot.PosAxis({ 0.0f,10.0f,-5.0f }));
	colliders_[(int)BODY::CHEST]->Update(sendPos, sendRot);

	// 腰
	mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, 1);
	pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す
	rot = MGetRotElem(mat);		// 行列から回転を取り出す
	sendPos = pos;
	sendRot = Quaternion::GetRotation(rot);
	sendPos = VAdd(sendPos, sendRot.PosAxis({ 0.0f,-5.0f,-3.0f }));
	colliders_[(int)BODY::WAIST]->Update(sendPos, sendRot);

	// 左腕
	mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, 8);
	pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す
	rot = MGetRotElem(mat);		// 行列から回転を取り出す
	sendPos = pos;
	sendRot = Quaternion::GetRotation(rot);
	sendPos = VAdd(sendPos, sendRot.PosAxis({ 0.0f,10.0f,0.0f }));
	colliders_[(int)BODY::ARM_LEFT]->Update(sendPos, sendRot);

	// 左手首
	mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, 9);
	pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す
	rot = MGetRotElem(mat);		// 行列から回転を取り出す
	sendPos = pos;
	sendRot = Quaternion::GetRotation(rot);
	sendPos = VAdd(sendPos, sendRot.PosAxis({ 0.0f,15.0f,0.0f }));
	colliders_[(int)BODY::WRIST_LEFT]->Update(sendPos, sendRot);

	// 右腕
	mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, 32);
	pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す
	rot = MGetRotElem(mat);		// 行列から回転を取り出す
	sendPos = pos;
	sendRot = Quaternion::GetRotation(rot);
	sendPos = VAdd(sendPos, sendRot.PosAxis({ 0.0f,10.0f,0.0f }));
	colliders_[(int)BODY::ARM_RIGHT]->Update(sendPos, sendRot);

	// 右手首
	mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, 33);
	pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す
	rot = MGetRotElem(mat);		// 行列から回転を取り出す
	sendPos = pos;
	sendRot = Quaternion::GetRotation(rot);
	sendPos = VAdd(sendPos, sendRot.PosAxis({ 0.0f,15.0f,0.0f }));
	colliders_[(int)BODY::WRIST_RIGHT]->Update(sendPos, sendRot);

	// 左太もも
	mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, 55);
	pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す
	rot = MGetRotElem(mat);		// 行列から回転を取り出す
	sendPos = pos;
	sendRot = Quaternion::GetRotation(rot);
	sendPos = VAdd(sendPos, sendRot.PosAxis({ 0.0f,20.0f,0.0f }));
	colliders_[(int)BODY::THIGH_LEFT]->Update(sendPos, sendRot);

	// 左すね
	mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, 56);
	pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す
	rot = MGetRotElem(mat);		// 行列から回転を取り出す
	sendPos = pos;
	sendRot = Quaternion::GetRotation(rot);
	sendPos = VAdd(sendPos, sendRot.PosAxis({ 0.0f,20.0f,0.0f }));
	colliders_[(int)BODY::SHIN_LEFT]->Update(sendPos, sendRot);

	// 右太もも
	mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, 60);
	pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す
	rot = MGetRotElem(mat);		// 行列から回転を取り出す
	sendPos = pos;
	sendRot = Quaternion::GetRotation(rot);
	sendPos = VAdd(sendPos, sendRot.PosAxis({ 0.0f,20.0f,0.0f }));
	colliders_[(int)BODY::THIGH_RIGHT]->Update(sendPos, sendRot);

	// 右すね
	mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, 61);
	pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す
	rot = MGetRotElem(mat);		// 行列から回転を取り出す
	sendPos = pos;
	sendRot = Quaternion::GetRotation(rot);
	sendPos = VAdd(sendPos, sendRot.PosAxis({ 0.0f,20.0f,0.0f }));
	colliders_[(int)BODY::SHIN_RIGHT]->Update(sendPos, sendRot);

	// マップ衝突用ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー

	// カプセル
	sendPos = VAdd(transform_.pos, { 0.0f,90.0f,0.0f });
	sendRot = {};
	colliders_[(int)BODY::CAP]->Update(sendPos, sendRot);

	// 線分(頭上)
	sendPos = transform_.pos;
	sendRot = {};
	auto col = std::dynamic_pointer_cast<ColliderLine>(colliders_[(int)BODY::TOPLINE]);
	col->Update(
		{ sendPos.x,sendPos.y + 180.0f,sendPos.z },
		{ sendPos.x,sendPos.y + 150.0f,sendPos.z },
		sendRot);

	// 線分(足元)
	sendPos = transform_.pos;
	sendRot = {};
	col = std::dynamic_pointer_cast<ColliderLine>(colliders_[(int)BODY::DOWNLINE]);
	col->Update(
		{ sendPos.x,sendPos.y + 20.0f,sendPos.z },
		{ sendPos.x,sendPos.y - 15.0f,sendPos.z },
		sendRot);

	// ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー

	// 目線
	// 必要なのは更新後のカメラ位置と注視点
	auto camera = std::dynamic_pointer_cast<GameCamera>(SceneManager::GetInstance().GetCamera());
	auto cPos = camera->GetLatestPos((int)ID_);
	auto tPos = camera->GetLatestTargetPos((int)ID_);
	sendRot = {};
	col = std::dynamic_pointer_cast<ColliderLine>(colliders_[(int)BODY::EYE]);
	col->Update(cPos, tPos, sendRot);

	//射線
	auto center = GetCenterPos();
	VECTOR barrelPos;
	if (gun_ == nullptr) {
		barrelPos = *center;
	}
	else {
		barrelPos = gun_->GetBarrelPos();
	}
	
	sendRot = {};
	col = std::dynamic_pointer_cast<ColliderLine>(colliders_[(int)BODY::RAY]);
	col->Update(barrelPos, *center, sendRot);
}

void Player::UpdateRecoil(void)
{
	//	 腰のフレーム
	// 対象フレームのローカル行列(大きさ、回転、位置)を取得する
	auto fMat = MV1GetFrameLocalMatrix(transform_.modelId, 2);
	auto fScl = MGetSize(fMat);			// 行列から大きさを取り出す
	//auto fRotMat = MGetRotElem(fMat);		// 行列から回転を取り出す
	auto fPos = MGetTranslateElem(fMat);	// 行列から移動値を取り出す
	//VECTOR fRot = Quaternion::ToEuler(Quaternion::GetRotation(fRotMat));
	//VECTOR fRot = Quaternion::ToEuler(Quaternion::GetRotation(fRotMat));
	

	auto rotPow = recoil_->GetRecoilPowX();


	// X軸回転ーーーーーーーーーーーーーーーーーーーーーーーー
	// 腰回転
	waistRot_.x += rotPow;

	// 腰のX軸回転制限
	waistRot_.x = std::clamp(waistRot_.x, Utility::Deg2RadF(-70.0f), Utility::Deg2RadF(40.0f));

	// カメラ同期用回転
	cameraRot_.x -= rotPow;

	// カメラのX軸回転制限
	cameraRot_.x = std::clamp(cameraRot_.x, Utility::Deg2RadF(-50.0f), Utility::Deg2RadF(60.0f));


	// 腰
	// 大きさ、回転、位置をローカル行列に戻す
	MATRIX mix = MGetIdent();
	mix = MMult(mix, MGetScale(fScl));		// 大きさ
	MATRIX matRot = MGetIdent();			// 回転
	matRot = MMult(matRot, MGetRotX(waistRot_.x));
	matRot = MMult(matRot, MGetRotY(waistRot_.y));
	matRot = MMult(matRot, MGetRotZ(waistRot_.z));
	mix = MMult(mix, matRot);
	mix = MMult(mix, MGetTranslate(fPos));	// 座標
	// 合成した行列を対象フレームにセットし直す
	// 合成した行列を対象フレームにセットし直す
	MV1SetFrameUserLocalMatrix(transform_.modelId, 2, mix);

	// 座標回転など更新
	TransformUpdate();
}

bool Player::UseGrenade(GRENADE_TYPE type)
{
	if (greNum_.at(type) <= 0) return false;

	for (auto& g : gres_)
	{
		if (!g->IsEnd() ||
			g->GetType() != greType_) continue;


		// 再利用
		g->Hold();

		// 今手に持っているグレネード
		greInHand_ = g;

		// 所持数から減少
		greNum_.at(type)--;

		if (greNum_.at(type) <= 0)
		{
			// 所持数が0になったら次のグレネードへ
			ChangeNextGrenade();
		}
		
		return true;
	}


	// 新たに生成
	std::shared_ptr<GrenadeBase> temp = nullptr;
	switch (type)
	{
	case GRENADE_TYPE::HAND:
		temp  = std::make_shared<GrenadeHand>(transform_, 35, std::dynamic_pointer_cast<ActorBase>(GetPtr()));
		break;
	case GRENADE_TYPE::SMOKE:
		temp = std::make_shared<GrenadeSmoke>(transform_, 35, std::dynamic_pointer_cast<ActorBase>(GetPtr()));
		break;
	case GRENADE_TYPE::STICKY:
		temp = std::make_shared<GrenadeSticky>(transform_, 35, std::dynamic_pointer_cast<ActorBase>(GetPtr()));
		break;
	}

	if (temp == nullptr) return false;

	temp->Init();
	gres_.emplace_back(temp);

	// 今手に持っているグレネード
	greInHand_ = temp;

	// 所持数から減少
	greNum_.at(type)--;

	if (greNum_.at(type) <= 0)
	{
		// 所持数が0になったら次のグレネードへ
		ChangeNextGrenade();
	}

	return true;
}


void Player::HideHead(void)
{
	// 対象フレームのローカル行列(大きさ、回転、位置)を取得する
	auto mat = MV1GetFrameLocalMatrix(transform_.modelId, 5);
	auto scl = MGetSize(mat);			// 行列から大きさを取り出す
	auto rot = MGetRotElem(mat);		// 行列から回転を取り出す
	auto pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す

	// 大きさ、回転、位置をローカル行列に戻す
	MATRIX mix = MGetIdent();
	mix = MMult(mix, MGetScale({0.0f,0.0f,0.0f}));	// 大きさを0にする
	mix = MMult(mix, rot);							// 回転
	mix = MMult(mix, MGetTranslate(pos));			// 座標

	// 合成した行列を対象フレームにセットし直す
	MV1SetFrameUserLocalMatrix(transform_.modelId, 5, mix);
}

void Player::DisplayHead(void)
{
	// 対象フレームのローカル行列(大きさ、回転、位置)を取得する
	auto mat = MV1GetFrameLocalMatrix(transform_.modelId, 5);
	auto scl = MGetSize(mat);			// 行列から大きさを取り出す
	auto rot = MGetRotElem(mat);		// 行列から回転を取り出す
	auto pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す

	// 大きさ、回転、位置をローカル行列に戻す
	MATRIX mix = MGetIdent();
	mix = MMult(mix, MGetScale({ 1.0f,1.0f,1.0f }));	// 大きさを1にする
	mix = MMult(mix, rot);							// 回転
	mix = MMult(mix, MGetTranslate(pos));			// 座標

	// 合成した行列を対象フレームにセットし直す
	MV1SetFrameUserLocalMatrix(transform_.modelId, 5, mix);
}

void Player::TransformUpdate(void)
{
	// 移動回転大きさ等をモデルに反映
	transform_.Update();

	// コライダー座標更新
	UpdateCollider();

	// 銃座標更新
	gun_->UpdateTransform();
}
