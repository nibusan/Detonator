#pragma once
#include "CameraBase.h"

class NormalCamera : public CameraBase
{
public:

	// �J�������[�h
	enum class MODE
	{
		NONE,
		FIXED_POINT,	// ��_�J����
		DEBUG,			// �f�o�b�O�p�J����
	};

	NormalCamera(void);
	~NormalCamera(void);

	void Init(void) override;
	void Update(void);

	// �J��������
	void SetBeforeDraw(void) override;
	void SetBeforeDrawFixedPoint(void);

private:

	// �J�������[�h
	MODE mode_;

};

