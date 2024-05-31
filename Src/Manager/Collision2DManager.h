#pragma once
#include "../Common/Collision2D/Collision2D.h"
#include "../Common/Collision2D/Collision2D_Point.h"
#include "../Common/Collision2D/Collision2D_Rect.h"
#include "../Common/Collision2D/Collision2D_Circle.h"
#include "../Common/Collision2D/Collision2D_Polygon.h"

class Collision2DManager {
public:
	/// <summary>
	/// è’ìÀÇµÇΩÇ©ÇîªíË
	/// </summary>
	/// <param name="a"></param>
	/// <param name="b"></param>
	/// <returns></returns>
	static bool IsContain_Rect(const Vector2<float>& a, Weak_Collision2D_Rect b);
	static bool IsContain_Circle(const Vector2<float>& a, Weak_Collision2D_Circle b);
	static bool IsContain_Polygon(const Vector2<float>& a, Weak_Collision2D_Polygon b);
	static bool IsHit_Rect_Rect(Weak_Collision2D_Rect a, Weak_Collision2D_Rect b);
	static bool IsHit_Circle_Circle(Weak_Collision2D_Circle a, Weak_Collision2D_Circle b);
	static bool IsHit_Polygon_Polygon(Weak_Collision2D_Polygon a, Weak_Collision2D_Polygon b);
};

