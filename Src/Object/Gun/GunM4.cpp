#include "../../Manager/SceneManager.h"
#include "../../Manager/AudioManager.h"
#include "../../Utility/Utility.h"
#include "../Bullet/BulletAR.h"
#include "../Actor/Player/Player.h"
#include "GunM4.h"

GunM4::GunM4(const Transform& follow, int frameNo, std::weak_ptr<ActorBase> parent) : GunBase(follow, frameNo, parent)
{
}

void GunM4::CreateBullet(void)
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

	// ダメージ設定
	for (auto& b : bullets_)
	{
		b->SetDamage(13.0f);
	}

	// 弾情報を設定
	SetBullet(tempBullet);

	// 配列に格納
	bullets_.emplace_back(tempBullet);

}

const VECTOR GunM4::GetScopePos() const
{
	auto t = GetTransform();
	VECTOR ret = t.pos;
	ret = VAdd(ret, t.quaRot.PosAxis({ 0.0f,13.5f,-23.5f }));
	//deg_ = ret;
	return ret;
}

void GunM4::PlayReloadSound(void)
{
	AudioManager::GetInstance().Play(
		AudioList::AUDIO_TYPE::GUN_RELOAD_M4,
		transform_.pos,
		500.0f,
		false,
		0.75f
	);
}

void GunM4::SetParam(void)
{
	// モデルファイル名
	fileName_ = "Model_Gun_M4";

	// スコープモデルファイル名
	scopeFileName_ = "Red+Dot+Sight.mv1";

	// スコープ用Transform
	scpTransform_.pos = { 0.0f,150.0f,0.0f };
	scpTransform_.quaRotLocal = Quaternion::Euler({ 0.0f,Utility::Deg2RadF(90.0f),0.0f });
	scpTransform_.scl = { 2.0f,2.0f,2.0f };
	scpRelativePos_ = { 0.0f,7.5f,-12.0f };

	// 弾のモデルファイル名
	bulletFileName_ = "ar.mv1";

	//// 追従フレーム番号(右手)
	//followFrameNo_ = 36;

	// バレルフレーム番号(弾の発射位置)
	barrelFrameNo_ = 0;

	// バレル相対座標
	barrelRelativePos_ = { 0.0f,6.0f,52.5f };

	// 最大弾数
	maxMagazine_ = 28;

	// 発射間隔
	shotDelay_ = 0.1f;

	// 大きさ
	transform_.scl = { 1.5f,1.5f,1.5f };

	// 相対座標
	relativePos_ = RELATIVE_POS;

	// ローカル回転
	VECTOR localRot = { Utility::Deg2RadF(0.0f),Utility::Deg2RadF(-90.0f),Utility::Deg2RadF(0.0f) };
	transform_.quaRotLocal = Quaternion::Euler(localRot);

	// リコイルパターン設定
	recoil_[0] = Recoil::RECOIL_TYPE::UP;
	recoil_[1] = Recoil::RECOIL_TYPE::UP;
	recoil_[2] = Recoil::RECOIL_TYPE::RIGHT;
	recoil_[3] = Recoil::RECOIL_TYPE::RIGHT;
	recoil_[4] = Recoil::RECOIL_TYPE::LEFT;
	recoil_[5] = Recoil::RECOIL_TYPE::LEFT;

	// 発射時の反動の大きさ
	recoilMag_ = 3.5f;

	randLength_ = 1200.0f;
	randRadius_ = 8.0f;
	// 腰撃ち時
	hipRandLength_ = 800.0f;
	hipRandRadius_ = 15.0f;

	// スコープ拡大倍率
	magnification_ = 2.0f;	

}

void GunM4::PlayBurstSound(void)
{
	AudioManager::GetInstance().Play(
		AudioList::AUDIO_TYPE::GUN_BURST_M4,
		barrelPos_,
		5500.0f,
		false,
		0.8f
	);
}

void GunM4::Draw_Object(void)
{
	auto player = std::dynamic_pointer_cast<Player>(parent_.lock());
	if (player->IsADS() && !player->IsAct() && player->IsMe())
	{
	}
	else
	{
		// モデル描画
		transform_.Draw();
	}

	// 弾描画
	for (auto& b : bullets_)
	{
		b->Draw();
	}

	MV1DrawFrame(scopeModelId_, 0);
}
