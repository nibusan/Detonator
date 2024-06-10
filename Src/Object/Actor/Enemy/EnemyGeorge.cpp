#include "../../../Manager/CollisionManager.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/GameManager.h"
#include "../../../Manager/Camera/GameCamera.h"
#include "../../Common/ColliderCapsule.h"
#include "../../Common/ColliderSphere.h"
#include "../../Common/ColliderLine.h"
#include "../../Bullet/BulletBase.h"
#include "../../Grenade/GrenadeBase.h"
#include "../../Gun/GunEnemy.h"
#include "Armour.h"
#include "EnemyGeorge.h"

EnemyGeorge::EnemyGeorge(std::vector<VECTOR*> followPos, bool isFirst):followPos_(followPos)
{
}

void EnemyGeorge::EnemyReset(void)
{
}

const VECTOR& EnemyGeorge::GetFollowPos(void) const
{
	return *followPos_[0];
}

VECTOR EnemyGeorge::GetPosForCamera(void) const
{
	return VAdd(transform_.pos, { 0.0f,150.0f,0.0f });
}

void EnemyGeorge::AnimNotice(int id)
{
	if (id == (int)ACTION::DEAD)
	{
		animCon_->SetIsStop((int)ACTION::DEAD, true);
	}
}

void EnemyGeorge::OnHit(std::weak_ptr<Collider> obj)
{
}

void EnemyGeorge::OnHit(std::weak_ptr<Collider> objA, std::weak_ptr<Collider> objB)
{
	float rate = 1.0f;

	switch (objA.lock()->objType)
	{
	case Collider::OBJECT_TYPE::PLAYER_HEAD:
		rate = 1.5f;
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

		damage = bullet->GetDamage() * rate;

		lastAtkID_ = bullet->GetID();

		// 加害者(最後に攻撃してきたオブジェクト)
		lastAtkObj_ = bullet->GetParent();

		isNoticeDamage_ = true;
	}
	else if (objB.lock()->objType == Collider::OBJECT_TYPE::EXPLOSION) {

		auto grenade = dynamic_pointer_cast<GrenadeBase>(objB.lock()->parent_.lock());

		rate = 1.0f - (float)aliveShieldNum_ / (float)(SHIELD_NUM * ARMOUR_NUM);
		damage = grenade->GetDamage() * rate;

		lastAtkID_ = grenade->GetID();

		// 加害者(最後に攻撃してきたオブジェクト)
		lastAtkObj_ = grenade->GetParent();

		isNoticeDamage_ = true;
	}
	else if (objB.lock()->objType == Collider::OBJECT_TYPE::SMOKE) {
		return;
	}

	damageCareTime_.Set();
	

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
		GameManager::GetInstance().AddDamageData(data);
	}
}

void EnemyGeorge::OnHit(MV1_COLL_RESULT_POLY info)
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


		// 押し戻す方向ベクトル計算
		VECTOR vec = VNorm(VSub(pPos, nearPos));


		// プレイヤーをステージ上に移動
		// 半径取得
		auto col = std::dynamic_pointer_cast<ColliderCapsule>(colliders_[(int)BODY::CAP]);
		nearPos.y = transform_.pos.y;
		transform_.pos = VAdd(nearPos,
			VScale(vec, col->GetRadius() + 0.1f));
	}

	// 座標回転など更新
	//TransformUpdate();
}

void EnemyGeorge::OnHit(HITRESULT_LINE info)
{
	if (transform_.IsRising()) return;

	// プレイヤーをステージ上に移動
	transform_.pos = info.Position;

	// 重力加速度をリセット
	transform_.ResetGravityAccel();


	// 座標回転など更新
	//TransformUpdate();
}

void EnemyGeorge::OnHit(VECTOR normal)
{
	// 法線方向へ押し返す
	transform_.pos = VAdd(transform_.pos, VScale(normal, 0.5f));

	// 上方向に押し戻されたならジャンプや重力をリセットする
	if (normal.y == 1)
	{
		if (transform_.IsRising()) return;

		transform_.ResetGravityAccel();
	}

	// 下方向に押し戻されたならジャンプ力をリセットする
	if (normal.y == -1)
	{
		transform_.ResetGravityAccel();
	}


	// 座標回転など更新
	//TransformUpdate();
}

