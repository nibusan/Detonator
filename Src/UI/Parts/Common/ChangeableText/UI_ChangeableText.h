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
	// 描画するテキスト
	std::string text_;

	// 使用するフォント
	Shared_Font usingFont_;

	// テキストの色
	unsigned int color_;

	// 文字列を更新する関数
	std::function<std::string(void)> updateText_;

	/// @brief テキストをセットする
	/// @param text テキスト
	void SetText(const std::string& text);
};
