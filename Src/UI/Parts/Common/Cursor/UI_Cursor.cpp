#include "../../../../Manager/InputManager.h"
#include "../../../../Manager/Collision2DManager.h"
#include "../../../../Utility/Utility.h"
#include "UI_Cursor.h"

UI_Cursor::UI_Cursor(Weak_Graphic graphic, DIR dir) :
graphic_(graphic),
dir_(dir) {

}

void UI_Cursor::Init_UI(void) {

}

bool UI_Cursor::Update_UI(void) {
	auto& inputManager = InputManager::GetInstance();

	if (!IsClickable()) return false;

	bool isHit = Collision2DManager::IsContain_Rect(
		inputManager.GetMousePos().ToVector2f(), 
		std::dynamic_pointer_cast<Collision2D_Rect>(collision_)
	);

	if (!isHit) return false;

	return inputManager.IsTrgMouseLeft();
}

void UI_Cursor::Draw_UI(void) {
	auto size = GetSize();
	graphic_.lock()->Draw({ size.x / 2.0f, size.y / 2.0f }, 1.0f, dir_ == DIR::LEFT ? 0.0f : Utility::Deg2RadF(180.0f), nullptr);
}

void UI_Cursor::Release_UI(void) {

}
