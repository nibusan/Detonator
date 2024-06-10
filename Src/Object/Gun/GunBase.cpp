#include <math.h>
#include "../../Manager/SceneManager.h"
#include "../../Manager/CollisionManager.h"
#include "../../Manager/EffectManager.h"
#include "../../Manager/AudioManager.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/Camera/GameCamera.h"
#include "../../Utility/Utility.h"
#include "../Actor/ActorBase.h"
#include "../Actor/Player/Player.h"
#include "GunBase.h"

GunBase::GunBase(const Transform& follow, int frameNo, std::weak_ptr<ActorBase> parent) :
follow_(follow),
followFrameNo_(frameNo),
netManager_(NetManager::GetInstance())
{
	parent_ = parent;
	ID_ = parent_.lock()->GetID();

	auto player = std::dynamic_pointer_cast<Player>(parent_.lock());
	if (player == nullptr) return;
	parentKey_ = player->GetKey();

	isShot_ = false;

}

GunBase::~GunBase(void)
{

}

void GunBase::UpdateAfterCollision(void)
{
	// 弾更新
	for (auto& b : bullets_)
	{
		b->UpdateAfterCollision();
	}
}

void GunBase::Init_Object(void)
{
	SetParam();

	model_ = std::make_unique<Model>(ResourceManager::GetInstance().GetResourceFile(fileName_.c_str())->GetHandle());
	model_->SetIsAutoDeleteHandle(true);
	transform_.SetModel(model_->GetHandle());


	// 弾の発射位置
	// 対象フレームのワールド行列(大きさ、回転、位置)を取得する
	auto mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, barrelFrameNo_);
	auto pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す
	barrelPos_ = pos;

	// 追従
	Follow();

	//scopeModel_ = std::make_unique<Model>(ResourceManager::GetInstance().Get(scopeFileName_.c_str())->GetHandle());
	//scopeModel_->SetIsAutoDeleteHandle(true);
	//scpTransform_.SetModel(scopeModel_->GetHandle());
	//scpTransform_.Update();

	// スコープモデル仮読み込み
	scopeModelId_ =
		MV1LoadModel(("Data/Model/Gun/Scope/" + scopeFileName_).c_str());
	scpTransform_.SetModel(scopeModelId_);
	scpTransform_.Update();

	// 弾モデル仮読み込み
	//bulletModelId_ =
	//	MV1LoadModel(("Data/Model/Bullet/" + bulletFileName_).c_str());

	// 移動回転大きさ等をモデルに反映
	transform_.Update();


	// 銃の状態
	state_ = STATE::NONE;
	preState_ = state_;

	// 連続して撃った数
	burstCnt_ = 0;

	// 弾数
	magazine_ = maxMagazine_;

	// 回転中かどうか
	isRotate_ = false;

	// 回転開始時のqua
	startQuaRot_ = {};

	// 回転割合
	rotateRate_ = 0.0f;
	// 遷移
	changeCnt_ = TARGET_CHANGE_TIME;

	isShot_ = false;
}

void GunBase::Update_Object(void)
{

	// 追従
	Follow();

	// 移動回転大きさ等をモデルに反映
	transform_.Update();

	// スコープ追従
	scpTransform_.quaRot = transform_.quaRot;
	scpTransform_.pos = VAdd(transform_.pos,
		scpTransform_.quaRot.PosAxis(scpRelativePos_));
	scpTransform_.Update();

	// 弾の発射位置
	// 対象フレームのワールド行列(大きさ、回転、位置)を取得する
	if (barrelFrameNo_ != 0)
	{
		auto mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, barrelFrameNo_);
		auto pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す
		barrelPos_ = pos;
	}
	else
	{
		VECTOR lotPos = VAdd(transform_.pos, transform_.quaRot.PosAxis(barrelRelativePos_));
		barrelPos_ = lotPos;
	}

	if (isShot_) {
		ShotUpdate();
	}

	// 発射間隔処理
	ShotDelay();

	//// 弾更新
	//for (auto& b : bullets_)
	//{
	//	b->Update();
	//}



	// エフェクト更新
	std::erase_if(effectID_,
		[](int id) {auto& effect = EffectManager::GetInstance();
	return !effect.GetData(id).lock()->IsPlaying(); });

	auto& effect = EffectManager::GetInstance();
	for (auto id : effectID_) {
		auto& data = effect.GetData(id);
		data.lock()->SetPos(barrelPos_);
		VECTOR localRot = Quaternion::ToEuler(
			transform_.quaRot.Mult(Quaternion::Euler({ 0.0f,90.0f,0.0f }))
		);
		data.lock()->SetAngle(localRot);
	}
	isShot_ = false;
}

