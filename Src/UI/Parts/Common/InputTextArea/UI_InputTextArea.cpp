#include <memory>
#include "../../../../Manager/ResourceManager.h"
#include "../../../../Manager/InputManager.h"
#include "../../../../Manager/Collision2DManager.h"
#include "UI_InputTextArea.h"

UI_InputTextArea::UI_InputTextArea(
	Weak_Graphic graphic, 
	const std::string& name, 
	int size, 
	unsigned int color, 
	INPUT_TYPE inputType, 
	INPUT_TEXT_TYPE inputTextType,
	int maxLength,
	const std::function<void(std::weak_ptr<UIBase>)>& endInputFunction
) : 
graphic_(graphic),
color_(color),
inputType_(inputType),
inputTextType_(inputTextType),
maxLength_(maxLength),
endInputFunction_(endInputFunction)
{
	usingFont_ = std::make_shared<Font>(name, size);
	usingFont_->SetIsAutoDeleteHandle(true);
}

void UI_InputTextArea::Init_UI(void) {
	inputKeyHandle_ = MakeKeyInput(maxLength_, false, false, false);
	inputKeyState_ = 0;
	preInputKeyState_ = 0;
}

bool UI_InputTextArea::Update_UI(void) {
	auto& inputManager = InputManager::GetInstance();
	auto mousePos = inputManager.GetMousePos();
	preInputKeyState_ = inputKeyState_;

	if (!IsClickable()) return false;

	bool isHit = Collision2DManager::IsContain_Rect(
		inputManager.GetMousePos().ToVector2f(), 
		std::dynamic_pointer_cast<Collision2D_Rect>(collision_)
	);

	bool ret = false;
	if (inputManager.IsTrgMouseLeft()) {
		if (isHit) {
			// 入力中の文字の色などを設定
			SetKeyInputStringColor(
				0xFF0000,
				0xFF0000,
				0xFF0000,
				0xFF0000,
				0xFF0000,
				0xFF0000,
				0xFF0000,
				0xFF0000,
				0xFF0000,
				0xFF0000,
				0xFF0000,
				0xFF0000,
				0xFF0000,
				0xFF0000,
				0xFF0000,
				0xFF0000,
				0xFF0000
			);

			// 入力受付状態にする
			SetActiveKeyInput(inputKeyHandle_);
			SetIsHighlighted(true);
			ret = true;
		}
		else {
			SetIsHighlighted(false);
		}
	}

	inputKeyState_ = CheckKeyInput(inputKeyHandle_);

	// 入力が完了した際の処理
	if (preInputKeyState_ == 0 && inputKeyState_ != 0) {
		// 入力された文字列をコピー
		GetKeyInputString(inputTextBuffer_, inputKeyHandle_);
		inputText_ = inputTextBuffer_;

		// もし何も入力されてなかったら0を入れる
		if (inputText_ == "") {
			inputText_ = "0";
		}
		SetActiveKeyInput(-1);

		// コールバック関数の呼び出し
		endInputFunction_(shared_from_this());
	}

	return ret;
}

void UI_InputTextArea::Draw_UI(void) {
	graphic_.lock()->Draw({ 0.0f,0.0f }, false, nullptr);
	SetKeyInputStringFont(usingFont_->GetHandle());

	// テキストを描画する際に使用するY軸のオフセット
	const int offsetY = 40;

	if (CheckKeyInput(inputKeyHandle_) == 0) {
		// 入力中の文字列の描画
		DrawKeyInputString(
			graphic_.lock()->GetSize().x / 2 - GetDrawStringWidthToHandle(inputText_.c_str(), inputText_.length(), usingFont_->GetHandle()) / 2,
			offsetY,
			inputKeyHandle_
		);
	}
	else {
		// 入力が完了した文字列の描画
		DrawStringToHandle(
			graphic_.lock()->GetSize().x / 2 - GetDrawStringWidthToHandle(inputText_.c_str(), inputText_.length(), usingFont_->GetHandle()) / 2,
			offsetY,
			inputText_.c_str(),
			color_,
			usingFont_->GetHandle()
		);
	}
}

void UI_InputTextArea::Release_UI(void) {

}

const std::string& UI_InputTextArea::GetInputString(void) const {
	return inputText_;
}

void UI_InputTextArea::HighlightedUpdate(void) {
	if (CheckKeyInput(inputKeyHandle_) != 0) {
		GetKeyInputString(inputTextBuffer_, inputKeyHandle_);
		inputText_ = inputTextBuffer_;
	}
}
