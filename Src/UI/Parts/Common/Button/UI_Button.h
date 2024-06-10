#pragma once
#include "../../../UIBase.h"
#include "../../Image/UI_Image.h"
#include "../../Text/UI_Text.h"

class UI_Button : public UIBase {
public:
	UI_Button(void) = default;
	UI_Button(
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
	~UI_Button(void) = default;

	void Init_UI(void) override;
	bool Update_UI(void) override;
	void Draw_UI(void) override;
	void Release_UI(void) override;
private:
	// �摜
	Weak_Graphic graphic_;

	// �g�p����s�N�Z���V�F�[�_�[
	Weak_PixelShader pixelShader_;

	// �`�悷��e�L�X�g�A�t�H���g�A�F
	std::string text_;
	Shared_Font usingFont_;
	unsigned int color_;

	// �`�悷��e�L�X�g�A�t�H���g�A�F(�⑫�p)
	std::string text2_;
	Shared_Font usingFont2_;
	unsigned int color2_;

	// 
	float scaleRad_;
	float colorValue_;

	void HighlightedUpdate(void) override;
};
