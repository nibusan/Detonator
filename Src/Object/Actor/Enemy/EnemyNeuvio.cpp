#include <cmath>
#include <algorithm>
#include "../../../Utility/Utility.h"
#include "../../../Application.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/CollisionManager.h"
#include "../../../Manager/GameManager.h"
#include "../../../Manager/EffectManager.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/Camera/GameCamera.h"
#include "../../ObjectBase.h"
#include "../../Actor/Player/Player.h"
#include "../../Common/ColliderSphere.h"
#include "../../Common/ColliderLine.h"
#include "../../Bullet/BulletEnemy.h"
#include "EnemyNeuvio.h"

EnemyNeuvio::EnemyNeuvio(std::vector<VECTOR*> followPos, bool isFirst) : followPos_(followPos), audioManager_(AudioManager::GetInstance())
{
	isFirst_ = isFirst;


}

EnemyNeuvio::~EnemyNeuvio(void)
{
}

void EnemyNeuvio::OnHit(std::weak_ptr<Collider> obj)
{
	if (obj.lock()->objType == Collider::OBJECT_TYPE::PLAYER_BULLET)
	{
		auto bullet = std::dynamic_pointer_cast<BulletBase>(obj.lock()->parent_.lock());
		lastAtkID_ = obj.lock()->parent_.lock()->GetID();
		Damage(bullet->GetDamage());


		audioManager_.Play(
			AudioList::AUDIO_TYPE::ENEMY_DAMAGE,
			transform_.pos,
			5500.0f,
			false,
			0.8f
		);

		auto id = obj.lock()->parent_.lock()->GetID();
		if ((int)NetManager::GetInstance().GetSelf().playerType == (int)id) {
			GameManager::DamageData data;
			data.damage_ = bullet->GetDamage();
			data.pos = collider_->pos;
			data.pos.x += (-10 + GetRand(20));
			data.pos.y += (-10 + GetRand(20));
			data.time_ = 1.0f;
			data.color_ = { 255,0,0 };
			GameManager::GetInstance().AddDamageData(data);
		}
		//AudioManager::GetInstance().Play(AudioList::AUDIO_TYPE::ENEMY_DAMAGE,transform_.pos, 500.0f);
	}

	if (obj.lock()->objType == Collider::OBJECT_TYPE::EXPLOSION)
	{
		auto grenade = std::dynamic_pointer_cast<GrenadeBase>(obj.lock()->parent_.lock());
		lastAtkID_ = obj.lock()->parent_.lock()->GetID();
		Damage(grenade->GetDamage());

		auto id = obj.lock()->parent_.lock()->GetID();
		if ((int)NetManager::GetInstance().GetSelf().playerType == (int)id) {
			GameManager::DamageData data;
			data.damage_ = grenade->GetDamage();
			data.pos = collider_->pos;
			data.pos.x += (-10 + GetRand(20));
			data.pos.y += (-10 + GetRand(20));
			data.time_ = 1.0f;
			data.color_ = { 255,0,0 };
			GameManager::GetInstance().AddDamageData(data);
		}
	}

	if (obj.lock()->objType == Collider::OBJECT_TYPE::MAP_UNBREAK || obj.lock()->objType == Collider::OBJECT_TYPE::MAP)
	{

		if (collider_->objType == Collider::OBJECT_TYPE::EXPLOSION) return;

		// 当たり判定フラグ
		isCollision_ = true;

		// 壁をよける
		transform_.pos.y++;

		// 衝突時の位置
		VECTOR collPos;

		switch (action_)
		{
		case EnemyNeuvio::ACTION::FIXED_TURRET:
			break;
		case EnemyNeuvio::ACTION::FOLLOW:
			break;
		case EnemyNeuvio::ACTION::PASS:
			break;
		case EnemyNeuvio::ACTION::RUSH:
			if (!isRush_ || isRotate_)return;
			collPos = transform_.pos;
			transform_.isHide = true;
			debugFlg_ = true;

			// 元のオブジェクトタイプ配列から要素を消しておく
			CollisionManager::GetInstance().DeleteCollider(collider_);

			// オブジェクトタイプを変えて再びセット
			collider_->ChangeObjectType(Collider::OBJECT_TYPE::EXPLOSION);
			CollisionManager::GetInstance().SetCollider(collider_);
			break;
		}

		// 追尾中は処理しない
		if (isNotice_ || isNoticeHearing_)return;

		// 中継地点切り替え
		switch (point_)
		{
		case EnemyNeuvio::POINT::A:
			pointB_ = { static_cast<float>(B_ENEMY_RESPAWN_POS_X + GetRand(F_ENEMY_RESPAWN_POS_X)) , 700.0f, static_cast<float>(B_ENEMY_RESPAWN_POS_Z + GetRand(F_ENEMY_RESPAWN_POS_Z)) };
			isCollision_ = false;
			break;
		case EnemyNeuvio::POINT::B:
			pointC_ = { static_cast<float>(B_ENEMY_RESPAWN_POS_X + GetRand(F_ENEMY_RESPAWN_POS_X)) , 700.0f, static_cast<float>(B_ENEMY_RESPAWN_POS_Z + GetRand(F_ENEMY_RESPAWN_POS_Z)) };
			isCollision_ = false;
			break;
		case EnemyNeuvio::POINT::C:
			pointA_ = { static_cast<float>(B_ENEMY_RESPAWN_POS_X + GetRand(F_ENEMY_RESPAWN_POS_X)) , 700.0f, static_cast<float>(B_ENEMY_RESPAWN_POS_Z + GetRand(F_ENEMY_RESPAWN_POS_Z)) };
			isCollision_ = false;
			break;
		}

	}




}