void EnemyGeorge::Init_Enemy(void)
{
	ID_ = ID::ENEMY_GEORGE;

	// モデルの読み込み
	model_ = std::make_unique<Model>(ResourceManager::GetInstance().GetResourceFile("Model_Enemy_Vanguard By T. Choonyung")->GetHandle());
	model_->SetIsAutoDeleteHandle(true);
	transform_.SetModel(model_->GetHandle());

	// デフォルトハンドル
	modelGrHandle_ = MV1GetTextureGraphHandle(transform_.modelId, 0);
	modelGrHandle2_ = MV1GetTextureGraphHandle(transform_.modelId, 3);
	// 強調用ハンドル
	colorGrHandle_ = LoadGraph("Data/Model/Enemy/Vanguard By T. Choonyung.fbm/red.png");

	// モデルの大きさ
	transform_.scl = { 1.5f,1.5f,1.5f };

	transform_.pos = {};

	// モデルの角度調整
	transform_.quaRotLocal = Quaternion::Euler({ 0.0f,Utility::Deg2RadF(180.0f),0.0f });
	transform_.quaRot = {};

	transform_.isGravity = true;
	transform_.heavy = 3.0f;
	transform_.height = 270.0f;

	transform_.Update();

	AnimationSetting();

	ColliderSetting();

	// コライダー座標更新
	UpdateCollider();

	// 現在のアクション
	act_ = ACTION::IDLE;
	preAct_ = act_;
	actUp_ = ACTION::IDLE;
	preActUp_ = actUp_;

	// 耐久力
	hp_ = hpMax_ = 1000.0f;

	gun_ = std::make_shared<GunEnemy>(transform_, 37, std::dynamic_pointer_cast<ActorBase>(GetPtr()));
	gun_->Init();

	// 銃の回転
	gun_->ChangeState(GunBase::STATE::NONE);
	gunState_ = GunBase::STATE::NONE;
	
	int i = 0;
	for (auto& armour : armours_) {
		armour = std::make_shared<Armour>();
		armour->SetShield(VAdd(transform_.pos, ARMOUR_RELATIVE_POS[i]),i % 2);
		i++;
	}

	aliveShieldNum_ = SHIELD_NUM * ARMOUR_NUM;
	
	waitTime_.Init(TARGET_WAIT_TIME, 1.0f / 60.0f);
	searchHearingTime_.Init(HEARING_SEARCH_TIME, 1.0f / 60.0f);
	searchViewTime_.Init(VIEW_TIME, 1.0f / 60.0f);
	damageCareTime_.Init(DAMAGE_CARE_TIME, 1.0f / 60.0f);

	targetIndex_ = 0;
	targetPos_ = TARGET_POS[targetIndex_];

	// 腰オイラー
	waistRot_ = {};

	// 検知フラグ
	isNotice_ = false;
	isNoticeHearing_ = false;
	isNoticeDamage_ = false;

	ChangeState(STATE::IDLE);
}

void EnemyGeorge::Update_Enemy(void)
{

	// 更新前情報更新
	transform_.PreUpdate();
	for (auto col : colliders_)
	{
		col->PreUpdate();
	}

	preAct_ = act_;
	preActUp_ = actUp_;

	if (act_ == ACTION::DEAD)
	{
		DeadUpdate();
		return;
	}

	//if (!isEscape_) {
		if (isNoticeDamage_ || state_ == STATE::SHOT) {
			SearchExpansionUpdate();
		}
		else {
			SearchUpdate();
		}
	//}

	
	CollisionManager::GetInstance().CollisionEnemyEyeMap();

	if (isNotice_) {
		ChangeState(STATE::SHOT);
	}
	else if (isNoticeHearing_) {
		ChangeState(STATE::SEARCH);
	}
	else if (isNoticeDamage_ && state_ != STATE::SEARCH && state_ != STATE::SHOT) {
		ChangeState(STATE::SEARCH);
	}

	//if (isEscape_) {
	//	ChangeState(STATE::RUN);
	//}

	ChangeGunState(GunBase::STATE::NONE);

	stateUpdate_();

	// アニメーション遷移
	animCon_->ChangeAnimation((int)act_);
	animCon_->ChangeAnimation((int)actUp_);

	// アニメーション更新
	animCon_->Update();

	transform_.Update();

	// コライダー座標更新
	UpdateCollider();

	// 銃の回転
	gun_->ChangeState(gunState_, 0.5f);

	// 銃座標更新
	gun_->Update();

	int i = 0;
	aliveShieldNum_ = 0;
	for (auto& armour : armours_) {
		armour->UpdateShield(VAdd(transform_.pos, ARMOUR_RELATIVE_POS[i]));
		aliveShieldNum_ += armour->GetAliveShieldNum();
		i++;
	}
}

