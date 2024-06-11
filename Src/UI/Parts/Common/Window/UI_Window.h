#pragma once
#include "../../../UIBase.h"
#include "../../Image/UI_Image.h"
#include "../../Text/UI_Text.h"
#include "../../../../Common/Vector2.h"

class UI_Window : public UIBase {
public:

	UI_Window(void) = default;
	UI_Window(
		Weak_Graphic graphic, 
		const std::string& text, 
		const std::string& name, 
		int size, 
		unsigned int color, 
		const std::string& text2, 
		const std::string& name2, 
		int size2, 
		unsigned int color2
	);
	~UI_Window(void) = default;

	void Init_UI(void) override;
	bool Update_UI(void) override;
	void Draw_UI(void) override;
	void Release_UI(void) override;

	/// @brief �{�^����ǉ�
	/// @param button �{�^��
	void AddButton(std::shared_ptr<UIBase> button);
private:
	// �摜
	Weak_Graphic graphic_;

	// �E�B���h�E�̃^�C�g��
	std::string windowText_;
	std::string windowText2_;

	// �g�p����t�H���g
	Shared_Font usingFont_;
	Shared_Font usingFont2_;

	// �e�L�X�g�̐F
	unsigned int color_;
	unsigned int color2_;

	// �E�B���h�E�ɂ���{�^��UI���i�[
	std::vector<std::shared_ptr<UIBase>> buttons_;
	
};
