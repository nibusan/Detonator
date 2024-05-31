#pragma once
#include "CameraBase.h"

class NormalCamera : public CameraBase
{
public:

	// カメラモード
	enum class MODE
	{
		NONE,
		FIXED_POINT,	// 定点カメラ
		DEBUG,			// デバッグ用カメラ
	};

	NormalCamera(void);
	~NormalCamera(void);

	void Init(void) override;
	void Update(void);

	// カメラ処理
	void SetBeforeDraw(void) override;
	void SetBeforeDrawFixedPoint(void);

private:

	// カメラモード
	MODE mode_;

};

