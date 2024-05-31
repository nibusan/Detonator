#include "../../Manager/SceneManager.h"
#include "../../Manager/AudioManager.h"
#include "../../Utility/Utility.h"
#include "../Bullet/BulletAR.h"
#include "GunAK47.h"

GunAK47::GunAK47(const Transform& follow, int frameNo, std::weak_ptr<ActorBase> parent) : GunBase(follow, frameNo, parent)
{

}

void GunAK47::CreateBullet(void)
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
	std::shared_ptr<BulletBase> tempBullet = std::make_shared<BulletAR>(bulletModelId_, parent_);

	tempBullet->Init();

	// 弾情報を設定
	SetBullet(tempBullet);

	// 配列に格納
	bullets_.emplace_back(tempBullet);

}

const VECTOR GunAK47::GetScopePos() const
{
	auto t = GetTransform();
	VECTOR ret = t.pos;
	//ret = VAdd(ret, t.quaRot.PosAxis({ 0.0f,18.1f,-25.0f }));
	ret = VAdd(ret, t.quaRot.PosAxis({ 0.0f,18.1f,-15.0f }));
	return ret;
}

void GunAK47::PlayReloadSound(void)
{
	AudioManager::GetInstance().Play(
		AudioList::AUDIO_TYPE::GUN_RELOAD_AK47,
		transform_.pos,
		500.0f,
		false,
		0.7f
	);
}

void GunAK47::SetParam(void)
{
	// モデルファイル名
	fileName_ = "Model_Gun_AK47";

	// スコープモデルファイル名
	scopeFileName_ = "Red+Dot+Sight.mv1";

	// スコープ用Transform
	scpTransform_.pos = {0.0f,150.0f,0.0f};
	scpTransform_.quaRotLocal = Quaternion::Euler({0.0f,Utility::Deg2RadF(90.0f),0.0f});
	scpTransform_.scl = { 2.0f,2.0f,2.0f };
	scpRelativePos_ = {0.0f,12.0f,-3.0f};

	// 弾のモデルファイル名
	bulletFileName_ = "ar.mv1";



	//// 追従フレーム番号(右手)
	//followFrameNo_ = 36;

	// バレルフレーム番号(弾の発射位置)
	barrelFrameNo_ = 20;

	// 最大弾数
	maxMagazine_ = 30;

	// 発射間隔
	shotDelay_ = 0.1f;

	// 相対座標
	relativePos_ = RELATIVE_POS;

	// ローカル回転
	VECTOR localRot = { Utility::Deg2RadF(0.0f),Utility::Deg2RadF(90.0f),Utility::Deg2RadF(0.0f) };
	transform_.quaRotLocal = Quaternion::Euler(localRot);

	// リコイルパターン設定
	recoil_[0] = Recoil::RECOIL_TYPE::UP;
	recoil_[1] = Recoil::RECOIL_TYPE::UP;
	recoil_[2] = Recoil::RECOIL_TYPE::RIGHT;
	recoil_[3] = Recoil::RECOIL_TYPE::RIGHT;
	recoil_[4] = Recoil::RECOIL_TYPE::LEFT;
	recoil_[5] = Recoil::RECOIL_TYPE::LEFT;

	// 発射時の反動の大きさ
	recoilMag_ = 5.0f;

	randLength_ = 1000.0f;
	randRadius_ = 10.0f;

	// 腰撃ち時
	hipRandLength_ = 700.0f;
	hipRandRadius_ = 30.0f;

	// スコープ拡大倍率
	magnification_ = 2.0f;
}

void GunAK47::PlayBurstSound(void)
{
	AudioManager::GetInstance().Play(
		AudioList::AUDIO_TYPE::GUN_BURST_AK47,
		barrelPos_,
		5500.0f,
		false,
		0.95f
	);
}

//void GunAK47::SetBullet(std::shared_ptr<BulletBase> bullet)
//{
//	// 発射位置にリセット(表示させる)
//	
//	// 発射方向を円状にばらけさせる
//	// 起点
//	auto sP = barrelPos_;
//	// 終点
//	auto eP = VAdd(sP, VScale(transform_.GetForward(), StoE_LENGTH));
//
//	// 角度ランダム
//	float radian = Utility::Deg2RadF((float)GetRand(360));
//
//	// 半径ランダム
//	float radius = GetRand((int)RADIUS * 10) / 10.0f;
//
//	// 円状の方向
//	auto up = Quaternion::AngleAxis(radian, transform_.GetForward());
//	auto sDir = up.GetUp();
//
//	// 目標点
//	auto oP = VAdd(eP, VScale(sDir, radius));
//
//	// 始点から目標点へのベクトル
//	auto vec = VNorm(VSub(oP, sP));
//	
//
//	// 発射準備
//	bullet->ReadyBullet(barrelPos_, vec);
//}
