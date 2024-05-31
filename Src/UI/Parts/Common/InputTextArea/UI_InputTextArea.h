#pragma once
#include "../../../UIBase.h"
#include "../../Image/UI_Image.h"
#include "../../Text/UI_Text.h"

class UI_InputTextArea : public UIBase {
public:
	// 入力されるテキストの種類
	enum class INPUT_TYPE {
		HALF_WIDTH,	// 半角
		FULL_WIDTH,	// 全角
	};

	// 入力を受け付けるテキストの種類
	enum class INPUT_TEXT_TYPE {
		NUMBER,		// 数字のみ
		CHARACTER	// 数字を含む全ての文字
	};

	UI_InputTextArea(void) = default;
	UI_InputTextArea(
		Weak_Graphic graphic,
		const std::string& name, 
		int size, 
		unsigned int color, 
		INPUT_TYPE inputType, 
		INPUT_TEXT_TYPE inputTextType, 
		int maxLength,
		const std::function<void(std::weak_ptr<UIBase>)>& endInputFunction
	);
	~UI_InputTextArea(void) = default;

	void Init_UI(void) override;
	bool Update_UI(void) override;
	void Draw_UI(void) override;
	void Release_UI(void) override;

	const std::string& GetInputString(void) const;
private:
	// 画像
	Weak_Graphic graphic_;

	std::string inputText_;
	Shared_Font usingFont_;
	unsigned int color_;
	INPUT_TYPE inputType_;
	INPUT_TEXT_TYPE inputTextType_;
	int maxLength_;
	std::function<void(std::weak_ptr<UIBase>)> endInputFunction_;

	int inputKeyHandle_;

	// 現在の状態
	int inputKeyState_;

	// 前フレームの状態
	int preInputKeyState_;

	char inputTextBuffer_[256];

	void HighlightedUpdate(void) override;
};