void GunBase::Draw_Object(void)
{
	// モデル描画
	transform_.Draw();

	// 弾描画
	for (auto& b : bullets_)
	{
		b->Draw();
	}

	MV1DrawFrame(scopeModelId_, 0);
	// デバッグ
	//DrawSphere3D(targetPos_, 10.0f, 12, 0xffffff, 0xffffff, true);

	//DrawFormatString(1200, 60 + GetFontSize() * 1 + 150 * (int)ID_, 0xff00ff,
	//	"pos : %.2f,%.2f,%.2f", transform_.pos.x, transform_.pos.y, transform_.pos.z);
	//DrawFormatString(1200, 60 + GetFontSize() * 2 + 150 * (int)ID_, 0xff00ff,
	//	"rot : %.2f,%.2f,%.2f", transform_.rot.x, transform_.rot.y, transform_.rot.z);

	//DrawSphere3D(barrelPos_, 1.0f, 10, 0xffffff, 0xffffff, true);
	//DrawSphere3D(deg_, 3.0f, 10, 0xffffff, 0xffffff, true);

}

void GunBase::Release_Object(void)
{
	// モデル削除(仮)
	transform_.Release();

	// 弾削除(仮)
	for (auto& b : bullets_)
	{
		b->Release();
	}
	bullets_.clear();
}

void GunBase::DrawTrans(void)
{
	// ライトの設定
	SetUseLighting(false);
	MV1DrawFrame(scopeModelId_,1);
	SetUseLighting(true);
}

void GunBase::UpdateTransform(void)
{
	// 追従
	Follow();

	transform_.Update();

	// スコープ追従
	scpTransform_.quaRot = transform_.quaRot;
	scpTransform_.pos = VAdd(transform_.pos,
		scpTransform_.quaRot.PosAxis(scpRelativePos_));
	scpTransform_.Update();

	// 弾の発射位置
	// 対象フレームのワールド行列(大きさ、回転、位置)を取得する
	if (barrelFrameNo_ != 0)
	{
		auto mat = MV1GetFrameLocalWorldMatrix(transform_.modelId, barrelFrameNo_);
		auto pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す
		barrelPos_ = pos;
	}
	else
	{
		VECTOR lotPos = VAdd(transform_.pos, transform_.quaRot.PosAxis(barrelRelativePos_));
		barrelPos_ = lotPos;
	}
}

void GunBase::Shot(void)
{
	isShot_ = true;
}

void GunBase::ShotUpdate(void)
{
	// 発射間隔
	if (shotDelayCnt_ > 0.0f || magazine_ <= 0.0f) return;

	PlayBurstSound();

	// 弾生成
	CreateBullet();

	// 発射間隔再設定
	shotDelayCnt_ = shotDelay_;

	// 弾数
	magazine_--;

	// 連続して撃った数
	burstCnt_++;


	//EffectManager::EFFECT temp;
	//temp.pos = barrelPos_;
	//VECTOR localRot = Quaternion::ToEuler(
	//	transform_.quaRot.Mult(Quaternion::Euler({ 0.0f,90.0f,0.0f }))
	//);
	//temp.rot = localRot;
	//temp.scl = { 5.0f,5.0f,5.0f };
	//temp.speed = 1.0f;
	//temp.type = EffectManager::EFFECT_TYPE::BULLET;
	//effectID_.emplace_back(
	//	EffectManager::GetInstance().AddEffect(temp));

	EffectManager::EFFECT temp;
	temp.pos = barrelPos_;
	VECTOR localRot = Quaternion::ToEuler(
		transform_.quaRot.Mult(Quaternion::Euler({ 0.0f,90.0f,0.0f }))
	);
	temp.rot = localRot;
	temp.scl = { 5.0f,5.0f,5.0f };
	temp.speed = 1.0f;
	temp.type = EffectManager::EFFECT_TYPE::BULLET;
	effectID_.emplace_back(
		EffectManager::GetInstance().AddEffect(temp));
}

