#pragma once
#include <DxLib.h>
#include "../../Common/Quaternion.h"
#include "../../Object/ObjectBase.h"

class CameraBase
{

public:

	// �J�����N���b�v�FNEAR
	static constexpr float CAMERA_NEAR = 10.0f;

	// �J�����N���b�v�FNEAR
	static constexpr float CAMERA_FAR = 15000.0f;

	// �J�����̏������W
	static constexpr VECTOR DEFAULT_CAMERA_POS = { 0.0f, 100.0f, -500.0f };

	// �J�����ʒu���璍���_�܂ł̑��΍��W
	static constexpr VECTOR RELATIVE_C2T_POS = { 0.0f, 0.0f, 500.0f };


	CameraBase(void);
	~CameraBase(void);

	// �J��������
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


	// �J�����̈ʒu
	VECTOR pos_;

	// �J�����̒����_
	VECTOR targetPos_;

	// �J�����̏����
	VECTOR cameraUp_;

	// �J�����̉�]
	Quaternion rot_;

	// �J�����̊p�x(rad)
	VECTOR angles_;

	// �J�����������ʒu�ɖ߂�
	void SetDefault(void);

};

