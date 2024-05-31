#pragma once
#include "Collision2D.h"

class Collision2D_Point : public Collision2D {
public:
	Collision2D_Point(void);
	Collision2D_Point(Vector2<float> pos);
	~Collision2D_Point(void) = default;

};