const Recoil::RECOIL_TYPE GunBase::GetRecoilType(void) const
{
	return recoil_[burstCnt_];
}

const int GunBase::GetMagazine(void) const
{
	return magazine_;
}

const int GunBase::GetMaxMagazine(void) const
{
	return maxMagazine_;
}

const float GunBase::GetRecoilMag(void) const
{
	return recoilMag_;
}

void GunBase::ResetBurstNum(void)
{
	burstCnt_ = 0.0f;
}

void GunBase::ReloadGun(void)
{
	magazine_ = maxMagazine_;
	burstCnt_ = 0.0f;
}

void GunBase::ChangeState(STATE state, float time)
{
	preState_ = state_;

	if (state_ == state) return;

	state_ = state;
	isRotate_ = true;

	// 回転割合
	rotateRate_ = 0.0f;
	rotateTimeCnt_ = rotateTime_ = time;

	// 回転開始時のqua
	startQuaRot_ = transform_.quaRot;

	changeCnt_ = 0.0f;
}


void GunBase::Follow(void)
{
	switch (state_)
	{
	case GunBase::STATE::NONE:
		FollowNone();
		break;
	case GunBase::STATE::SHOT:
		FollowShot();
		break;
	case GunBase::STATE::RELOAD:
		FollowReload();
		break;
	}
}

void GunBase::FollowNone(void)
{

	//// 対象フレームのワールド行列(大きさ、回転、位置)を取得する
	//auto mat = MV1GetFrameLocalWorldMatrix(follow_.modelId, followFrameNo_);
	////auto scl = MGetSize(mat);			// 行列から大きさを取り出す
	//auto rot = MGetRotElem(mat);		// 行列から回転を取り出す
	//auto pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す


	// 回転同期
	// フレームAとフレームBのベクトルから角度を求め、
	// その回転をさせる

	// ただ、銃の回転点をずらす必要がある(相対座標でOK)

	// 対象フレームのワールド行列(大きさ、回転、位置)を取得する(先)
	auto mat = MV1GetFrameLocalWorldMatrix(follow_.modelId, 15);
	auto fraPosA = MGetTranslateElem(mat);	// 行列から移動値を取り出す
	//Target(fraPosA);

	// 対象フレームのワールド行列(大きさ、回転、位置)を取得する(元)
	mat = MV1GetFrameLocalWorldMatrix(follow_.modelId, followFrameNo_);
	auto fraPosB = MGetTranslateElem(mat);	// 行列から移動値を取り出す

	// 2点から角度を求める(BtoA)
	auto diff = VSub(fraPosA, fraPosB);

	// 方位角
	float azimuth = atan2f(diff.x, diff.z);

	// 仰角
	float elevation = atanf(-diff.y / Utility::MagnitudeF({ diff.x,0.0f,diff.z }));

	// Z軸は回転させない
	VECTOR rot = { elevation ,azimuth , 0.0f };

	// 回転を設定
	// 回転中なら
	if (isRotate_)Rotation(Quaternion::Euler(rot));
	else transform_.quaRot = Quaternion::Euler(rot);

	// 位置同期
	transform_.pos = VAdd(fraPosB, transform_.quaRot.PosAxis(relativePos_));

	//transform_.quaRot = Quaternion::GetRotation(rot);
	//auto qua = Quaternion::GetRotation(rot);
	//
	//// これが手の上方向
	//auto a = qua.GetUp();
	//
	// フレームの上方向を軸にして回転(回転量)
	//auto axis = Quaternion::AngleAxis(d * DX_PI_F / 180.0f, qua.GetUp());
	//auto axisF = Quaternion::AngleAxis(-90.0f * DX_PI_F / 180.0f, qua.GetForward());/*
	//auto axisR = Quaternion::AngleAxis(-90.0f * DX_PI_F / 180.0f, qua.GetRight());*/
	//
	// フレームの上方向を軸にして回転(回転量)
	// Unity基準でZXYの順で回転させる
	//Quaternion gunAxisY = Quaternion::AngleAxis(-90.0f * DX_PI_F / 180.0f, qua.GetUp());
	//Quaternion gunAxisX = Quaternion::AngleAxis(20.0f * DX_PI_F / 180.0f, qua.GetRight());
	//Quaternion gunAxisZ = Quaternion::AngleAxis(-45.0f * DX_PI_F / 180.0f, qua.GetForward());
	//
	// 微調整回転量作成
	//Quaternion rotPow = Quaternion::Identity();
	//rotPow = Quaternion::Mult(gunAxisZ, Quaternion::Mult(gunAxisX, gunAxisY));
	//
	// 回転同期
	//transform_.quaRot = rotPow.Mult(qua);
	//
	// 座標同期
	//VECTOR localPos = Quaternion::PosAxis(transform_.quaRot, relativePos_);
	//transform_.pos = VAdd(pos, localPos);
	//transform_.pos = pos;
}

