#include "../../Manager/SceneManager.h"
#include "../../Manager/AudioManager.h"
#include "../../Utility/Utility.h"
#include "../Bullet/BulletSub.h"
#include "GunM24R.h"

GunM24R::GunM24R(const Transform& follow, int frameNo, std::weak_ptr<ActorBase> parent) : GunBase(follow, frameNo, parent)
{
}

void GunM24R::CreateBullet(void)
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
	std::shared_ptr<BulletBase> tempBullet = std::make_shared<BulletSub>(bulletModelId_, parent_);

	tempBullet->Init();

	// 弾情報を設定
	SetBullet(tempBullet);

	// 配列に格納
	bullets_.emplace_back(tempBullet);

}

const VECTOR GunM24R::GetScopePos() const
{
	auto t = GetTransform();
	VECTOR ret = t.pos;
	ret = VAdd(ret, t.quaRot.PosAxis({ 0.0f,11.0f,-3.0f }));
	//deg_ = ret;
	return ret;
}

void GunM24R::PlayReloadSound(void)
{
	AudioManager::GetInstance().Play(
		AudioList::AUDIO_TYPE::GUN_RELOAD_M24R,
		transform_.pos,
		500.0f,
		false,
		1.0f
	);
}

void GunM24R::SetParam(void)
{

	// モデルファイル名
	fileName_ = "Model_Gun_M24R";

	// スコープモデルファイル名
	scopeFileName_ = "Red+Dot+Sight.mv1";

	// スコープ用Transform
	scpTransform_.pos = { 0.0f,150.0f,0.0f };
	scpTransform_.quaRotLocal = Quaternion::Euler({ 0.0f,Utility::Deg2RadF(90.0f),0.0f });
	scpTransform_.scl = { 2.0f,2.0f,2.0f };
	scpRelativePos_ = { 0.0f,5.0f,5.0f };

	// 弾のモデルファイル名
	bulletFileName_ = "sub.mv1";

	//// 追従フレーム番号(右手)
	//followFrameNo_ = 36;

	// バレルフレーム番号(弾の発射位置)
	barrelFrameNo_ = 0;

	// バレル相対座標
	barrelRelativePos_ = { 0.0f,5.0f,30.0f };

	// 最大弾数
	maxMagazine_ = 25;

	// 発射間隔
	shotDelay_ = 0.05f;

	// 相対座標
	relativePos_ = RELATIVE_POS;

	// 大きさ
	transform_.scl = { 8.0f,8.0f,8.0f };

	// ローカル回転
	VECTOR localRot = { Utility::Deg2RadF(0.0f),Utility::Deg2RadF(180.0f),Utility::Deg2RadF(0.0f) };
	transform_.quaRotLocal = Quaternion::Euler(localRot);

	// リコイルパターン設定
	recoil_[0] = Recoil::RECOIL_TYPE::UP;
	recoil_[1] = Recoil::RECOIL_TYPE::UP;
	recoil_[2] = Recoil::RECOIL_TYPE::RIGHT;
	recoil_[3] = Recoil::RECOIL_TYPE::RIGHT;
	recoil_[4] = Recoil::RECOIL_TYPE::LEFT;
	recoil_[5] = Recoil::RECOIL_TYPE::LEFT;

	// 発射時の反動の大きさ
	recoilMag_ = 8.0f;

	randLength_ = 400.0f;
	randRadius_ = 20.0f;

	// 腰撃ち時
	hipRandLength_ = 200.0f;
	hipRandRadius_ = 30.0f;

	// スコープ拡大倍率
	magnification_ = 1.5f;
}

void GunM24R::PlayBurstSound(void)
{
	AudioManager::GetInstance().Play(
		AudioList::AUDIO_TYPE::GUN_BURST_M24R,
		barrelPos_,
		5500.0f,
		false,
		0.8f
	);
}