void EnemyNeuvio::OnHit(MV1_COLL_RESULT_POLY info)
{
	
}

void EnemyNeuvio::EnemyReset(void)
{

	// 元のオブジェクトタイプ配列から要素を消しておく
	CollisionManager::GetInstance().DeleteCollider(collider_);

	// オブジェクトタイプを変えて再びセット
	collider_->ChangeObjectType(Collider::OBJECT_TYPE::ENEMY);
	CollisionManager::GetInstance().SetCollider(collider_);

	// 半径をデフォルトに
	auto col = std::dynamic_pointer_cast<ColliderSphere>(collider_);
	col->SetRadius(COL_RAD);

	// モデルの位置
	transform_.pos = { 0.0f,3000.0f,0.0f };//3000y

	// 敵機の中継地点
	point_ = POINT::A;

	// 耐久力
	hp_ = 50.0f;

	// 当たり判定
	isCollision_ = false;

	collider_->isCollision = false;

	isFirstCol_ = false;

	// 表示
	transform_.isHide = false;

	// 弾の発射位置
	// 対象フレームのワールド行列(大きさ、回転、位置)を取得する
	auto mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, barrelFrameNo_);
	auto pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す
	barrelPos_ = pos;

	// 発射間隔
	shotDelay_ = 0.1f;

	// 敵機の中継地点初期化
	pointA_ = { static_cast<float>(B_ENEMY_RESPAWN_POS_X + GetRand(F_ENEMY_RESPAWN_POS_X)) , static_cast<float>(B_ENEMY_RESPAWN_POS_Y + GetRand(F_ENEMY_RESPAWN_POS_Y)), static_cast<float>(B_ENEMY_RESPAWN_POS_Z + GetRand(F_ENEMY_RESPAWN_POS_Z)) };
	pointB_ = { static_cast<float>(B_ENEMY_RESPAWN_POS_X + GetRand(F_ENEMY_RESPAWN_POS_X)) , static_cast<float>(B_ENEMY_RESPAWN_POS_Y + GetRand(F_ENEMY_RESPAWN_POS_Y)) , static_cast<float>(B_ENEMY_RESPAWN_POS_Z + GetRand(F_ENEMY_RESPAWN_POS_Z)) };
	pointC_ = { static_cast<float>(B_ENEMY_RESPAWN_POS_X + GetRand(F_ENEMY_RESPAWN_POS_X)) , static_cast<float>(B_ENEMY_RESPAWN_POS_Y + GetRand(F_ENEMY_RESPAWN_POS_Y)) , static_cast<float>(B_ENEMY_RESPAWN_POS_Z + GetRand(F_ENEMY_RESPAWN_POS_Z)) };

	// 回転中かどうか
	isRotate_ = false;

	// 突進中かどうか
	isRush_ = false;

	// 相対座標
	//transform_.localPos = VAdd(transform_.pos, LOCAL_COL_POS);

	// 突進中の向き
	rushDir_ = {};

	deg_ = 0.0f;

	// デバッグ用
	debugFlg_ = false;

	// 行動処理
	EnemyAction();

	//soundHandle_ = audioManager_.Play(
	//	AudioList::AUDIO_TYPE::PLAYER_DEATH3,
	//	transform_.pos,
	//	500.0f,
	//	true,
	//	1.0f
	//);
}