void GunBase::Rotation(Quaternion goalRot)
{

	rotateTimeCnt_ -= SceneManager::GetInstance().GetDeltaTime();
	if (rotateTimeCnt_ <= 0.0f)rotateTimeCnt_ = 0.0f;
	float rate = (rotateTime_ - rotateTimeCnt_) / rotateTime_;
	// 回転割合
	//rotateRate_ += SceneManager::GetInstance().GetDeltaMult(5.0f);
	//if (rotateRate_ >= 1.0f)
	if (rate >= 1.0f)
	{
		//transform_.quaRot = goalRot;
		//rotateRate_ = 1.0f;
		//isRotate_ = false;
		transform_.quaRot = goalRot;
		isRotate_ = false;
	}
	else
	{
		// 徐々に回転
		//transform_.quaRot = Quaternion::Slerp(startQuaRot_, goalRot, rotateRate_);
		transform_.quaRot = Quaternion::Slerp(startQuaRot_, goalRot, rate);
	}



	//// 回転中かどうか
	//if (fabsf(Quaternion::Angle(transform_.quaRot, goalRot)) < Utility::Deg2RadF(3.0f))
	//{
	//	transform_.quaRot = goalRot;
	//	isRotate_ = false;
	//}
	
}

//void GunBase::Target(VECTOR goalTargetPos)
//{
//	//changeCnt_ += SceneManager::GetInstance().GetDeltaTime();
//	//if (changeCnt_ >= TARGET_CHANGE_TIME)
//	//{
//	//	changeCnt_ = TARGET_CHANGE_TIME;
//	//}
//	//float rate = changeCnt_ / TARGET_CHANGE_TIME;
//	//targetPos_ = Utility::Lerp(targetPos_, goalTargetPos, rate);
//}


