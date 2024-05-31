#pragma once
#include "../../../UIBase.h"
#include "../../Image/UI_Image.h"
#include "../../Text/UI_Text.h"

class UI_MessageBox : public UIBase {
public:
	UI_MessageBox(void) = default;
	UI_MessageBox(Weak_Graphic graphic, std::string text, std::string name, int size, unsigned int color, std::function<std::string(void)> getAddTextFunction);
	~UI_MessageBox(void) = default;

	void Init_UI(void) override;
	bool Update_UI(void) override;
	void Draw_UI(void) override;
	void Release_UI(void) override;

	void SetAddText(const std::string& text);
private:
	// �摜
	Weak_Graphic graphic_;

	// ������\������e�L�X�g
	std::string defText_;

	// �ォ��ϓ��܂��͒ǉ�����e�L�X�g
	std::string addText_;

	Shared_Font usingFont_;
	unsigned int color_;
	std::function<std::string(void)> getAddTextFunction_;
};
