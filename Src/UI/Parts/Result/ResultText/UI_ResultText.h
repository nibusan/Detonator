#pragma once
#include "../../../UIBase.h"
#include "../../Image/UI_Image.h"
#include "../../Text/UI_Text.h"

class UI_ResultText : public UIBase {
public:
	UI_ResultText(void) = default;
	UI_ResultText(
		const std::string& text, 
		const std::string& name, 
		int size, 
		unsigned int color, 
		const std::function<std::string(void)>& getAddTextFunction
	);
	~UI_ResultText(void) = default;

	void Init_UI(void) override;
	bool Update_UI(void) override;
	void Draw_UI(void) override;
	void Release_UI(void) override;

	void SetAddText(const std::string& text);
private:
	// 元から表示するテキスト
	std::string defText_;

	// 後から変動または追加するテキスト
	std::string addText_;

	Shared_Font usingFont_;
	unsigned int color_;
	std::function<std::string(void)> getAddTextFunction_;

};