void EnemyNeuvio::Init_Enemy(void)
{

	// 敵機の中継地点初期化
	pointA_ = { static_cast<float>(B_ENEMY_RESPAWN_POS_X + GetRand(F_ENEMY_RESPAWN_POS_X)) , static_cast<float>(200 + GetRand(F_ENEMY_RESPAWN_POS_Y)), static_cast<float>(B_ENEMY_RESPAWN_POS_Z + GetRand(F_ENEMY_RESPAWN_POS_Z)) };
	pointB_ = { static_cast<float>(B_ENEMY_RESPAWN_POS_X + GetRand(F_ENEMY_RESPAWN_POS_X)) , static_cast<float>(B_ENEMY_RESPAWN_POS_Y + GetRand(F_ENEMY_RESPAWN_POS_Y)) , static_cast<float>(B_ENEMY_RESPAWN_POS_Z + GetRand(F_ENEMY_RESPAWN_POS_Z)) };
	pointC_ = { static_cast<float>(B_ENEMY_RESPAWN_POS_X + GetRand(F_ENEMY_RESPAWN_POS_X)) , static_cast<float>(B_ENEMY_RESPAWN_POS_Y + GetRand(F_ENEMY_RESPAWN_POS_Y)) , static_cast<float>(B_ENEMY_RESPAWN_POS_Z + GetRand(F_ENEMY_RESPAWN_POS_Z)) };
	// モデルの読み込み
	model_ = std::make_unique<Model>(ResourceManager::GetInstance().Get("Model_Enemy_robot_metallic")->GetHandle());
	model_->SetIsAutoDeleteHandle(true);
	transform_.SetModel(model_->GetHandle());/*
	transform_.modelId = MV1LoadModel("Data/Model/Enemy/robot_metallic.mv1");*/

	//// 弾モデルの読み込み
	//bulletModelId_ = MV1LoadModel("Data/Model/Bullet/ar.mv1");

	// モデルの大きさ
	transform_.scl = { 0.1f,0.2f,0.2f };

	// モデルの位置
	if (isFirst_)
	{
		transform_.pos = pointA_;
	}

	// モデルの角度調整
	transform_.quaRotLocal = Quaternion::Euler({ 0.0f,Utility::Deg2RadF(180.0f),0.0f });

	// 当たり判定用
	collider_ = std::make_shared<ColliderSphere>(GetPtr(), Collider::OBJECT_TYPE::ENEMY, COL_RAD);
	CollisionManager::GetInstance().SetCollider(collider_);

	colliderEye_ = std::make_shared<ColliderLine>(GetPtr(), Collider::OBJECT_TYPE::ENEMY_EYE);
	CollisionManager::GetInstance().SetCollider(colliderEye_);

	// 敵機の中継地点
	point_ = POINT::A;

	// 突進対象の位置
	followRushPos_ = {};

	// 耐久力
	hp_ = 50.0f;

	// 当たり判定
	isCollision_ = false;

	collider_->isCollision = false;

	isFirstCol_ = false;

	// 表示
	transform_.isHide = false;

	// 弾の発射位置
	// 対象フレームのワールド行列(大きさ、回転、位置)を取得する
	auto mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, barrelFrameNo_);
	auto pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す
	barrelPos_ = pos;



	// 発射間隔
	shotDelay_ = 0.2f;

	// バレルフレーム番号(弾の発射位置)
	barrelFrameNo_ = 6;

	// 回転中かどうか
	isRotate_ = false;

	// 突進中かどうか
	isRush_ = false;

	// 相対座標
	//transform_.localPos = VAdd(transform_.pos, LOCAL_COL_POS);

	// 突進中の向き
	rushDir_ = {};

	deg_ = 0.0f;

	// デバッグ用
	debugFlg_ = false;

	// 行動処理
	EnemyAction();

	//soundHandle_ = audioManager_.Play(
	//	AudioList::AUDIO_TYPE::PLAYER_DEATH3,
	//	transform_.pos,
	//	500.0f,
	//	true,
	//	1.0f
	//);
}

