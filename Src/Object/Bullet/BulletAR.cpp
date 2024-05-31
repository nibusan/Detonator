#include "../../Utility/Utility.h"
#include "BulletAR.h"

BulletAR::BulletAR(int modelId, std::weak_ptr<ActorBase> parent) : BulletBase(modelId , parent)
{
	
}

void BulletAR::SetParam(void)
{

	// スピード
	speed_ = 200.0f;

	// 生存時間
	aliveTime_ = 1.0f;

	// ダメージ
	damage_ = 15.0f;

	// ローカル回転
	// Z軸方向を前に
	VECTOR localRot = { Utility::Deg2RadF(90.0f),Utility::Deg2RadF(0.0f),Utility::Deg2RadF(0.0f) };
	transform_.quaRotLocal = Quaternion::Euler(localRot);

	// 大きさ
	float scale = 0.1f;
	transform_.scl = { scale,scale,scale };



}