void EnemyGeorge::Draw_Enemy(void)
{
	SetUseLighting(false);
	MV1SetTextureGraphHandle(transform_.modelId, 0, colorGrHandle_, 0);
	MV1SetTextureGraphHandle(transform_.modelId, 3, colorGrHandle_, 0);

	transform_.scl = { 1.55f,1.55f, 1.55f };
	auto camera = std::dynamic_pointer_cast<GameCamera>(SceneManager::GetInstance().GetCamera());
	auto dir = VNorm(VSub(transform_.pos, camera->GetPos()));
	transform_.pos = VAdd(transform_.pos,VScale(dir,5.0f));
	transform_.pos.y -= 5.0f;
	transform_.Update();
	MV1DrawModel(transform_.modelId);

	MV1SetTextureGraphHandle(transform_.modelId, 0, modelGrHandle_, 0);
	MV1SetTextureGraphHandle(transform_.modelId, 3, modelGrHandle2_, 0);
	SetUseLighting(true);

	transform_.scl = { 1.5f,1.5f,1.5f };
	transform_.pos = VSub(transform_.pos, VScale(dir, 5.0f));
	transform_.pos.y += 5.0f;
	transform_.Update();
	transform_.Draw();

	// 銃
	gun_->Draw();

	for (auto& armour : armours_) {
		armour->Draw();
	}

	//for (auto& col : colliders_) {
	//	col->DrawDebug();
	//}
	//for (int i = 0; i < (int)BODY::CAP; i++) {
	//	colliders_[i]->DrawDebug();
	//}
	//colliders_[(int)BODY::EYE]->DrawDebug();

	//if (!isNoticeDamage_) {
	//	// 聴覚
	//	DrawCone3D(
	//		VAdd(transform_.pos, { 0.0f,0.0f,0.0f }),
	//		VAdd(transform_.pos, { 0.0f,1.0f,0.0f }),
	//		HEARING_RANGE, 10, 0xfff7f, 0xfff7f, true);

	//	// 視覚
	//	float viewRad = Utility::Deg2RadF(VIEW_ANGLE);
	//	VECTOR localPos = { 0.0f,5.0f,VIEW_RANGE };
	//	// 回転行列
	//	MATRIX mat = MGetIdent();
	//	mat = MMult(mat, MGetRotY(transform_.rot.y));

	//	// 前方
	//	VECTOR forwardPos = VAdd(transform_.pos, VTransform(localPos, mat));

	//	// 右側
	//	MATRIX rightMat = MMult(mat, MGetRotY(viewRad));
	//	VECTOR rightPos = VAdd(transform_.pos, VTransform(localPos, rightMat));

	//	DrawTriangle3D({ transform_.pos.x,transform_.pos.y + 5,transform_.pos.z },
	//		forwardPos, rightPos, 0xffff00, true);

	//	// 左側
	//	MATRIX leftMat = MMult(mat, MGetRotY(-viewRad));
	//	VECTOR leftPos = VAdd(transform_.pos, VTransform(localPos, leftMat));

	//	DrawTriangle3D({ transform_.pos.x,transform_.pos.y + 5,transform_.pos.z },
	//		leftPos, forwardPos, 0xffff00, true);
	//}
	//else {
	//	// 聴覚
	//	DrawCone3D(
	//		VAdd(transform_.pos, { 0.0f,0.0f,0.0f }),
	//		VAdd(transform_.pos, { 0.0f,1.0f,0.0f }),
	//		HEARING_RANGE_EXPANSION, 10, 0xfff7f, 0xfff7f, true);

	//	// 視覚
	//	float viewRad = Utility::Deg2RadF(VIEW_ANGLE);
	//	VECTOR localPos = { 0.0f,5.0f,VIEW_RANGE_EXPANSION };
	//	// 回転行列
	//	MATRIX mat = MGetIdent();
	//	mat = MMult(mat, MGetRotY(transform_.rot.y));

	//	// 前方
	//	VECTOR forwardPos = VAdd(transform_.pos, VTransform(localPos, mat));

	//	// 右側
	//	MATRIX rightMat = MMult(mat, MGetRotY(viewRad));
	//	VECTOR rightPos = VAdd(transform_.pos, VTransform(localPos, rightMat));

	//	DrawTriangle3D({ transform_.pos.x,transform_.pos.y + 5,transform_.pos.z },
	//		forwardPos, rightPos, 0xffff00, true);

	//	// 左側
	//	MATRIX leftMat = MMult(mat, MGetRotY(-viewRad));
	//	VECTOR leftPos = VAdd(transform_.pos, VTransform(localPos, leftMat));

	//	DrawTriangle3D({ transform_.pos.x,transform_.pos.y + 5,transform_.pos.z },
	//		leftPos, forwardPos, 0xffff00, true);
	//}

}

void EnemyGeorge::Release_Enemy(void)
{
	transform_.Release();

	animCon_->Release();

	// 銃
	gun_->Release();

	for (auto& armour : armours_) {
		armour->Release();
	}

	DeleteGraph(modelGrHandle_);
	DeleteGraph(modelGrHandle2_);
	DeleteGraph(colorGrHandle_);
}

void EnemyGeorge::TransformUpdate(void)
{
	// 移動回転大きさ等をモデルに反映
	transform_.Update();

	// コライダー座標更新
	UpdateCollider();

	// 銃座標更新
	gun_->UpdateTransform();
}