void EnemyNeuvio::Update_Enemy(void)
{
	//if (transform_.isHide)return;

	// 発射間隔処理
	ShotDelay();

	// 移動処理
	ProcessMove();

	// 弾発射処理
	ProcessShot();

	// 当たり判定
	if (!isFirstCol_)
	{
		if (transform_.pos.y >= 1150.0f) { collider_->isCollision = false; }
		else { collider_->isCollision = true; isFirstCol_ = true; }
	}
	//collider_->isCollision = true;

	// 移動回転大きさ等をモデルに反映
	transform_.Update();

	// 弾更新
	for (auto& b : bullets_)
	{
		b->Update();
	}

	// 当たり判定制御
	SyncCollision();

}

void EnemyNeuvio::Draw_Enemy(void)
{

	transform_.Draw();

	// 弾描画
	for (auto& b : bullets_)
	{
		b->Draw();
	}

	//// デバッグ
	//DrawDebug();
}

void EnemyNeuvio::Release_Enemy(void)
{
	// モデルの削除(仮)
	transform_.Release();

	// 弾削除(仮)
	for (auto& b : bullets_)
	{
		b->Release();
	}
	bullets_.clear();
}

void EnemyNeuvio::ProcessMove(void)
{

	if (transform_.isHide)return;

	// 壁と当たっていたら処理しない
	if (isCollision_) return;

	// 敵の行動
	switch (action_)
	{
	case EnemyNeuvio::ACTION::FIXED_TURRET:
		Search();
		Motion();
		if (point_ != POINT::B)
		{
			GoToPoint();
			isNotice_ = false;
			return;
		}
		if(isNoticeHearing_ && !isNoticeBack_)
			for (auto p : followPos_)
			{
				Rotation(*p);
			}
		break;
	case EnemyNeuvio::ACTION::FOLLOW:
		Search();
		Follow();
		Motion();
		// 敵機の範囲に入っていたら、追従を優先する
		if (isNotice_ || isNoticeHearing_) return;
		GoToPoint();
		break;
	case EnemyNeuvio::ACTION::PASS:
		isNoticeHearing_ = false;
		isNotice_ = false;
		Motion();
		GoToPoint();
		break;
	case EnemyNeuvio::ACTION::RUSH:
		Search();
		Rush();
		Motion();
		if (isRush_)return;
		GoToPoint();
		break;
	}

}

void EnemyNeuvio::GoToPoint(VECTOR start, VECTOR end, POINT point)
{
	//if (isRotate_)return;

	// 中継地点への移動

	// 現在の地点から特定の地点までの距離
	float dis;
	dis = VSize(VSub(end, start));

	// 中継地点への移動＆中継地点切り替え
	if (dis < POINT_RADIUS)
	{
		point_ = point;
		isRotate_ = true;
	}
	else
	{
		if (!isRotate_) { transform_.pos = Lerp(transform_.pos, end, ENEMY_SPEED); }
	}

	Rotation(end);

}

void EnemyNeuvio::Rotation(VECTOR end)
{

	auto followPos = VAdd(end, LOCAL_SHOT_POS);

	// 現在の地点から特定の地点までの距離 
	VECTOR vec = VSub(transform_.pos , followPos);

	// 方位角
	float azimuth = atan2f(-vec.x, -vec.z);

	// 仰角
	float elevation = atanf(vec.y / Utility::MagnitudeF({ vec.x,0.0f,vec.z }));

	// Z軸は回転させない
	VECTOR rot = { elevation ,azimuth , 0.0f };

	// ゆっくり回転
	LazyRotation(rot.y, rot.x);

}

