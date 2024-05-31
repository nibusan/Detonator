#pragma once
#include "memory"
#include "../Vector2.h"

class Collision2D {
public:
	enum class TYPE {
		POINT,
		RECT,
		CIRCLE,
		POLYGON
	};

protected:
	TYPE type_;
	Vector2<float> pos_;

public:
	Collision2D(void);
	Collision2D(Vector2<float> pos);
	virtual ~Collision2D(void) = default;
	virtual void DrawDebug(void);
	const Vector2<float>& GetPos(void) const;
	void SetPos(Vector2<float>& pos);
};

using Unique_Collision2D = std::unique_ptr<Collision2D>;
using Shared_Collision2D = std::shared_ptr<Collision2D>;
using Weak_Collision2D = std::weak_ptr<Collision2D>;