void EnemyGeorge::AnimationSetting(void)
{
	// アニメーターコントローラ
	std::vector<int> array = { 3 };
	animCon_ = std::make_unique<AnimatorController>(std::dynamic_pointer_cast<ActorBase>(GetPtr()), transform_, array);

	// アニメーションによる座標移動無効化用座標
	VECTOR defPos = { 0.0f, 0.0f, 0.0f };

	// アニメーション
	int anim = -1;
	// 待機
	anim = MV1LoadModel("Data/Model/Enemy/Animation/Rifle Idle.mv1");
	animCon_->SetAnimData((int)ACTION::IDLE, anim, defPos);
	// 歩く
	anim = MV1LoadModel("Data/Model/Enemy/Animation/Walk With Rifle.mv1");
	animCon_->SetAnimData((int)ACTION::WALK, anim, defPos);
	// 走る
	anim = MV1LoadModel("Data/Model/Enemy/Animation/Rifle Run.mv1");
	animCon_->SetAnimData((int)ACTION::RUN, anim, defPos);
	// 撃つ
	anim = MV1LoadModel("Data/Model/Enemy/Animation/Firing Rifle.mv1");
	animCon_->SetAnimData((int)ACTION::SHOT, anim, defPos);
	// リロード
	anim = MV1LoadModel("Data/Model/Enemy/Animation/Reloading.mv1");
	animCon_->SetAnimData((int)ACTION::RELOAD, anim, defPos);
	// 死
	anim = MV1LoadModel("Data/Model/Enemy/Animation/Death From Front Headshot.mv1");
	animCon_->SetAnimData((int)ACTION::DEAD, anim, defPos);
	animCon_->SetNoticeTime((int)ACTION::DEAD, 170.0f);
}

void EnemyGeorge::ChangeAction(ACTION act)
{
	act_ = act;
}

void EnemyGeorge::ChangeActionUp(ACTION actUp)
{
	//if (isAct_) return;
	actUp_ = actUp;
}

void EnemyGeorge::ColliderSetting(void)
{
	// 頭
	colliders_[(int)BODY::HEAD] = std::make_shared<ColliderSphere>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_HEAD,
		22.5f);
	// 衝突管理クラスにセット
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::HEAD]);

	// 胸
	colliders_[(int)BODY::CHEST] = std::make_shared<ColliderCapsule>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_BODY,
		30.0f, 30.0f);
	// 衝突管理クラスにセット
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::CHEST]);

	// 腰
	colliders_[(int)BODY::WAIST] = std::make_shared<ColliderCapsule>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_BODY,
		30.0f, 15.0f);
	// 衝突管理クラスにセット
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::WAIST]);

	// 左腕
	colliders_[(int)BODY::ARM_LEFT] = std::make_shared<ColliderCapsule>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_LIMBS,
		15.0f, 22.5f);
	// 衝突管理クラスにセット
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::ARM_LEFT]);

	// 左手首
	colliders_[(int)BODY::WRIST_LEFT] = std::make_shared<ColliderCapsule>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_LIMBS,
		15.0f, 22.5f);
	// 衝突管理クラスにセット
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::WRIST_LEFT]);

	// 右腕
	colliders_[(int)BODY::ARM_RIGHT] = std::make_shared<ColliderCapsule>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_LIMBS,
		15.0f, 22.5f);
	// 衝突管理クラスにセット
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::ARM_RIGHT]);

	// 右手首
	colliders_[(int)BODY::WRIST_RIGHT] = std::make_shared<ColliderCapsule>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_LIMBS,
		15.0f, 22.5f);
	// 衝突管理クラスにセット
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::WRIST_RIGHT]);

	// 左太もも
	colliders_[(int)BODY::THIGH_LEFT] = std::make_shared<ColliderCapsule>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_LIMBS,
		18.0f, 35.0f);
	// 衝突管理クラスにセット
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::THIGH_LEFT]);

	// 左すね
	colliders_[(int)BODY::SHIN_LEFT] = std::make_shared<ColliderCapsule>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_LIMBS,
		15.0f, 45.0f);
	// 衝突管理クラスにセット
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::SHIN_LEFT]);

	// 右太もも
	colliders_[(int)BODY::THIGH_RIGHT] = std::make_shared<ColliderCapsule>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_LIMBS,
		18.0f, 35.0f);
	// 衝突管理クラスにセット
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::THIGH_RIGHT]);

	// 右すね
	colliders_[(int)BODY::SHIN_RIGHT] = std::make_shared<ColliderCapsule>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_LIMBS,
		15.0f, 45.0f);
	// 衝突管理クラスにセット
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::SHIN_RIGHT]);

	// マップ衝突用ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー

	// カプセル
	colliders_[(int)BODY::CAP] = std::make_shared<ColliderCapsule>(GetPtr(),
		Collider::OBJECT_TYPE::PLAYER_CAP,
		60.0f, 180.0f);
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
		Collider::OBJECT_TYPE::ENEMY_EYE);
	// 衝突管理クラスにセット
	CollisionManager::GetInstance().SetCollider(colliders_[(int)BODY::EYE]);
}

