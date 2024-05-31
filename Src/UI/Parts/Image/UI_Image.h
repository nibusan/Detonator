#pragma once
#include <string>
#include "../../../Common/Handle/Graphic/Graphic.h"
#include "../../UIBase.h"

class UI_Image: public UIBase {
public:
	UI_Image() = default;
	UI_Image(Weak_Graphic graphic);
	~UI_Image() = default;

	void Init_UI(void) override;
	bool Update_UI(void) override;
	void Draw_UI(void) override;
	void Release_UI(void) override;

	const Weak_Graphic GetGraphic(void) const;
	void SetGraphic(Weak_Graphic graphic);

	Weak_PixelShader GetUsingPixelShader(void) const;

private:
	// ‰æ‘œ
	Weak_Graphic graphic_;

	void HighlightedUpdate(void) override;
};