void GunBase::FollowShot(void)
{
	// 回転同期
	// 注視点とフレームのベクトルから角度を求め、
	// その回転をさせる

	// ただ、銃の回転点をずらす必要がある(相対座標でOK)

	// 注視点の座標
	//auto camera = std::dynamic_pointer_cast<GameCamera>(SceneManager::GetInstance().GetCamera());
	//VECTOR gTarPos = CollisionManager::GetInstance().GetGunImpactPos();
	auto player = std::dynamic_pointer_cast<Player>(parent_.lock());
	
	VECTOR gTarPos = player->GetLatestTargetPos();
	//gTarPos = camera->GetLatestTargetPos();
	//Target(gTarPos);
	

	// 対象フレームのワールド行列(大きさ、回転、位置)を取得する
	auto mat = MV1GetFrameLocalWorldMatrix(follow_.modelId, followFrameNo_);
	auto fraPos = MGetTranslateElem(mat);	// 行列から移動値を取り出す

	//if (Utility::Distance(targetPos_, gTarPos) >= 1.0f && preState_ == state_)
	//{
	//	//auto dif = VSub(gTarPos, targetPos_);
	//	//auto vec = VNorm(dif);
	//	//targetPos_ = VAdd(targetPos_,VScale(vec,5.0f));
	//	targetPos_ = Utility::Lerp(targetPos_, gTarPos, 0.05f);
	//}
	//else
	//{
		targetPos_ = gTarPos;
	//}

	// 2点から角度を求める
	auto diff = VSub(targetPos_, fraPos);

	// 方位角
	float azimuth = atan2f(diff.x, diff.z);

	// 仰角
	float elevation = atanf(-diff.y / Utility::MagnitudeF({ diff.x,0.0f,diff.z }));

	// Z軸は回転させない
	VECTOR rot = { elevation ,azimuth , 0.0f };

	// 回転を設定
	// 回転中なら
	if (isRotate_)Rotation(Quaternion::Euler(rot));
	else transform_.quaRot = Quaternion::Euler(rot);

	// 位置同期
	transform_.pos = VAdd(fraPos, transform_.quaRot.PosAxis(relativePos_));
}

void GunBase::FollowReload(void)
{
	// 対象フレームのワールド行列(大きさ、回転、位置)を取得する
	auto mat = MV1GetFrameLocalWorldMatrix(follow_.modelId, followFrameNo_);
	//auto mat = MV1GetFrameLocalWorldMatrix(follow_.modelId, 38);
	auto fraPos = MGetTranslateElem(mat);	// 行列から移動値を取り出す
	auto rot = MGetRotElem(mat);	// 行列から回転を取り出す

	//transform_.quaRot = Quaternion::GetRotation(rot);
	auto qua = Quaternion::GetRotation(rot);

	// フレームの上方向を軸にして回転(回転量)
	// Unity基準でZXYの順で回転させる
	Quaternion gunAxisY = Quaternion::AngleAxis(-90.0f * DX_PI_F / 180.0f, qua.GetUp());
	Quaternion gunAxisX = Quaternion::AngleAxis(20.0f * DX_PI_F / 180.0f, qua.GetRight());
	Quaternion gunAxisZ = Quaternion::AngleAxis(-45.0f * DX_PI_F / 180.0f, qua.GetForward());
	//
	// 微調整回転量作成
	Quaternion rotPow = Quaternion::Identity();
	rotPow = Quaternion::Mult(gunAxisZ, Quaternion::Mult(gunAxisX, gunAxisY));

	//auto gTargetPos = rotPow.PosAxis({ 0.0f,0.0f,100.0f });
	//Target(gTargetPos);



	// 回転同期
	if (isRotate_)
	{
		// 目標
		VECTOR goal = rotPow.Mult(qua).ToEuler();

		Rotation(Quaternion::Euler(goal));
	}
	else transform_.quaRot = rotPow.Mult(qua);
	

	// 位置同期
	transform_.pos = VAdd(fraPos, transform_.quaRot.PosAxis(relativePos_));
}

void GunBase::ShotDelay(void)
{
	// 発射間隔処理
	if (shotDelayCnt_ > 0.0f)
	{
		shotDelayCnt_ -= SceneManager::GetInstance().GetDeltaMult(1.0f);
		if (shotDelayCnt_ <= 0.0f)shotDelayCnt_ = 0.0f;
	}
}

