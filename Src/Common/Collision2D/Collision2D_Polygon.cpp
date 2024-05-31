#include "Collision2D_Polygon.h"

Collision2D_Polygon::Collision2D_Polygon(void) {

}

Collision2D_Polygon::Collision2D_Polygon(Vector2<float> pos, std::vector<Vector2<float>> vertices) : 
Collision2D(pos),
vertices_(vertices) {
	type_ = TYPE::POLYGON;
}

const std::vector<Vector2<float>>& Collision2D_Polygon::GetVertices(void) const {
	return vertices_;
}
