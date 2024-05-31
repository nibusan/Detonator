#include <DxLib.h>
#include "../../Manager/SceneManager.h"
#include "../../Utility/Utility.h"
#include "Transform.h"

Transform::Transform(void)
{
	modelId = -1;

	scl = Utility::VECTOR_ONE;
	rot = Utility::VECTOR_ZERO;
	pos = Utility::VECTOR_ZERO;
	localPos = Utility::VECTOR_ZERO;
	prePos = pos;

	matScl = MGetIdent();
	matRot = MGetIdent();
	matPos = MGetIdent();
	quaRot = Quaternion();
	quaRotLocal = Quaternion();


	// 非表示にするか
	isHide = false;

	// 重力をONにするか
	isGravity = false;

	// 重力加速度
	gravitySpeed_ = 0.0f;

	// 重力係数
	heavy = 1.0f;
}


Transform::~Transform(void)
{
}

void Transform::Update(void)
{

	// 大きさ
	matScl = MGetScale(scl);

	// 回転
	rot = quaRot.ToEuler();
	matRot = quaRot.ToMatrix();

	// 位置
	// ローカル
	auto lPos = VAdd(pos, quaRot.PosAxis(localPos));
	matPos = MGetTranslate(lPos);

	// 行列の合成
	MATRIX mat = MGetIdent();
	mat = MMult(mat, matScl);					// 大きさ
	Quaternion q = quaRot.Mult(quaRotLocal);	// 現在の回転量にローカル回転量を合成
	mat = MMult(mat, q.ToMatrix());				// 回転(クォータニオンから行列へ変換)
	mat = MMult(mat, matPos);					// 位置



	// 行列をモデルに判定
	if (modelId != -1)
	{
		MV1SetMatrix(modelId, mat);
	}

}

void Transform::Draw(void)
{
	// 非表示状態なら表示しない
	if (isHide) return;

	MV1DrawModel(modelId);
}

void Transform::Release(void)
{
	MV1DeleteModel(modelId);
}

void Transform::SetModel(int model)
{
	modelId = model;
}

void Transform::PreUpdate(void)
{
	// 大きさ
	preScl = scl;
	// 回転
	preRot = rot;
	// 位置
	prePos = pos;

	// 回転
	preQuaRot = quaRot;
}


VECTOR Transform::GetForward(void) const
{
	return GetDir(Utility::DIR_F);
}

VECTOR Transform::GetBack(void) const
{
	return GetDir(Utility::DIR_B);
}

VECTOR Transform::GetRight(void) const
{
	return GetDir(Utility::DIR_R);
}

VECTOR Transform::GetLeft(void) const
{
	return GetDir(Utility::DIR_L);
}

VECTOR Transform::GetUp(void) const
{
	return GetDir(Utility::DIR_U);
}

VECTOR Transform::GetDown(void) const
{
	return GetDir(Utility::DIR_D);
}

VECTOR Transform::GetDir(const VECTOR& vec) const
{
	return quaRot.PosAxis(vec);
}

void Transform::ResetGravityAccel(void)
{
	// 重力加速度
	gravitySpeed_ = 0.0f;
}

const float& Transform::GetGravityAccel(void) const
{
	return gravitySpeed_;
}

void Transform::Jump(float pow)
{
	// 重力加速度
	gravitySpeed_ -= pow;
}

bool Transform::IsRising(void)
{
	return (gravitySpeed_ < 0.0f);
}

bool Transform::IsFalling(void)
{
	return (gravitySpeed_ >= 0.0f);
}

void Transform::HideFrame(int frameNo)
{

	// 対象フレームのローカル行列(大きさ、回転、位置)を取得する
	auto mat = MV1GetFrameLocalMatrix(modelId, frameNo);
	auto scl = MGetSize(mat);			// 行列から大きさを取り出す
	auto rot = MGetRotElem(mat);		// 行列から回転を取り出す
	auto pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す

	// 大きさ、回転、位置をローカル行列に戻す
	MATRIX mix = MGetIdent();
	mix = MMult(mix, MGetScale({ 0.0f,0.0f,0.0f }));	// 大きさを0にする
	mix = MMult(mix, rot);							// 回転
	mix = MMult(mix, MGetTranslate(pos));			// 座標

	// 合成した行列を対象フレームにセットし直す
	MV1SetFrameUserLocalMatrix(modelId, frameNo, mix);
}

void Transform::AppearFrame(int frameNo)
{
	// 対象フレームのローカル行列(大きさ、回転、位置)を取得する
	auto mat = MV1GetFrameLocalMatrix(modelId, frameNo);
	auto scl = MGetSize(mat);			// 行列から大きさを取り出す
	auto rot = MGetRotElem(mat);		// 行列から回転を取り出す
	auto pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す

	// 大きさ、回転、位置をローカル行列に戻す
	MATRIX mix = MGetIdent();
	mix = MMult(mix, MGetScale({ 1.0f,1.0f,1.0f }));	// 大きさを1にする
	mix = MMult(mix, rot);							// 回転
	mix = MMult(mix, MGetTranslate(pos));			// 座標

	// 合成した行列を対象フレームにセットし直す
	MV1SetFrameUserLocalMatrix(modelId, frameNo, mix);
}

const bool Transform::IsHidingFrame(int frameNo)
{
	// 対象フレームのローカル行列(大きさ、回転、位置)を取得する
	auto mat = MV1GetFrameLocalMatrix(modelId, frameNo);
	auto scl = MGetSize(mat);			// 行列から大きさを取り出す

	return Utility::EqualsVZero(scl);
}

void Transform::Gravity(void)
{
	if (!isGravity)return;

	// 重力加速度
	gravitySpeed_ += SceneManager::GetInstance().GetDeltaMult(GRAVITY * heavy);

	// 落下
	pos = VAdd(pos, VScale({ 0.0f,-1.0f,0.0f }, gravitySpeed_));
}