bool GunBase::IsRotate(void) const
{
	return isRotate_;
}

bool GunBase::IsRemainMagazine(void) const
{
	return magazine_ > 0;
}

bool GunBase::IsShootable(void) const
{
	//return !(shotDelayCnt_ > 0.0f || magazine_ <= 0.0f);
	return (shotDelayCnt_ <= 0.0f && magazine_ > 0);
}

const float& GunBase::GetMagnification(void) const
{
	return magnification_;
}

const VECTOR& GunBase::GetBarrelPos(void) const
{
	return barrelPos_;
}

const float& GunBase::GetRandLength(void) const
{
	return randLength_;
}

const float& GunBase::GetRandRadius(void) const
{
	return randRadius_;
}

const float& GunBase::GetShotDelayTime(void) const
{
	return shotDelay_;
}

VECTOR GunBase::GetFoward(void) const
{
	return transform_.GetForward();
}

const VECTOR GunBase::GetScopePos(void) const
{
	return {};
}

std::vector<std::shared_ptr<BulletBase>>& GunBase::GetBullets(void)
{
	return bullets_;
}

void GunBase::SetBullet(std::shared_ptr<BulletBase> bullet)
{
	// 発射位置にリセット(表示させる)

	//// 発射方向を円状にばらけさせる
	//// 起点
	//auto sP = barrelPos_;
	//// 終点
	//auto eP = VAdd(sP, VScale(transform_.GetForward(), randLength_));

	//// 角度ランダム
	//float radian = Utility::Deg2RadF((float)GetRand(360));

	//// 半径ランダム
	//float radius = GetRand((int)randRadius_ * 10) / 10.0f;

	//// 円状の方向
	//auto up = Quaternion::AngleAxis(radian, transform_.GetForward());
	//auto sDir = up.GetUp();

	//// 目標点
	//auto oP = VAdd(eP, VScale(sDir, radius));

	//// 始点から目標点へのベクトル
	//auto vec = VNorm(VSub(oP, sP));

	//// テスト
	//vec = VNorm(VSub(eP, sP));

	//// 発射準備
	////bullet->ReadyBullet(barrelPos_, vec);

	//int frame = netManager_.GetFrameNo();

	//if (parentKey_ != -1 && 
	//	parentKey_ != netManager_.GetSelf().key) {
	//	vec = netManager_.GetBulletDir(parentKey_, vec);
	//}

	//if (parentKey_ != -1 &&
	//	parentKey_ == netManager_.GetSelf().key) {
	//	netManager_.SetBulletDir(vec);
	//}



	auto player = std::dynamic_pointer_cast<Player>(parent_.lock());
	if (player == nullptr) return;
	int seed = player->GetRandSeed();

	float randLength;
	float randRadius;
	if (player->IsADS()) {
		randLength = randLength_;
		randRadius = randRadius_;
	}
	else {
		randLength = hipRandLength_;
		randRadius = hipRandRadius_;
	}


	// 角度ランダム
	//float radian = Utility::Deg2RadF((float)GetRand(360));
	float radian = Utility::Deg2RadF((float)(seed % 360));

	// 半径ランダム
	//float radius = GetRand((int)randRadius_ * 10) / 10.0f;

	float radius = 0.0f;
	if (randRadius != 0.0f) {
		radius = (seed % (int)(randRadius * 10.0f)) / 10.0f;
	}

	// 発射方向を円状にばらけさせる
	// 起点
	auto sP = barrelPos_;
	// 終点
	auto eP = VAdd(sP, VScale(transform_.GetForward(), randLength));


	

	// 円状の方向
	auto up = Quaternion::AngleAxis(radian, transform_.GetForward());
	auto sDir = up.GetUp();

	// 目標点
	auto oP = VAdd(eP, VScale(sDir, radius));

	// 始点から目標点へのベクトル
	auto vec = VNorm(VSub(oP, sP));

	// 発射準備
	bullet->ReadyBullet(barrelPos_, vec);
}
