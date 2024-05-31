#pragma once
#include <string>
#include "../../../../Common/Handle/Font/Font.h"
#include "../../../UIBase.h"

class UI_Time : public UIBase {
public:
	UI_Time() = default;
	UI_Time(std::string name, int size, unsigned int color);
	~UI_Time() = default;

	void Init_UI(void) override;
	bool Update_UI(void) override;
	void Draw_UI(void) override;
	void Release_UI(void) override;
	const Weak_Font GetFont(void) const;
private:
	Shared_Font usingFont_;
	unsigned int color_;
};

