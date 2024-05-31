#include "UI_MessageBox.h"

UI_MessageBox::UI_MessageBox(Weak_Graphic graphic, std::string text, std::string name, int size, unsigned int color, std::function<std::string(void)> getAddTextFunction) :
graphic_(graphic),
defText_(text),
color_(color),
getAddTextFunction_(getAddTextFunction) {
	usingFont_ = std::make_shared<Font>(name, size);
	usingFont_->SetIsAutoDeleteHandle(true);
}

void UI_MessageBox::Init_UI(void) {

}

bool UI_MessageBox::Update_UI(void) {
	addText_ = getAddTextFunction_();
	return false;
}

void UI_MessageBox::Draw_UI(void) {
	auto textTotalCount = defText_.length() + addText_.length();
	auto size = graphic_.lock()->GetSize();
	graphic_.lock()->Draw({ 0.0f,0.0f }, false, 3, 1, 0, 0, nullptr);
	graphic_.lock()->Draw(
		{ 
			static_cast<float>(size.x / 3.0f),
			0.0f
		}, 
		{ 
			static_cast<float>(size.x / 3 + GetDrawStringWidthToHandle((defText_ + addText_).c_str(), textTotalCount, usingFont_->GetHandle())),
			static_cast<float>(size.y)
		}, 
		false, 
		3, 1, 1, 0, 
		nullptr
	);
	graphic_.lock()->Draw(
		{ 
			static_cast<float>(size.x / 3 + GetDrawStringWidthToHandle((defText_ + addText_).c_str(), textTotalCount, usingFont_->GetHandle())),
			0.0f
		}, 
		false,
		3, 1, 2, 0, 
		nullptr
	);
	DrawStringToHandle(
		size.x / 3, 
		size.y / 2 - usingFont_->GetFontSize() / 2,
		(defText_ + addText_).c_str(),
		color_,
		usingFont_->GetHandle()
	);
}

void UI_MessageBox::Release_UI(void) {

}

void UI_MessageBox::SetAddText(const std::string& text) {
	addText_ = text;
}
