#pragma once
#include "../../../UIBase.h"
#include "../../Image/UI_Image.h"
#include "../../Text/UI_Text.h"

class UI_Cursor : public UIBase {
public:
	// カーソルの向き
	enum class DIR {
		RIGHT = 0,
		LEFT
	};

	UI_Cursor(void) = default;
	UI_Cursor(Weak_Graphic graphic, DIR dir);
	~UI_Cursor(void) = default;

	void Init_UI(void) override;
	bool Update_UI(void) override;
	void Draw_UI(void) override;
	void Release_UI(void) override;
private:
	// 画像
	Weak_Graphic graphic_;

	// カーソルの向き
	DIR dir_;
};
