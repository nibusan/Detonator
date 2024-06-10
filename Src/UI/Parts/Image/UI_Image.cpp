#include "../../../Common/ResourceKey.h"
#include "../../../Manager/Collision2DManager.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/InputManager.h"
#include "UI_Image.h"

UI_Image::UI_Image(Weak_Graphic graphic) : 
graphic_(graphic) {

}

void UI_Image::Init_UI(void) {
	auto& resourceManager = ResourceManager::GetInstance();
}

bool UI_Image::Update_UI(void) {
	auto& inputManager = InputManager::GetInstance();
	auto mousePos = inputManager.GetMousePos();

	// マウスカーソルがUIの中に入っているかを判定
	bool isHit = Collision2DManager::IsContain_Rect(
		inputManager.GetMousePos().ToVector2f(), 
		std::dynamic_pointer_cast<Collision2D_Rect>(collision_)
	);
	
	// 中に入っていなくてなおかつクリックできなかったら強調表示状態を解除してfalseを返す
	if (!isHit || !IsClickable()) {
		SetIsHighlighted(false);
		scale_ = 1.0f;
		return false;
	}

	SetIsHighlighted(true);

	return inputManager.IsTrgMouseLeft() && IsClickable();
}

void UI_Image::Draw_UI(void) {
	graphic_.lock()->Draw({ 0.0f ,0.0f }, false, usingPixelShader_.lock() != nullptr ? usingPixelShader_.lock() : nullptr);
}

void UI_Image::Release_UI(void) {

}

const Weak_Graphic UI_Image::GetGraphic(void) const {
    return graphic_;
}

void UI_Image::SetGraphic(Weak_Graphic graphic) {
	graphic_ = graphic;
}

Weak_PixelShader UI_Image::GetUsingPixelShader(void) const {
	return usingPixelShader_;
}

void UI_Image::HighlightedUpdate(void) {
	scale_ = HIGHLIGHTED_SCALE;
}
