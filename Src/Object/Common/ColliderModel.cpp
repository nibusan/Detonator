#include "../ObjectBase.h"
#include "ColliderModel.h"

ColliderModel::ColliderModel(std::weak_ptr<ObjectBase> obj, Collider::OBJECT_TYPE type, int model)
	: Collider(obj)
{
	// 衝突タイプ
	colType = COL_TYPE::MODEL;

	// オブジェクトタイプ
	objType = type;

	// モデルID
	modelId_ = model;

	// 当たり判定(コライダ)作成(最適化されたもの)
	int ret = MV1SetupCollInfo(modelId_, -1, 1, 1, 1);
}

void ColliderModel::Update(VECTOR followPos, Quaternion rot)
{
	// 回転
	quaRot = rot;

	// 相対座標を加えて更新
	pos = VAdd(followPos, rot.PosAxis(relativePos));

	// モデルコリジョン用のinfoをDxLib側で更新
	MV1RefreshCollInfo(modelId_, -1);
}

void ColliderModel::Update(VECTOR followPos, VECTOR rot)
{
	// 回転
	quaRot = rot;

	// 相対座標を加えて更新
	Quaternion q = Quaternion::Euler(rot);
	pos = VAdd(followPos, q.PosAxis(relativePos));

	// モデルコリジョン用のinfoをDxLib側で更新
	MV1RefreshCollInfo(modelId_, -1);
}

void ColliderModel::Update(void)
{
	// モデルコリジョン用のinfoをDxLib側で更新
	MV1RefreshCollInfo(modelId_, -1);
}

int ColliderModel::GetModelId(void) const
{
	return modelId_;
}

void ColliderModel::HideFrame(int frameNo)
{
	// 対象フレームのローカル行列(大きさ、回転、位置)を取得する
	auto mat = MV1GetFrameLocalMatrix(modelId_, frameNo);
	auto scl = MGetSize(mat);			// 行列から大きさを取り出す
	auto rot = MGetRotElem(mat);		// 行列から回転を取り出す
	auto pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す

	// 大きさ、回転、位置をローカル行列に戻す
	MATRIX mix = MGetIdent();
	mix = MMult(mix, MGetScale({ 0.0f,0.0f,0.0f }));	// 大きさを0にする
	mix = MMult(mix, rot);							// 回転
	mix = MMult(mix, MGetTranslate({0.0f,-100.0f,0.0f}));			// 座標

	// 合成した行列を対象フレームにセットし直す
	MV1SetFrameUserLocalMatrix(modelId_, frameNo, mix);
}

void ColliderModel::AppearFrame(int frameNo)
{
	// 対象フレームのローカル行列(大きさ、回転、位置)を取得する
	auto mat = MV1GetFrameLocalMatrix(modelId_, frameNo);
	auto scl = MGetSize(mat);			// 行列から大きさを取り出す
	auto rot = MGetRotElem(mat);		// 行列から回転を取り出す
	auto pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す

	// 大きさ、回転、位置をローカル行列に戻す
	MATRIX mix = MGetIdent();
	mix = MMult(mix, MGetScale({ 1.0f,1.0f,1.0f }));	// 大きさを1にする
	mix = MMult(mix, rot);							// 回転
	mix = MMult(mix, MGetTranslate(pos));			// 座標

	// 合成した行列を対象フレームにセットし直す
	MV1SetFrameUserLocalMatrix(modelId_, frameNo, mix);
}

VECTOR ColliderModel::GetScaleFrame(int frameNo) const
{
	// 対象フレームのローカル行列(大きさ、回転、位置)を取得する
	auto mat = MV1GetFrameLocalMatrix(modelId_, frameNo);
	auto scl = MGetSize(mat);			// 行列から大きさを取り出す
	return scl;
}
