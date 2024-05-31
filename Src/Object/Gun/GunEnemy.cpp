#include "../../Manager/AudioManager.h"
#include "../Bullet/BulletGeorge.h"
#include "../../Utility/Utility.h"
#include "../Actor/Enemy/EnemyGeorge.h"
#include "GunEnemy.h"

GunEnemy::GunEnemy(const Transform& follow, int frameNo, std::weak_ptr<ActorBase> parent) : GunBase(follow, frameNo, parent)
{
}

void GunEnemy::CreateBullet(void)
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
	std::shared_ptr<BulletBase> tempBullet = std::make_shared<BulletGeorge>(bulletModelId_, parent_);

	tempBullet->Init();

	// 弾情報を設定
	SetBullet(tempBullet);

	// 配列に格納
	bullets_.emplace_back(tempBullet);
}

void GunEnemy::PlayReloadSound(void)
{
	AudioManager::GetInstance().Play(
		AudioList::AUDIO_TYPE::GUN_RELOAD_AK47,
		transform_.pos,
		500.0f,
		false,
		0.7f
	);
}

void GunEnemy::SetParam(void)
{
	// モデルファイル名
	fileName_ = "Model_Gun_SS2";

	// スコープモデルファイル名
	scopeFileName_ = "Red+Dot+Sight.mv1";

	// バレルフレーム番号(弾の発射位置)
	barrelFrameNo_ = 6;

	// 最大弾数
	maxMagazine_ = 9999;

	// 発射間隔
	shotDelay_ = 0.1f;

	// 相対座標
	relativePos_ = RELATIVE_POS;

	// ローカル回転
	VECTOR localRot = { Utility::Deg2RadF(0.0f),Utility::Deg2RadF(180.0f),Utility::Deg2RadF(0.0f) };
	transform_.quaRotLocal = Quaternion::Euler(localRot);

	randLength_ = 1000.0f;
	randRadius_ = 10.0f;

	transform_.scl = { 1.5f,1.5f,1.5f };
}

void GunEnemy::PlayBurstSound(void)
{
	AudioManager::GetInstance().Play(
		AudioList::AUDIO_TYPE::GUN_BURST_AK47,
		barrelPos_,
		5500.0f,
		false,
		0.95f
	);
}

void GunEnemy::FollowNone(void)
{
	// 対象フレームのワールド行列(大きさ、回転、位置)を取得する(先)
	auto mat = MV1GetFrameLocalWorldMatrix(follow_.modelId, 21);
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
}

void GunEnemy::FollowShot(void)
{
	auto enemy = std::dynamic_pointer_cast<EnemyGeorge>(parent_.lock());

	VECTOR gTarPos = enemy->GetFollowPos();

	// 対象フレームのワールド行列(大きさ、回転、位置)を取得する
	auto mat = MV1GetFrameLocalWorldMatrix(follow_.modelId, followFrameNo_);
	auto fraPos = MGetTranslateElem(mat);	// 行列から移動値を取り出す

	targetPos_ = gTarPos;

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

void GunEnemy::FollowReload(void)
{
	// 対象フレームのワールド行列(大きさ、回転、位置)を取得する
	auto mat = MV1GetFrameLocalWorldMatrix(follow_.modelId, followFrameNo_);
	auto fraPos = MGetTranslateElem(mat);	// 行列から移動値を取り出す
	auto rot = MGetRotElem(mat);	// 行列から回転を取り出す

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

void GunEnemy::SetBullet(std::shared_ptr<BulletBase> bullet)
{

	// 角度ランダム
	float radian = Utility::Deg2RadF((float)GetRand(360));

	// 半径ランダム
	float radius = GetRand((int)randRadius_ * 10) / 10.0f;


	// 発射方向を円状にばらけさせる
	// 起点
	auto sP = barrelPos_;
	// 終点
	auto eP = VAdd(sP, VScale(transform_.GetForward(), randLength_));




	// 円状の方向
	auto up = Quaternion::AngleAxis(radian, transform_.GetForward());
	auto sDir = up.GetUp();

	// 目標点
	auto oP = VAdd(eP, VScale(sDir, radius));

	// 始点から目標点へのベクトル
	auto vec = VNorm(VSub(oP, sP));

	// 発射準備
	bullet->ReadyBullet(barrelPos_, vec);

	magazine_ = maxMagazine_;
}
