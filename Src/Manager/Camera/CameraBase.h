#pragma once
#include <DxLib.h>
#include "../../Common/Quaternion.h"
#include "../../Object/ObjectBase.h"

class CameraBase
{

public:

	// カメラクリップ：NEAR
	static constexpr float CAMERA_NEAR = 10.0f;

	// カメラクリップ：NEAR
	static constexpr float CAMERA_FAR = 15000.0f;

	// カメラの初期座標
	static constexpr VECTOR DEFAULT_CAMERA_POS = { 0.0f, 100.0f, -500.0f };

	// カメラ位置から注視点までの相対座標
	static constexpr VECTOR RELATIVE_C2T_POS = { 0.0f, 0.0f, 500.0f };


	CameraBase(void);
	~CameraBase(void);

	// カメラ処理
	virtual void SetBeforeDraw(void);

	virtual void Init(void);
	virtual void Update(void);
	virtual void Draw(void);
	virtual void Release(void);

	virtual void NetUpdate(void);

	const VECTOR& GetPos(void) const;
	const VECTOR& GetAngle(void) const;
	const Quaternion& GetRot(void)const;

	VECTOR* GetCameraPos(void);

protected:


	// カメラの位置
	VECTOR pos_;

	// カメラの注視点
	VECTOR targetPos_;

	// カメラの上方向
	VECTOR cameraUp_;

	// カメラの回転
	Quaternion rot_;

	// カメラの角度(rad)
	VECTOR angles_;

	// カメラを初期位置に戻す
	void SetDefault(void);

};