void EnemyGeorge::UpdateCollider(void)
{
	VECTOR sendPos = {};
	Quaternion sendRot = {};

	// 頭のローカル行列を取得する
	auto mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, 7);
	auto pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す
	auto rot = MGetRotElem(mat);		// 行列から回転を取り出す
	sendPos = pos;
	sendRot = Quaternion::GetRotation(rot);
	sendPos = VAdd(sendPos, sendRot.PosAxis({ 0.0f,15.0f,0.0f }));
	colliders_[(int)BODY::HEAD]->Update(sendPos, sendRot);

	// 胸
	mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, 4);
	pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す
	rot = MGetRotElem(mat);		// 行列から回転を取り出す
	sendPos = pos;
	sendRot = Quaternion::GetRotation(rot);
	sendPos = VAdd(sendPos, sendRot.PosAxis({ 0.0f,10.0f,-7.5f }));
	colliders_[(int)BODY::CHEST]->Update(sendPos, sendRot);

	// 腰
	mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, 3);
	pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す
	rot = MGetRotElem(mat);		// 行列から回転を取り出す
	sendPos = pos;
	sendRot = Quaternion::GetRotation(rot);
	sendPos = VAdd(sendPos, sendRot.PosAxis({ 0.0f,-7.5f,-4.5f }));
	colliders_[(int)BODY::WAIST]->Update(sendPos, sendRot);

	// 左腕
	mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, 10);
	pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す
	rot = MGetRotElem(mat);		// 行列から回転を取り出す
	sendPos = pos;
	sendRot = Quaternion::GetRotation(rot);
	sendPos = VAdd(sendPos, sendRot.PosAxis({ 0.0f,15.0f,0.0f }));
	colliders_[(int)BODY::ARM_LEFT]->Update(sendPos, sendRot);

	// 左手首
	mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, 11);
	pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す
	rot = MGetRotElem(mat);		// 行列から回転を取り出す
	sendPos = pos;
	sendRot = Quaternion::GetRotation(rot);
	sendPos = VAdd(sendPos, sendRot.PosAxis({ 0.0f,15.0f,0.0f }));
	colliders_[(int)BODY::WRIST_LEFT]->Update(sendPos, sendRot);

	// 右腕
	mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, 34);
	pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す
	rot = MGetRotElem(mat);		// 行列から回転を取り出す
	sendPos = pos;
	sendRot = Quaternion::GetRotation(rot);
	sendPos = VAdd(sendPos, sendRot.PosAxis({ 0.0f,15.0f,0.0f }));
	colliders_[(int)BODY::ARM_RIGHT]->Update(sendPos, sendRot);

	// 右手首
	mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, 35);
	pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す
	rot = MGetRotElem(mat);		// 行列から回転を取り出す
	sendPos = pos;
	sendRot = Quaternion::GetRotation(rot);
	sendPos = VAdd(sendPos, sendRot.PosAxis({ 0.0f,15.0f,0.0f }));
	colliders_[(int)BODY::WRIST_RIGHT]->Update(sendPos, sendRot);

	// 左太もも
	mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, 62);
	pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す
	rot = MGetRotElem(mat);		// 行列から回転を取り出す
	sendPos = pos;
	sendRot = Quaternion::GetRotation(rot);
	sendPos = VAdd(sendPos, sendRot.PosAxis({ 0.0f,40.0f,0.0f }));
	colliders_[(int)BODY::THIGH_LEFT]->Update(sendPos, sendRot);

	// 左すね
	mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, 63);
	pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す
	rot = MGetRotElem(mat);		// 行列から回転を取り出す
	sendPos = pos;
	sendRot = Quaternion::GetRotation(rot);
	sendPos = VAdd(sendPos, sendRot.PosAxis({ 0.0f,40.0f,0.0f }));
	colliders_[(int)BODY::SHIN_LEFT]->Update(sendPos, sendRot);

	// 右太もも
	mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, 57);
	pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す
	rot = MGetRotElem(mat);		// 行列から回転を取り出す
	sendPos = pos;
	sendRot = Quaternion::GetRotation(rot);
	sendPos = VAdd(sendPos, sendRot.PosAxis({ 0.0f,40.0f,0.0f }));
	colliders_[(int)BODY::THIGH_RIGHT]->Update(sendPos, sendRot);

	// 右すね
	mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, 58);
	pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す
	rot = MGetRotElem(mat);		// 行列から回転を取り出す
	sendPos = pos;
	sendRot = Quaternion::GetRotation(rot);
	sendPos = VAdd(sendPos, sendRot.PosAxis({ 0.0f,40.0f,0.0f }));
	colliders_[(int)BODY::SHIN_RIGHT]->Update(sendPos, sendRot);

	// マップ衝突用ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー

	// カプセル
	sendPos = VAdd(transform_.pos, { 0.0f,135.0f,0.0f });
	sendRot = {};
	colliders_[(int)BODY::CAP]->Update(sendPos, sendRot);

	// 線分(頭上)
	sendPos = transform_.pos;
	sendRot = {};
	auto col = std::dynamic_pointer_cast<ColliderLine>(colliders_[(int)BODY::TOPLINE]);
	col->Update(
		{ sendPos.x,sendPos.y + 270.0f,sendPos.z },
		{ sendPos.x,sendPos.y + 225.0f,sendPos.z },
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
	// エネミーとプレイヤーの線
	mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, 7);
	pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す
	sendPos = pos;
	sendRot = {};
	auto cameraPos = SceneManager::GetInstance().GetCamera()->GetPos();
	col = std::dynamic_pointer_cast<ColliderLine>(colliders_[(int)BODY::EYE]);
	col->Update(sendPos, cameraPos, sendRot);
}