void EnemyNeuvio::LazyRotation(float goalRotY , float goalRotX)
{

	// 回転処理
	float degNowAngleY = Utility::Rad2DegF(transform_.rot.y);
	float degGoalAngleY = Utility::Rad2DegF(goalRotY);

	float degNowAngleX = Utility::Rad2DegF(transform_.rot.x);
	float degGoalAngleX = Utility::Rad2DegF(goalRotX);

	// 0度～360度以内に角度をおさめる
	degGoalAngleY = static_cast<float>(Utility::DegIn360(degGoalAngleY));
	degNowAngleY = static_cast<float>(Utility::DegIn360(degNowAngleY));

	degGoalAngleX = static_cast<float>(Utility::DegIn360(degGoalAngleX));
	degNowAngleX = static_cast<float>(Utility::DegIn360(degNowAngleX));

	// 回転が少ない方の回転向きを取得する(時計回り:1、反時計回り:-1)
	int aroundDirY = Utility::DirNearAroundDeg(degNowAngleY, degGoalAngleY);
	int aroundDirX = Utility::DirNearAroundDeg(degNowAngleX, degGoalAngleX);

	// 到達したい角度に回転を加える
	if (degNowAngleY <= degGoalAngleY + 5.0f &&
		degNowAngleY >= degGoalAngleY - 5.0f)
	{
		transform_.rot.y = goalRotY;
		isRotate_ = false;
	}
	else
	{
		transform_.rot.y += SPEED_ROT_RAD_ENEMY * static_cast<float>(aroundDirY);
		isRotate_ = true;
	}

	// 到達したい角度に回転を加える
	if (degNowAngleX <= degGoalAngleX + 5.0f &&
		degNowAngleX >= degGoalAngleX - 5.0f)
	{
		transform_.rot.x = goalRotX;
		//isRotate_ = false;
	}
	else
	{
		transform_.rot.x += SPEED_ROT_RAD_ENEMY * static_cast<float>(aroundDirX);
		//isRotate_ = true;
	}

	// 0度～360度以内に角度をおさめる
	transform_.rot.y = static_cast<float>(Utility::RadIn2PI(transform_.rot.y));
	transform_.rot.x = static_cast<float>(Utility::RadIn2PI(transform_.rot.x));

	// クォータニオンに変換
	transform_.quaRot = Quaternion::Euler(transform_.rot);
	
}

VECTOR EnemyNeuvio::Lerp(VECTOR start, VECTOR end, float speed)
{
	// 線形補間

	// 返す値
	VECTOR ret = start;

	// 現在の地点から特定の地点までの距離 
	VECTOR vec = VSub(end, start);

	// 距離を正規化した値
	VECTOR dir = VNorm(vec);

	// 返す値
	ret = VAdd(ret, VScale(dir, speed));

	return ret;

}

void EnemyNeuvio::EnemyAction(void)
{
	int action = GetRand(2);

	switch (action)
	{
	case 0:
		ID_ = ID::ENEMY_NEUVIO_FIXED;
		action_ = ACTION::FIXED_TURRET;
		// モデル色調整
		MV1SetDifColorScale(transform_.modelId, { 1.0f,0.5f,0.5f,1.0f });
		break;
	case 1:
		ID_ = ID::ENEMY_NEUVIO_FOLLOW;
		action_ = ACTION::FOLLOW;
		// モデル色調整
		MV1SetDifColorScale(transform_.modelId, { 0.5f,1.0f,0.5f,1.0f });
		break;
	case 2:
		ID_ = ID::ENEMY_NEUVIO_THROUGH;
		action_ = ACTION::PASS;
		// モデル色調整
		MV1SetDifColorScale(transform_.modelId, { 0.5f,0.5f,1.0f,1.0f });
		break;
	case 3:
 		action_ = ACTION::RUSH;
		// モデル色調整
		MV1SetDifColorScale(transform_.modelId, { 0.5f,1.0f,1.0f,1.0f });
		break;
	}

}

void EnemyNeuvio::GoToPoint(void)
{

	// 決められた地点に向かう
	switch (point_)
	{
	case EnemyNeuvio::POINT::A:
		GoToPoint(transform_.pos, pointB_, POINT::B);
		break;
	case EnemyNeuvio::POINT::B:
		GoToPoint(transform_.pos, pointC_, POINT::C);
		break;
	case EnemyNeuvio::POINT::C:
		GoToPoint(transform_.pos, pointA_, POINT::A);
		break;
	}

}

void EnemyNeuvio::Motion(void)
{
	deg_ += 5.0f;
	float speed = 1.0f;
	// エネミーからプレイヤーまでのベクトル
	//float dis = VSize(VSub(followPos_, transform_.pos));
	//float sin = std::clamp(sinf(((deg_ / 50.0f * DX_PI_F) / 180.0f) * speed), -1.0f, 1.0f);
	//transform_.pos = VAdd(transform_.pos, { 0.0f,5.0f - 2.0f * sin,0.0f });
	//transform_.pos.y = (transform_.prePos.y - 1.0f) - transform_.prePos.y * sin;

	float pow = sinf(Utility::Deg2RadF(deg_)) * 1.0f;
	transform_.pos.y += pow;
}

