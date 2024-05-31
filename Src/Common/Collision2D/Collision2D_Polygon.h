#pragma once
#include <vector>
#include "Collision2D.h"

class Collision2D;
class Collision2D_Polygon : public Collision2D {
private:
	std::vector<Vector2<float>> vertices_;

public:
	Collision2D_Polygon(void);
	Collision2D_Polygon(Vector2<float> pos, std::vector<Vector2<float>> vertices);
	~Collision2D_Polygon(void) = default;
	const std::vector<Vector2<float>>& GetVertices(void) const;
};

using Unique_Collision2D_Polygon = std::unique_ptr<Collision2D_Polygon>;
using Shared_Collision2D_Polygon = std::shared_ptr<Collision2D_Polygon>;
using Weak_Collision2D_Polygon = std::weak_ptr<Collision2D_Polygon>;