#include "UI_ColorBox.h"

UI_ColorBox::UI_ColorBox(unsigned int color) :
color_(color) {

}

void UI_ColorBox::Init_UI(void) {

}

bool UI_ColorBox::Update_UI(void) {
	return false;
}

void UI_ColorBox::Draw_UI(void) {
	auto& screenSize = GetSize();
	DrawBox(0, 0, screenSize.x, screenSize.y, color_, true);
}

void UI_ColorBox::Release_UI(void) {

}