void EnemyNeuvio::Follow(void)
{
	auto& info = GameManager::GetInstance();
	for (int i = 0; i < info.MAX_PLAYER; i++)
	{
		if (info.IsHide(i))return;
	}
	// 範囲外だったら、処理をしない
	if (!isNoticeHearing_ || isNoticeBack_ || isCollision_	&& info.GetTimeToStart()>= 0.0f)return;

	colliderEye_->isCollision = true;

	// 範囲内だったら、プレイヤーに向かって回転させる
	if (isNoticeHearing_)
	{
		for (auto p : followPos_)
		{
			for (auto p : followPos_)
			{
				Rotation(*p);
			}
		}
	}

	//if (!isNotice_)
	//{
	//	return;
	//}

	for (auto p : followPos_)
	{
		// エネミーからプレイヤーまでのベクトル
		float dis = VSize(VSub(*p, transform_.pos));

		VECTOR diff = VSub(*p, transform_.pos);

		// Z方向を0とした時の角度
		float goalRot = atan2f(diff.x, diff.z);

		//rot_.y = goalRot;

		VECTOR dir = Utility::VNormalize(diff);
		//dir.y = 0.0f;

		if (dis < PLAYER_DISTANCE)
		{
		}
		else
		{
			transform_.pos = VAdd(transform_.pos, VScale(dir, ENEMY_SPEED));
		}
	}
}

void EnemyNeuvio::Search(void)
{

	// 検知フラグリセット
	isNotice_ = false;
	isNoticeHearing_ = false;
	isNoticeBack_ = false;

	auto& info = GameManager::GetInstance();
	if (info.GetTimeToStart() >= 0.0f)return;

	for (int i = 0; i < info.MAX_PLAYER; i++)
	{
		if (info.IsHide(i))return;
	}

	for (auto p : followPos_)
	{
		// プレイヤーの座標を取得
		VECTOR pPos = *p;

		// エネミーの角度(qua→rot)
		VECTOR rot = Quaternion::ToEuler(transform_.quaRot);

		// エネミーからプレイヤーまでのベクトル
		VECTOR diff = VSub(pPos, transform_.pos);

		// 聴覚
		if ((diff.z * diff.z + diff.x * diff.x + diff.y * diff.y) <= HEARING_RANGE * HEARING_RANGE)
		{
			isNoticeHearing_ = true;
		}

		// エネミーの向いている向き
		float degE = Utility::DegIn360(
			Utility::Rad2DegF(rot.y));

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

		// 背後範囲
		float degRangeBackL = 180.0f - VIEW_ANGLE;
		float degRangeBackR = 180.0f + VIEW_ANGLE;

		if (degRangeBackL <= degP && degP <= degRangeBackR)
		{
			if ((diff.z * diff.z + diff.x * diff.x + diff.y * diff.y)
				<= VIEW_RANGE * VIEW_RANGE)
			{
				isNoticeHearing_ = false;
				isNoticeBack_ = true;
				// デバッグ
				//MV1SetDifColorScale(transform_.modelId, { 1.0f,1.0f,1.0f,1.0f });
			}
		}
	}
}

void EnemyNeuvio::Rush(void)
{

	if (!isRush_) { if (!isNoticeHearing_)return; }

	for (auto p : followPos_)
	{
		if (!isRush_ && isNoticeHearing_)
		{

			isRush_ = true;

			followRushPos_ = *p;

			// エネミーからプレイヤーまでのベクトル
			float dis = VSize(VSub(*p, transform_.pos));

			VECTOR diff = VSub(*p, transform_.pos);

			// Z方向を0とした時の角度
			float goalRot = atan2f(diff.x, diff.z);

			//rot_.y = goalRot;

			VECTOR dir = Utility::VNormalize(diff);
			//dir.y = 0.0f;

			rushDir_ = dir;

		}
	}
	// 回転
	Rotation(followRushPos_);

	// 移動
	transform_.pos = VAdd(transform_.pos, VScale(rushDir_, ENEMY_SPEED));

}

