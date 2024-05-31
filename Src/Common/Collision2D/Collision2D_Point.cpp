#include "Collision2D_Point.h"

Collision2D_Point::Collision2D_Point(void) {

}

Collision2D_Point::Collision2D_Point(Vector2<float> pos) : 
Collision2D(pos) {
	type_ = TYPE::POINT;
}
