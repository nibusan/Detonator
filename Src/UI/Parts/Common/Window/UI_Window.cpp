#include "../../../../Manager/ResourceManager.h"
#include "../../Image/UI_Image.h"
#include "UI_Window.h"

UI_Window::UI_Window(
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
windowText_(text),
color_(color),
windowText2_(text2),
color2_(color2){
	usingFont_ = std::make_shared<Font>(name, size);
	usingFont2_ = std::make_shared<Font>(name2, size2);
}

void UI_Window::Init_UI(void) {
	
}

bool UI_Window::Update_UI(void) {
    return false;
}

void UI_Window::Draw_UI(void) {
	graphic_.lock()->Draw({ 0.0f ,0.0f }, false, usingPixelShader_.lock() != nullptr ? usingPixelShader_.lock() : nullptr);

	DrawStringToHandle(
		graphic_.lock()->GetSize().x / 2 - (GetDrawStringWidthToHandle(windowText_.c_str(), windowText_.length(), usingFont_->GetHandle()) / 2),
		40, 
		windowText_.c_str(), 
		color_, 
		usingFont_->GetHandle()
	);
	DrawStringToHandle(
		graphic_.lock()->GetSize().x / 2 - (GetDrawStringWidthToHandle(windowText2_.c_str(), windowText2_.length(), usingFont2_->GetHandle()) / 2),
		40 + usingFont_->GetFontSize(),
		windowText2_.c_str(), 
		color2_, 
		usingFont2_->GetHandle()
	);
}

void UI_Window::Release_UI(void) {
	
}

void UI_Window::AddButton(std::shared_ptr<UIBase> button) {
	button->SetParent(shared_from_this());
	buttons_.emplace_back(button);
}
