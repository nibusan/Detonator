#pragma once
#include <string>
#include "Vector2.h"
#include "../Manager/InputTextManager.h"

// �}�E�X�ŋ�`�����N���b�N��������͂��󂯕t����N���X
// �ėp���Ȃ��B���쐬�B
class InputTextArea
{

public:

	const unsigned int COLOR_BACK_DEFAULT = 0x000000;
	const unsigned int COLOR_BACK_ACTIVE = 0x2f4f4f;
	const unsigned int COLOR_CHAR = 0xffffff;
	const unsigned int COLOR_FRAME = 0xffffff;

	InputTextArea(Vector2<int> pos, Vector2<int> size, int length);
	~InputTextArea(void);

	// �X�V�X�e�b�v
	void Update(void);

	// �`��
	void Draw(void);

	// �������
	void Release(void);

	// �L�[���̓n���h�����擾����
	int GetKeyHandle(void);

	// �e�L�X�g���Z�b�g
	std::string GetText(void);
	void SetText(std::string text);

	// ���͓��e���Z�b�g
	void SetKeyInputStringBuffer(void);

private:

	InputTextManager& manager_;

	// �L�[���̓n���h��
	int keyInputHandle_;

	int mouseInputOld_;
	int mouseInputNew_;

	// ��`�̍�����W
	Vector2<int> pos_;

	// ��`�̑傫��
	Vector2<int> size_;

	// ������
	int length_;

	// ���̓o�b�t�@
	char buffer_[256];

	// ���̓e�L�X�g
	std::string text_;

	// �`��
	void DrawDefault(void);
	void DrawActive(void);

};
