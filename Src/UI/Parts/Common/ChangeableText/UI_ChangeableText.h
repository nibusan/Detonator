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
	std::string text_;
	Shared_Font usingFont_;
	unsigned int color_;

	void SetText(const std::string& text);
	std::function<std::string(void)> updateText_;
};