std::shared_ptr<GunBase> EnemyGeorge::GetGun(void)
{
	return gun_;
}

void EnemyGeorge::ProcessDead(void)
{
	auto& gMng = GameManager::GetInstance();
	gMng.AddScore(10000, (int)lastAtkID_);
	gMng.Kill(ID_, lastAtkID_);
	gMng.AddElimination(ID_);

	animCon_->Reset();
	animCon_->SetValidAnimMove(true);
	// フレーム回転リセット
	MV1ResetFrameUserLocalMatrix(transform_.modelId, 0);

	animCon_->SetIsStop((int)ACTION::DEAD, false);
	//isAct_ = false;
	ChangeAction(ACTION::DEAD);
	ChangeActionUp(ACTION::DEAD);
	// アニメーション遷移
	animCon_->ChangeAnimation((int)act_);
	animCon_->ChangeAnimation((int)actUp_);

	// 銃の回転
	gun_->ChangeState(GunBase::STATE::RELOAD, 0.5f);
}

void EnemyGeorge::DeadUpdate(void)
{
	// 重力
	transform_.Gravity();

	// アニメーション遷移
	animCon_->ChangeAnimation((int)act_);
	animCon_->ChangeAnimation((int)actUp_);

	// アニメーション更新
	animCon_->Update();

	// 銃座標更新
	gun_->Update();

	// 座標回転など更新
	TransformUpdate();
}

void EnemyGeorge::Update_EnemyAfterCollision(void)
{

}

void EnemyGeorge::OnHit()
{
	// 検知フラグ
	isNotice_ = false;
	isNoticeHearing_ = false;
}

void EnemyGeorge::ChangeState(STATE state)
{
	preState_ = state_;
	state_ = state;
	switch (state_)
	{
	case EnemyGeorge::STATE::IDLE:
		stateUpdate_ = std::bind(&EnemyGeorge::Idle, this);
		waitTime_.Set();

		// 次に目的とする場所を決める
		if (preState_ != STATE::SEARCH &&
			preState_ != STATE::SHOT) {
			targetIndex_++;
		}
		
		if (targetIndex_ >= TARGET_NUM) {
			targetIndex_ = 0;
		}
		targetPos_ = TARGET_POS[targetIndex_];
		targetGoalRot_ = Quaternion::ToEuler(
			Quaternion::LookRotation(VNorm(VSub(TARGET_POS[targetIndex_], transform_.pos)))
		);
		break;
	case EnemyGeorge::STATE::ROTATE:
		stateUpdate_ = std::bind(&EnemyGeorge::Rotation, this);
		isRotate_ = true;
		break;
	case EnemyGeorge::STATE::MOVE:
		stateUpdate_ = std::bind(&EnemyGeorge::Move, this);
		break;
	case EnemyGeorge::STATE::SHOT:
		stateUpdate_ = std::bind(&EnemyGeorge::Shot, this);
		searchViewTime_.Set();
		break;
	case EnemyGeorge::STATE::SEARCH:
		stateUpdate_ = std::bind(&EnemyGeorge::Search, this);
		searchHearingTime_.Set();
		break;
	}
}

void EnemyGeorge::ChangeGunState(GunBase::STATE state)
{
	gunState_ = state;
}

