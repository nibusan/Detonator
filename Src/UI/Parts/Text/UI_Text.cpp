#include "../../../Common/ResourceKey.h"
#include "../../../Manager/Collision2DManager.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/InputManager.h"
#include "UI_Text.h"

UI_Text::UI_Text(std::string text, std::string name, int size, unsigned int color) : 
text_(text),
usingFont_(nullptr),
color_(color) {
	UI_Text::CreateFont(name, size);
}

void UI_Text::Init_UI(void) {
	
}

bool UI_Text::Update_UI(void) {
	auto& inputManager = InputManager::GetInstance();
	auto mousePos = inputManager.GetMousePos();

	// マウスカーソルがUIの中に入っているかを判定
	bool isHit = Collision2DManager::IsContain_Rect(
		inputManager.GetMousePos().ToVector2f(), 
		std::dynamic_pointer_cast<Collision2D_Rect>(collision_)
	);

	// 中に入っててなおかつ左クリックしたらtrueを返す
	if (isHit && inputManager.IsTrgMouseLeft()) {
		return true;
	}

	return false;
}

void UI_Text::Draw_UI(void) {
	DrawStringToHandle(0, 0, text_.c_str(), color_, usingFont_->GetHandle());
}

void UI_Text::Release_UI(void) {

}

const std::string& UI_Text::GetText(void) const {
	return text_;
}

void UI_Text::SetText(const std::string& text) {
	text_ = text;
}

void UI_Text::CreateFont(std::string name, int size) {
	usingFont_ = std::make_shared<Font>(name, size);
	usingFont_->SetIsAutoDeleteHandle(true);
}

const Weak_Font UI_Text::GetFont(void) const {
	return usingFont_;
}