void EnemyNeuvio::SyncCollision(void)
{

	//if (transform_.isHide) return;

	// 追従対象の位置
	VECTOR followPos = transform_.pos;

	// 追従対象の向き
	Quaternion followRot = transform_.quaRot;

	// 追従対象から当たり判定までの相対座標
	VECTOR relativeLPos = followRot.PosAxis(LOCAL_COL_POS);

	// 当たり判定位置の更新
	VECTOR colPos = VAdd(followPos, relativeLPos);

	// 当たり判定回転
	VECTOR jetRot = transform_.quaRot.ToEuler(followRot);

	// 当たり判定
	collider_->Update(colPos, jetRot);

	for (auto p : followPos_)
	{
		colliderEye_->Update(transform_.pos, *p);
	}

	// 目線
	// エネミーとプレイヤーの線
	VECTOR sendRot = {};
	auto cameraPos = SceneManager::GetInstance().GetCamera()->GetPos();
	auto col = std::dynamic_pointer_cast<ColliderLine>(colliderEye_);
	col->Update(transform_.pos, cameraPos, sendRot);

	//colliderEye_->isCollision = true;

	isCollision_ = false;


}

void EnemyNeuvio::CreateBullet(void)
{

	// 弾の配列を回し、再利用可能なものを探す
	for (auto& b : bullets_)
	{
		auto bPtr = std::dynamic_pointer_cast<ObjectBase>(b);
		auto transform = bPtr->GetTransform();
		// 表示状態のものは再利用できない
		if (!transform.isHide) continue;

		// 弾情報を設定
		SetBullet(b);

		return;
	}

	// 一時的な弾
	std::shared_ptr<BulletBase> tempBullet = std::make_shared<BulletEnemy>(bulletModelId_, std::dynamic_pointer_cast<ActorBase>(GetPtr()));

	tempBullet->Init();

	// 弾情報を設定
	SetBullet(tempBullet);

	// 配列に格納
	bullets_.emplace_back(tempBullet);

}

void EnemyNeuvio::SetBullet(std::shared_ptr<BulletBase> bullet)
{

	// 発射位置にリセット(表示させる)

	// 発射準備
	for (auto p : followPos_)
	{
		bullet->ReadyBullet(barrelPos_, transform_.GetForward());
	}

}

void EnemyNeuvio::ShotDelay(void)
{
	// 発射間隔処理
	if (shotDelayCnt_ > 0.0f)
	{
		shotDelayCnt_ -= SceneManager::GetInstance().GetDeltaMult(1.0f);
		if (shotDelayCnt_ <= 0.0f)shotDelayCnt_ = 0.0f;
	}
}

void EnemyNeuvio::ProcessShot(void)
{

	auto& info = GameManager::GetInstance();
	for (int i = 0; i < info.MAX_PLAYER; i++)
	{
		if (info.IsHide(i))
		{
			return;
		}
	}

	if (transform_.isHide || info.GetTimeToStart() >= 0.0f)return;



	// 発射間隔
	if (shotDelayCnt_ > 0.0f || !isNotice_ || isCollision_) return;

	if (CollisionManager::GetInstance().IsCollisionEnemyEyeMap(colliderEye_)) return;

	AudioManager::GetInstance().Play(
		AudioList::AUDIO_TYPE::ENMEY_SHOT,
		transform_.pos,
		5500.0f,
		false,
		0.5f,
		100 / 2
	);

	// 弾の発射位置
	// 対象フレームのワールド行列(大きさ、回転、位置)を取得する
	auto mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, barrelFrameNo_);
	auto pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す
	barrelPos_ = pos;

	// 弾生成
	CreateBullet();

	// 発射間隔再設定
	shotDelayCnt_ = shotDelay_;
}

