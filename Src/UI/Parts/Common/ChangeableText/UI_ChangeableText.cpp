#include "../../../../Data/Message/MessageTable.h"
#include "../../../../Utility/Utility.h"
#include "UI_ChangeableText.h"

UI_ChangeableText::UI_ChangeableText(int messageID, std::string name, int size, unsigned int color) :
color_(color) {
	text_ = Utility::WStringToString(MessageTable::GetInstance().GetMessage_(messageID));
	usingFont_ = std::make_shared<Font>(name, size);
}

void UI_ChangeableText::Init_UI(void) {

}

bool UI_ChangeableText::Update_UI(void) {
	// •¶Žš—ñ‚ðXV‚·‚é
	SetText(updateText_());
	return false;
}

void UI_ChangeableText::Draw_UI(void) {
	DrawStringToHandle(0, 0, text_.c_str(), color_, usingFont_->GetHandle());
}

void UI_ChangeableText::Release_UI(void) {

}

void UI_ChangeableText::SetText(const std::string& text) {
	text_ = text;
}
