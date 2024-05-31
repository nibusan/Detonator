#include "../../../../Manager/GameManager.h"
#include "UI_Time.h"

UI_Time::UI_Time(std::string name, int size, unsigned int color) :
color_(color) {
	usingFont_ = std::make_shared<Font>(name, size);
	usingFont_->SetIsAutoDeleteHandle(true);
}

void UI_Time::Init_UI(void) {

}

bool UI_Time::Update_UI(void) {
	return false;
}

void UI_Time::Draw_UI(void) {
	auto time = GameManager::GetInstance().GetGameTime();
	int min = time / 60.0f;
	int sec = (int)(time) % 60;
	std::string str_min = (min < 10) ? "0" + std::to_string(min) : std::to_string(min);
	std::string str_sec = (sec < 10) ? "0" + std::to_string(sec) : std::to_string(sec);

	DrawStringToHandle(
		0,
		0,
		(str_min + ":" + str_sec).c_str(),
		color_,
		usingFont_->GetHandle()
	);

}

void UI_Time::Release_UI(void) {

}

const Weak_Font UI_Time::GetFont(void) const {
	return usingFont_;
}
