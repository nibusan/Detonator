#pragma once
#include <string>
#include "../../../../Common/Handle/Font/Font.h"
#include "../../../UIBase.h"

class UI_ChangeableText : public UIBase {
public:
	UI_ChangeableText() = default;
	UI_ChangeableText(int messageID, std::string name, int size, unsigned int color);
	~UI_ChangeableText() = default;

	void Init_UI(void) override;
	bool Update_UI(void) override;
	void Draw_UI(void) override;
	void Release_UI(void) override;

private:
	// �`�悷��e�L�X�g
	std::string text_;

	// �g�p����t�H���g
	Shared_Font usingFont_;

	// �e�L�X�g�̐F
	unsigned int color_;

	// ��������X�V����֐�
	std::function<std::string(void)> updateText_;

	/// @brief �e�L�X�g���Z�b�g����
	/// @param text �e�L�X�g
	void SetText(const std::string& text);
};
