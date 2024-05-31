#include <DxLib.h>
#include "Collision2D.h"

Collision2D::Collision2D(void) :
type_(TYPE::POINT),
pos_({}) {

}

Collision2D::Collision2D(Vector2<float> pos) :
type_(TYPE::POINT),
pos_(pos) {

}

void Collision2D::DrawDebug(void) {

}

const Vector2<float>& Collision2D::GetPos(void) const {
	return pos_;
}

void Collision2D::SetPos(Vector2<float>& pos) {
	pos_ = pos;
}
