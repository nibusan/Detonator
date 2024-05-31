#include "../../Utility/Utility.h"
#include "BulletSub.h"

BulletSub::BulletSub(int modelId, std::weak_ptr<ActorBase> parent) : BulletBase(modelId , parent)
{
	transform_.SetModel(MV1DuplicateModel(modelId));
}

void BulletSub::SetParam(void)
{

	// スピード
	speed_ = 200.0f;

	// 生存時間
	aliveTime_ = 1.0f;

	// ダメージ
	damage_ = 11.0f;

	// ローカル回転
	// Z軸方向を前に
	VECTOR localRot = { Utility::Deg2RadF(90.0f),Utility::Deg2RadF(0.0f),Utility::Deg2RadF(0.0f) };
	transform_.quaRotLocal = Quaternion::Euler(localRot);

	// 大きさ
	float scale = 0.1f;
	transform_.scl = { scale,scale,scale };


}
