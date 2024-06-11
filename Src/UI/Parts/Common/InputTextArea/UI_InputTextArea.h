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

	// 入力しているテキスト
	std::string inputText_;

	// 使用するフォント
	Shared_Font usingFont_;

	// テキストの色
	unsigned int color_;

	// 入力される文字の種類
	INPUT_TYPE inputType_;

	// 入力を受け付けるテキストの種類
	INPUT_TEXT_TYPE inputTextType_;

	// 入力できる文字数の最大数
	int maxLength_;

	// 入力が終わった際に呼ばれるコールバック関数
	std::function<void(std::weak_ptr<UIBase>)> endInputFunction_;

	// 入力を受け付けるキーのハンドル
	int inputKeyHandle_;

	// 現在の状態
	int inputKeyState_;

	// 前フレームの状態
	int preInputKeyState_;

	// 入力される文字列を一時的に格納するバッファ
	char inputTextBuffer_[256];

	void HighlightedUpdate(void) override;
};
