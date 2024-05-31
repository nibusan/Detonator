#pragma once
#include "Collision2D.h"

class Collision2D_Circle : public Collision2D {
private:
	float radius_;

public:
	Collision2D_Circle(void);
	Collision2D_Circle(Vector2<float> pos, float radius);
	~Collision2D_Circle(void) = default;
	float GetRadius(void) const;
};

using Unique_Collision2D_Circle = std::unique_ptr<Collision2D_Circle>;
using Shared_Collision2D_Circle = std::shared_ptr<Collision2D_Circle>;
using Weak_Collision2D_Circle = std::weak_ptr<Collision2D_Circle>;