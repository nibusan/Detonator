#include "UI_ResultText.h"

UI_ResultText::UI_ResultText(
	const std::string& text, 
	const std::string& name, 
	int size, 
	unsigned int color, 
	const std::function<std::string(void)>& getAddTextFunction) :
defText_(text),
color_(color),
getAddTextFunction_(getAddTextFunction) {
	usingFont_ = std::make_shared<Font>(name, size);
	usingFont_->SetIsAutoDeleteHandle(true);
}

void UI_ResultText::Init_UI(void) {

}

bool UI_ResultText::Update_UI(void) {
	addText_ = getAddTextFunction_();
	return false;
}

void UI_ResultText::Draw_UI(void) {
	DrawStringToHandle(0, 0, (defText_ + addText_).c_str(), color_, usingFont_->GetHandle());
}

void UI_ResultText::Release_UI(void) {

}

void UI_ResultText::SetAddText(const std::string& text) {
	addText_ = text;
}
