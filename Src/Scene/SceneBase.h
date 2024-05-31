#pragma once
#include "../Common/Handle/Graphic/Graphic.h"

class SceneBase
{

public:

	// �R���X�g���N�^
	SceneBase(void);

	// �f�X�g���N�^
	virtual ~SceneBase(void) = 0;

	// ����������
	virtual void Init(void) = 0;

	// �X�V�X�e�b�v
	virtual void Update(void) = 0;

	// �`�揈��
	virtual void Draw(void) = 0;

	// �������
	virtual void Release(void) = 0;

	// �`��X�N���[����Ԃ�
	[[nodiscard]] Shared_Graphic GetDrawScreen(void) const;

protected:
	Shared_Graphic sceneScreen_;

};
