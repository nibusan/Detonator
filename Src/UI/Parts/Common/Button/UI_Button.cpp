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
color2_(color2),
scaleRad_(0.0f) {
	pixelShader_ = std::dynamic_pointer_cast<PixelShader>(ResourceManager::GetInstance().Get("PixelShader_Button"));
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

	bool isHit = Collision2DManager::IsContain_Rect(
		inputManager.GetMousePos().ToVector2f(), 
		std::dynamic_pointer_cast<Collision2D_Rect>(collision_)
	);

	if (!isHit) {
		SetIsHighlighted(false);
		scaleRad_ = 0.0f;
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
	
	DrawStringToHandle(
		graphic_.lock()->GetSize().x / 2 - GetDrawStringWidthToHandle(text_.c_str(), text_.length(), usingFont_->GetHandle()) / 2,
		40, 
		text_.c_str(), 
		color_, 
		usingFont_->GetHandle()
	);

	DrawStringToHandle(
		graphic_.lock()->GetSize().x / 2 - (GetDrawStringWidthToHandle(text2_.c_str(), text2_.length(), usingFont2_->GetHandle()) / 2),
		40 + usingFont_->GetFontSize(),
		text2_.c_str(), 
		color2_, 
		usingFont2_->GetHandle()
	);
}

void UI_Button::Release_UI(void) {

}

void UI_Button::HighlightedUpdate(void) {
	scaleRad_ += Utility::Deg2RadF(2.0f);
	//scale_ = 1.05f;
	if (colorValue_ < 0.5f) {
		colorValue_ += 0.1f;
	}
}
