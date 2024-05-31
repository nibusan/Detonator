#include "../../Manager/SceneManager.h"
#include "../../Manager/AudioManager.h"
#include "../../Utility/Utility.h"
#include "../Actor/Player/Player.h"
#include "../Bullet/BulletSr.h"
#include "GunAWM.h"

GunAWM::GunAWM(const Transform& follow, int frameNo, std::shared_ptr<ActorBase> parent) : GunBase(follow, frameNo, parent)
{
}

void GunAWM::Draw_Object(void)
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

void GunAWM::CreateBullet(void)
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
	std::shared_ptr<BulletBase> tempBullet = std::make_shared<BulletSr>(bulletModelId_, parent_);

	tempBullet->Init();

	// 弾情報を設定
	SetBullet(tempBullet);

	// 配列に格納
	bullets_.emplace_back(tempBullet);

}

const VECTOR GunAWM::GetScopePos() const
{
	auto t = GetTransform();
	VECTOR ret = t.pos;
	ret = VAdd(ret, t.quaRot.PosAxis({ 0.0f,17.2f,12.3f }));
	return ret;
}

void GunAWM::PlayReloadSound(void)
{
	AudioManager::GetInstance().Play(
		AudioList::AUDIO_TYPE::GUN_RELOAD_AWM,
		transform_.pos,
		500.0f,
		false,
		1.0f
	);
}

void GunAWM::SetParam(void)
{

	// モデルファイル名
	fileName_ = "Model_Gun_AWM";

	// スコープモデルファイル名
	scopeFileName_ = "AWMScope.mv1";

	// 画像読み込み
	imgH_ = LoadGraph("Data/Image/SR_Scope.png");

	//// スコープ用Transform
	//scpTransform_.pos = { 0.0f,150.0f,0.0f };
	//scpTransform_.quaRotLocal = Quaternion::Euler({ 0.0f,Utility::Deg2RadF(180.0f),0.0f });
	//scpTransform_.scl = { 2.0f,2.0f,2.0f };
	//scpRelativePos_ = { 0.0f,17.2f,15.0f };

	// 弾のモデルファイル名
	bulletFileName_ = "sr.mv1";

	//// 追従フレーム番号(右手)
	//followFrameNo_ = 36;

	// バレルフレーム番号(弾の発射位置)
	barrelFrameNo_ = 0;

	// バレル相対座標
	barrelRelativePos_ = { 0.0f,15.0f,90.0f };

	// 最大弾数
	maxMagazine_ = 5;

	// 発射間隔
	shotDelay_ = 2.0f;

	// 相対座標
	relativePos_ = RELATIVE_POS;

	// 大きさ
	transform_.scl = { 1.0f,1.0f,1.0f };

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
	recoilMag_ = 10.0f;

	randLength_ = 30.0f;
	randRadius_ = 0.0f;

	// 腰撃ち時
	hipRandLength_ = 150.0f;
	hipRandRadius_ = 30.0f;

	// スコープ拡大倍率
	magnification_ = 4.0f;
}

void GunAWM::PlayBurstSound(void)
{
	AudioManager::GetInstance().Play(
		AudioList::AUDIO_TYPE::GUN_BURST_AWM,
		barrelPos_,
		5500.0f,
		false,
		1.0f
	);
}

void GunAWM::DrawTrans(void)
{
	// ライトの設定
	//SetUseLighting(false);
	//MV1DrawFrame(scopeModelId_, 1);
	//SetUseLighting(true);

	auto player = std::dynamic_pointer_cast<Player>(parent_.lock());
	if (player->IsADS() && !player->IsAct() && player->IsMe() && player->GetHP() > 0.0f)
	{
		DrawGraph(0, 0, imgH_, true);
	}

}

void GunAWM::Release_Object(void)
{
	// モデル削除(仮)
	transform_.Release();

	// 弾削除(仮)
	for (auto& b : bullets_)
	{
		b->Release();
	}
	bullets_.clear();
	DeleteGraph(imgH_);
}
