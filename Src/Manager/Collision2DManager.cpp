#include <cmath>
#include "Collision2DManager.h"

bool Collision2DManager::IsContain_Rect(const Vector2<float>& a, Weak_Collision2D_Rect b) {
	auto& bPos = b.lock()->GetPos();
	auto& bSize = b.lock()->GetSize();
	auto diff = bPos - a;
	return (std::abs(diff.x) < bSize.x) && (std::abs(diff.y) < bSize.y);
}

bool Collision2DManager::IsContain_Circle(const Vector2<float>& a, Weak_Collision2D_Circle b) {
	return false;
}

bool Collision2DManager::IsContain_Polygon(const Vector2<float>& a, Weak_Collision2D_Polygon b) {
	return false;
}

bool Collision2DManager::IsHit_Rect_Rect(Weak_Collision2D_Rect a, Weak_Collision2D_Rect b) {
	return false;
}

bool Collision2DManager::IsHit_Circle_Circle(Weak_Collision2D_Circle a, Weak_Collision2D_Circle b) {
	return false;
}

bool Collision2DManager::IsHit_Polygon_Polygon(Weak_Collision2D_Polygon a, Weak_Collision2D_Polygon b) {
	return false;
}