void EnemyGeorge::SearchUpdate(void)
{
	// 検知フラグリセット
	isNotice_ = false;
	isNoticeHearing_ = false;

	// プレイヤーの座標を取得
	VECTOR pPos = *followPos_[0];

	// エネミーからプレイヤーまでのベクトル
	VECTOR diff = VSub(pPos, transform_.pos);

	// 聴覚
	if ((diff.z * diff.z + diff.x * diff.x + diff.y * diff.y) <= HEARING_RANGE * HEARING_RANGE)
	{
		isNoticeHearing_ = true;
	}

	// エネミーの向いている向き
	float degE = Utility::DegIn360(
		Utility::Rad2DegF(transform_.rot.y));

	// エネミーが向いている向きを0度としたときのプレイヤーの角度
	float degP = Utility::DegIn360(
		Utility::Rad2DegF(
			atan2f(diff.x, diff.z)));

	degP = Utility::DegIn360(degP - degE);

	// 視野範囲
	float degRangeL = 360.0f - VIEW_ANGLE;
	float degRangeR = 0.0f + VIEW_ANGLE;

	if (degRangeL <= degP || degP <= degRangeR)
	{
		if ((diff.z * diff.z + diff.x * diff.x + diff.y * diff.y)
			<= VIEW_RANGE * VIEW_RANGE)
		{
			isNotice_ = true;
		}
	}


}

void EnemyGeorge::SearchExpansionUpdate(void)
{
	// 検知フラグリセット
	isNotice_ = false;
	isNoticeHearing_ = false;

	// プレイヤーの座標を取得
	VECTOR pPos = *followPos_[0];

	// エネミーからプレイヤーまでのベクトル
	VECTOR diff = VSub(pPos, transform_.pos);

	// 聴覚
	if ((diff.z * diff.z + diff.x * diff.x + diff.y * diff.y) <= HEARING_RANGE_EXPANSION * HEARING_RANGE_EXPANSION)
	{
		isNoticeHearing_ = true;
	}

	// エネミーの向いている向き
	float degE = Utility::DegIn360(
		Utility::Rad2DegF(transform_.rot.y));

	// エネミーが向いている向きを0度としたときのプレイヤーの角度
	float degP = Utility::DegIn360(
		Utility::Rad2DegF(
			atan2f(diff.x, diff.z)));

	degP = Utility::DegIn360(degP - degE);

	// 視野範囲
	float degRangeL = 360.0f - VIEW_ANGLE;
	float degRangeR = 0.0f + VIEW_ANGLE;

	if (degRangeL <= degP || degP <= degRangeR)
	{
		if ((diff.z * diff.z + diff.x * diff.x + diff.y * diff.y)
			<= VIEW_RANGE_EXPANSION * VIEW_RANGE_EXPANSION)
		{
			isNotice_ = true;
		}
	}
}

void EnemyGeorge::Idle(void)
{
	ChangeAction(ACTION::IDLE);
	ChangeActionUp(ACTION::IDLE);
	WaistRotate(0.0f);

	if (waitTime_.Update()) {
		ChangeState(STATE::ROTATE);
	}

	if (damageCareTime_.IsCount()) {
		if (waitTime_.Update()) {
			ChangeState(STATE::ROTATE);
		}
	}
}

void EnemyGeorge::Rotation(void)
{
	ChangeAction(ACTION::IDLE);
	ChangeActionUp(ACTION::IDLE);

	LazyRotationY(targetGoalRot_.y);
	WaistRotate(0.0f);
	if (!isRotate_) {
		ChangeState(STATE::MOVE);
	}
}

void EnemyGeorge::LazyRotationY(float goalRotY)
{
	// 回転処理
	float degNowAngleY = Utility::Rad2DegF(transform_.rot.y);
	float degGoalAngleY = Utility::Rad2DegF(goalRotY);

	// 0度～360度以内に角度をおさめる
	degGoalAngleY = static_cast<float>(Utility::DegIn360(degGoalAngleY));
	degNowAngleY = static_cast<float>(Utility::DegIn360(degNowAngleY));

	// 回転が少ない方の回転向きを取得する(時計回り:1、反時計回り:-1)
	int aroundDirY = Utility::DirNearAroundDeg(degNowAngleY, degGoalAngleY);

	// 到達したい角度に回転を加える
	if (degNowAngleY <= degGoalAngleY + 5.0f &&
		degNowAngleY >= degGoalAngleY - 5.0f)
	{
		transform_.rot.y = goalRotY;
		isRotate_ = false;
	}
	else
	{
		if (!damageCareTime_.IsCount()) {
			transform_.rot.y += ROTATE_POW * static_cast<float>(aroundDirY);
		}else {
			transform_.rot.y += ROTATE_POW * 3.0f * static_cast<float>(aroundDirY);
		}
		
	}



	// 0度～360度以内に角度をおさめる
	transform_.rot.y = static_cast<float>(Utility::RadIn2PI(transform_.rot.y));

	// クォータニオンに変換
	transform_.quaRot = Quaternion::Euler(transform_.rot);
}

