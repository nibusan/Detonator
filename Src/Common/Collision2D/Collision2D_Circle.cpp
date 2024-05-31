#include "Collision2D_Circle.h"

Collision2D_Circle::Collision2D_Circle(void) {

}

Collision2D_Circle::Collision2D_Circle(Vector2<float> pos, float radius) :
Collision2D(pos),
radius_(radius) {
	type_ = TYPE::CIRCLE;
}

float Collision2D_Circle::GetRadius(void) const {
	return radius_;
}
