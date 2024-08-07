#include "../../../../Common/ResourceKey.h"
#include "../../../../Manager/ResourceManager.h"
#include "../../../../Manager/InputManager.h"
#include "../../../../Manager/Collision2DManager.h"
#include "../../../../Utility/Utility.h"
#include "UI_Button.h"

UI_Button::UI_Button(
	Weak_Graphic graphic,
	const std::string& text, 
	const std::string& name, 
	int size, 
	unsigned int color,
	const std::string& text2, 
	const std::string& name2, 
	int size2, 
	unsigned int color2
) : 
graphic_(graphic),
text_(text),
color_(color),
text2_(text2),
color2_(color2) {
	pixelShader_ = std::dynamic_pointer_cast<PixelShader>(ResourceManager::GetInstance().GetResourceFile("PixelShader_Button"));
	usingFont_ = std::make_shared<Font>(name, size);
	usingFont_->SetIsAutoDeleteHandle(true);
	usingFont2_ = std::make_shared<Font>(name2, size2);
	usingFont2_->SetIsAutoDeleteHandle(true);
}

void UI_Button::Init_UI(void) {
	colorValue_ = 0.0f;
	pixelShader_.lock()->CreateConstantBuffer(sizeof(float) * 4);
	pixelShader_.lock()->SetConstantValue(0, colorValue_);
}

bool UI_Button::Update_UI(void) {
	auto& inputManager = InputManager::GetInstance();
	auto mousePos = inputManager.GetMousePos();

	if (!IsClickable()) return false;

	// マウスカーソルがUIの中に入ってるかを判定
	bool isHit = Collision2DManager::IsContain_Rect(
		inputManager.GetMousePos().ToVector2f(), 
		std::dynamic_pointer_cast<Collision2D_Rect>(collision_)
	);

	// 入っていなかったら強調表示状態を解除してクリックされてない状態を示すfalseを返す
	if (!isHit) {
		SetIsHighlighted(false);
		scale_ = 1.0f;

		if (colorValue_ >= 0.0f) {
			colorValue_ -= 0.1f;
		}

		return false;
	}

	SetIsHighlighted(true);

	return inputManager.IsTrgMouseLeft();
}

void UI_Button::Draw_UI(void) {
	pixelShader_.lock()->SetConstantValue(0, colorValue_);
	graphic_.lock()->Draw({ 0.0f ,0.0f }, false, pixelShader_.lock());
	
	// 描画する文字のY座標のオフセット
	const int offsetY = 40;
	DrawStringToHandle(
		graphic_.lock()->GetSize().x / 2 - GetDrawStringWidthToHandle(text_.c_str(), text_.length(), usingFont_->GetHandle()) / 2,
		offsetY, 
		text_.c_str(), 
		color_, 
		usingFont_->GetHandle()
	);

	DrawStringToHandle(
		graphic_.lock()->GetSize().x / 2 - (GetDrawStringWidthToHandle(text2_.c_str(), text2_.length(), usingFont2_->GetHandle()) / 2),
		offsetY + usingFont_->GetFontSize(),
		text2_.c_str(), 
		color2_, 
		usingFont2_->GetHandle()
	);
}

void UI_Button::Release_UI(void) {

}

void UI_Button::HighlightedUpdate(void) {
	// ピクセルシェーダーに渡す色の濃さを計算
	const float MAX_COLOR_VALUE = 0.5f;
	const float ADD_COLOR_VALUE = 0.1f;
	if (colorValue_ < MAX_COLOR_VALUE) {
		colorValue_ += ADD_COLOR_VALUE;
	}
}
