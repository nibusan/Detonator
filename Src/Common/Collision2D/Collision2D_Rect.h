#pragma once
#include "Collision2D.h"

class Collision2D_Rect : public Collision2D {
private:
	Vector2<float> size_;

public:
	Collision2D_Rect(void);
	Collision2D_Rect(Vector2<float> pos, Vector2<float> size);
	~Collision2D_Rect(void) = default;
	void DrawDebug(void) override;
	const Vector2<float>& GetSize(void) const;
};

using Unique_Collision2D_Rect = std::unique_ptr<Collision2D_Rect>;
using Shared_Collision2D_Rect = std::shared_ptr<Collision2D_Rect>;
using Weak_Collision2D_Rect = std::weak_ptr<Collision2D_Rect>;