void EnemyNeuvio::ProcessDead(void)
{
	transform_.isHide = true;

	EffectManager::EFFECT temp;
	temp.pos = transform_.pos;
	temp.rot = transform_.quaRot.ToEuler();
	temp.scl = { 15.0f,15.0f,15.0f };
	temp.speed = 3.0f;
	temp.type = EffectManager::EFFECT_TYPE::DESTROY;
	EffectManager::GetInstance().AddEffect(temp);

	audioManager_.Play(
		AudioList::AUDIO_TYPE::ENEMY_DESTROY,
		transform_.pos,
		5500.0f,
		false,
		1.0f
	);
	//audioManager_.Stop(soundHandle_);

	auto& gMng = GameManager::GetInstance();

	switch (action_)
	{
	case EnemyNeuvio::ACTION::FIXED_TURRET:
		// 当たり判定
		isCollision_ = false;
		collider_->isCollision = false;
		gMng.AddScore(FIXED_TURRET_SCORE,(int)lastAtkID_);
		gMng.Kill(ID_, lastAtkID_);
		break;
	case EnemyNeuvio::ACTION::FOLLOW:
		// 当たり判定
		isCollision_ = false;
		collider_->isCollision = false;
		gMng.AddScore(FOLLOW_SCORE, (int)lastAtkID_);
		gMng.Kill(ID_, lastAtkID_);
		break;
	case EnemyNeuvio::ACTION::PASS:
		// 当たり判定
		isCollision_ = false;
		collider_->isCollision = false;
		gMng.AddScore(PASS_SCORE, (int)lastAtkID_);
		gMng.Kill(ID_, lastAtkID_);
		break;
	case EnemyNeuvio::ACTION::RUSH:
		// 当たり判定
		isCollision_ = false;
		collider_->isCollision = false;
		gMng.AddScore(RUSH_SCORE, (int)lastAtkID_);
		gMng.Kill(ID_, lastAtkID_);
		break;
	}
	gMng.AddElimination(ID_);

}

VECTOR EnemyNeuvio::VMult(VECTOR vec1, VECTOR vec2)
{

	VECTOR ret{};

	ret.x = vec1.x * vec2.x;
	ret.y = vec1.y * vec2.y;
	ret.z = vec1.z * vec2.z;

	return ret;

}

void EnemyNeuvio::DrawDebug(void)
{

	if (debugFlg_)
	{
		DrawFormatString(100, 100, 0xff0000,"hit");
	}
	else
	{
		DrawFormatString(100, 100, 0xff0000, "noHit");
	}

	//DrawSphere3D(*followPos_[0], 20.0f, 10, 0xffffff, 0xffffff, true);

	// エネミーの角度(qua→rot)
	//VECTOR rot = Quaternion::ToEuler(transform_.quaRot);

	// 聴覚
	//DrawCone3D(
	//	VAdd(transform_.pos, { 0.0f,0.0f,0.0f }),
	//	VAdd(transform_.pos, { 0.0f,1.0f,0.0f }),
	//	HEARING_RANGE, 10, 0xfff7f, 0xfff7f, true);

	//// 視覚
	//float viewRad = Utility::Deg2RadF(VIEW_ANGLE);
	//VECTOR localPos = { 0.0f,5.0f,VIEW_RANGE };

	//// 回転行列
	//MATRIX mat = MGetIdent();
	//mat = MMult(mat, MGetRotY(rot.y));

	//// 前方
	//VECTOR forwardPos = VAdd(transform_.pos, VTransform(localPos, mat));
	//// 後方
	//VECTOR buckPos = VAdd(transform_.GetBack(),VTransform(localPos,mat));

	//// 右側
	//MATRIX rightMat = MMult(mat, MGetRotY(viewRad));
	////VECTOR rightPos = VAdd(transform_.pos, VTransform(localPos, rightMat));
	//VECTOR rightPos = VAdd(transform_.GetBack(), VTransform(localPos, rightMat));

	////DrawTriangle3D({ transform_.pos.x,transform_.pos.y + 5,transform_.pos.z },
	////	forwardPos, rightPos, 0xffff00, true);
	//DrawTriangle3D({ buckPos.x,buckPos.y + 5,buckPos.z },
	//	forwardPos, rightPos, 0x00ffff, true);

	//// 左側
	//MATRIX leftMat = MMult(mat, MGetRotY(-viewRad));
	////VECTOR leftPos = VAdd(transform_.pos, VTransform(localPos, leftMat));
	//VECTOR leftPos = VAdd(transform_.GetBack(), VTransform(localPos, leftMat));

	////DrawTriangle3D({ transform_.pos.x,transform_.pos.y + 5,transform_.pos.z },
	////	leftPos, forwardPos, 0xffff00, true);
	//DrawTriangle3D({ buckPos.x,buckPos.y + 5,buckPos.z },
	//	leftPos, forwardPos, 0x00ffff, true);

	//DrawSphere3D(transform_.pos, 10.0f, 5, 0xffff00, 0xffff00, false);


}
