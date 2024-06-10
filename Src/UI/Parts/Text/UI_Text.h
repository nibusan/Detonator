#pragma once
#include <string>
#include "../../../Common/Handle/Font/Font.h"
#include "../../UIBase.h"

class UI_Text : public UIBase {
public:
	UI_Text() = default;
	~UI_Text() = default;

	/// @brief コンストラクタ
	/// @param text 描画するテキスト
	/// @param name 使用するフォントの名前
	/// @param size フォントのサイズ
	/// @param color テキストの色
	UI_Text(std::string text, std::string name, int size, unsigned int color);

	void Init_UI(void) override;
	bool Update_UI(void) override;
	void Draw_UI(void) override;
	void Release_UI(void) override;

	/// @brief 描画するテキストを返す 
	/// @return テキスト
	const std::string& GetText(void) const;

	/// @brief 描画するテキストをセットする
	/// @param text テキスト
	void SetText(const std::string& text);

	/// @brief 描画する際に使用するフォントを生成する
	/// @param name フォントの名前
	/// @param size フォントのサイズ
	void CreateFont(std::string name, int size);

	/// @brief 描画する際に使用するフォントを返す
	/// @return フォント
	const Weak_Font GetFont(void) const;

private:
	// テキスト
	std::string text_;

	// 使用するフォント
	Shared_Font usingFont_;

	// テキストの色
	unsigned int color_;

	std::function<std::string(void)> updateText_;
};

