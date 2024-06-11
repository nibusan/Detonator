#pragma once
#include "../../../UIBase.h"
#include "../../Image/UI_Image.h"
#include "../../Text/UI_Text.h"

class UI_InputTextArea : public UIBase {
public:
	// ���͂����e�L�X�g�̎��
	enum class INPUT_TYPE {
		HALF_WIDTH,	// ���p
		FULL_WIDTH,	// �S�p
	};

	// ���͂��󂯕t����e�L�X�g�̎��
	enum class INPUT_TEXT_TYPE {
		NUMBER,		// �����̂�
		CHARACTER	// �������܂ޑS�Ă̕���
	};

	UI_InputTextArea(void) = default;
	UI_InputTextArea(
		Weak_Graphic graphic,
		const std::string& name, 
		int size, 
		unsigned int color, 
		INPUT_TYPE inputType, 
		INPUT_TEXT_TYPE inputTextType, 
		int maxLength,
		const std::function<void(std::weak_ptr<UIBase>)>& endInputFunction
	);
	~UI_InputTextArea(void) = default;

	void Init_UI(void) override;
	bool Update_UI(void) override;
	void Draw_UI(void) override;
	void Release_UI(void) override;

	const std::string& GetInputString(void) const;
private:
	// �摜
	Weak_Graphic graphic_;

	// ���͂��Ă���e�L�X�g
	std::string inputText_;

	// �g�p����t�H���g
	Shared_Font usingFont_;

	// �e�L�X�g�̐F
	unsigned int color_;

	// ���͂���镶���̎��
	INPUT_TYPE inputType_;

	// ���͂��󂯕t����e�L�X�g�̎��
	INPUT_TEXT_TYPE inputTextType_;

	// ���͂ł��镶�����̍ő吔
	int maxLength_;

	// ���͂��I������ۂɌĂ΂��R�[���o�b�N�֐�
	std::function<void(std::weak_ptr<UIBase>)> endInputFunction_;

	// ���͂��󂯕t����L�[�̃n���h��
	int inputKeyHandle_;

	// ���݂̏��
	int inputKeyState_;

	// �O�t���[���̏��
	int preInputKeyState_;

	// ���͂���镶������ꎞ�I�Ɋi�[����o�b�t�@
	char inputTextBuffer_[256];

	void HighlightedUpdate(void) override;
};
