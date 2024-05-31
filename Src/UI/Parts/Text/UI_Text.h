#pragma once
#include <string>
#include "../../../Common/Handle/Font/Font.h"
#include "../../UIBase.h"

class UI_Text : public UIBase {
public:
	UI_Text() = default;
	UI_Text(std::string text, std::string name, int size, unsigned int color);
	~UI_Text() = default;

	void Init_UI(void) override;
	bool Update_UI(void) override;
	void Draw_UI(void) override;
	void Release_UI(void) override;
	const std::string& GetText(void) const;
	void SetText(const std::string& text);
	void CreateFont(std::string name, int size);
	const Weak_Font GetFont(void) const;

private:
	std::string text_;
	Shared_Font usingFont_;
	unsigned int color_;

	std::function<std::string(void)> updateText_;
};