void EnemyGeorge::WaistRotate(float goalRotX)
{
	// 腰のフレーム
	// 対象フレームのローカル行列(大きさ、回転、位置)を取得する
	auto fMat = MV1GetFrameLocalMatrix(transform_.modelId, 3);
	auto fScl = MGetSize(fMat);			// 行列から大きさを取り出す
	auto fPos = MGetTranslateElem(fMat);	// 行列から移動値を取り出す

	// 腰回転
	LazyRotationX(goalRotX);

	//if (waistRot_.x == 0.0f) return;

	//// 腰のX軸回転制限
	//waistRot_.x = std::clamp(waistRot_.x, Utility::Deg2RadF(-70.0f), Utility::Deg2RadF(40.0f));

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
	MV1SetFrameUserLocalMatrix(transform_.modelId, 3, mix);

	//TransformUpdate();
}

void EnemyGeorge::Move(void)
{

	if (damageCareTime_.Update()) {
		ChangeAction(ACTION::WALK);
		ChangeActionUp(ACTION::WALK);
		WaistRotate(0.0f);

		transform_.pos = VAdd(transform_.pos, VScale(transform_.quaRot.GetForward(), 2.0f));

		if (Utility::Distance(transform_.pos, targetPos_) <= 2.0f) {
			transform_.pos = targetPos_;
			ChangeState(STATE::IDLE);
		}
	}
	else {
		ChangeAction(ACTION::RUN);
		ChangeActionUp(ACTION::RUN);
		WaistRotate(0.0f);

		transform_.pos = VAdd(transform_.pos, VScale(transform_.quaRot.GetForward(), 7.0f));

		if (Utility::Distance(transform_.pos, targetPos_) <= 7.0f) {
			transform_.pos = targetPos_;
			ChangeState(STATE::IDLE);
		}
	}

}

void EnemyGeorge::Shot(void)
{
	if (GameManager::GetInstance().GetPlayerInfo(0).isHide){
		isNoticeDamage_ = false;
		ChangeAction(ACTION::IDLE);
		ChangeActionUp(ACTION::IDLE);
		ChangeState(STATE::IDLE);
		return;
	}


	auto follow = *followPos_[0];
	auto now = transform_.pos;
	auto target = Quaternion::LookRotation(VNorm(VSub(follow, now)));

	follow.y = 0.0f;
	now.y = 0.0f;
	transform_.quaRot = Quaternion::LookRotation(VNorm(VSub(follow, now)));
	transform_.quaRot.Mult(Quaternion::Euler({ 0.0f,Utility::Deg2RadF(30.0f),0.0f }));

	//transform_.Update();
	//follow = *followPos_[0];
	now = transform_.pos;
	now.y += 350.0f;
	follow = *followPos_[0];

	target = Quaternion::LookRotation(VNorm(VSub(follow, now)));


	WaistRotate(-target.ToEuler().x);


	if (isNotice_) {
		ChangeAction(ACTION::SHOT);
		ChangeActionUp(ACTION::SHOT);
		ChangeGunState(GunBase::STATE::SHOT);
		if (!gun_->IsRotate() && preActUp_ == ACTION::SHOT) {
			gun_->Shot();
		}
	}
	else {
		ChangeAction(ACTION::IDLE);
		ChangeActionUp(ACTION::IDLE);
		ChangeGunState(GunBase::STATE::NONE);
	}
	

	if (searchViewTime_.Update()) {
		isNoticeDamage_ = false;
		ChangeState(STATE::IDLE);
	}
}

void EnemyGeorge::Search(void)
{
	ChangeAction(ACTION::IDLE);
	ChangeActionUp(ACTION::IDLE);

	auto goal = Quaternion::ToEuler(
		Quaternion::LookRotation(VNorm(VSub(*followPos_[0], transform_.pos)))
	);
	LazyRotationY(goal.y);

	if (searchHearingTime_.Update()) {
		if (isNoticeDamage_) {
			isNoticeDamage_ = false;
			//isEscape_ = true;
			return;
		}

		isNoticeDamage_ = false;
		ChangeState(STATE::IDLE);
	}
}

void EnemyGeorge::LazyRotationX(float goalRotX)
{
	// 回転処理
	float degNowAngleX = Utility::Rad2DegF(waistRot_.x);
	float degGoalAngleX = Utility::Rad2DegF(goalRotX);

	// 0度～360度以内に角度をおさめる
	degGoalAngleX = static_cast<float>(Utility::DegIn360(degGoalAngleX));
	degNowAngleX = static_cast<float>(Utility::DegIn360(degNowAngleX));

	// 回転が少ない方の回転向きを取得する(時計回り:1、反時計回り:-1)
	int aroundDirX = Utility::DirNearAroundDeg(degNowAngleX, degGoalAngleX);

	// 到達したい角度に回転を加える
	if (degNowAngleX <= degGoalAngleX + 5.0f &&
		degNowAngleX >= degGoalAngleX - 5.0f)
	{
		waistRot_.x = goalRotX;
	}
	else
	{
		waistRot_.x += ROTATE_POW * static_cast<float>(aroundDirX);
	}


	// 0度～360度以内に角度をおさめる
	waistRot_.x = static_cast<float>(Utility::RadIn2PI(waistRot_.x));
}
