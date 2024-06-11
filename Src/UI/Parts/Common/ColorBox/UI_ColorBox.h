#pragma once
#include "../../../UIBase.h"
#include "../../Image/UI_Image.h"
#include "../../Text/UI_Text.h"

class UI_ColorBox : public UIBase {
public:
	UI_ColorBox(void) = default;
	UI_ColorBox(unsigned int color);
	~UI_ColorBox(void) = default;

	void Init_UI(void) override;
	bool Update_UI(void) override;
	void Draw_UI(void) override;
	void Release_UI(void) override;
private:
	// ìhÇËÇ¬Ç‘Ç∑êF
	unsigned int color_;
};
