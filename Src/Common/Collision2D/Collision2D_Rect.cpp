#include <DxLib.h>
#include "Collision2D_Rect.h"

Collision2D_Rect::Collision2D_Rect(void) {
}

Collision2D_Rect::Collision2D_Rect(Vector2<float> pos, Vector2<float> size) :
Collision2D(pos),
size_(size) {
	type_ = TYPE::RECT;
}

void Collision2D_Rect::DrawDebug(void) {
	DrawBox(
		pos_.x - size_.x, 
		pos_.y - size_.y, 
		pos_.x + size_.x, 
		pos_.y + size_.y,
		0xFF0000,
		false
	);
}

const Vector2<float>& Collision2D_Rect::GetSize(void) const {
	return size_;
}
