#include <tuple>
#include "../Utility/Utility.h"
#include "IntVector3.h"

// コンストラクタ
IntVector3::IntVector3()
{
	x = 0;
	y = 0;
	z = 0;
}

// コンストラクタ
IntVector3::IntVector3(int vX, int vY, int vZ)
{
	x = vX;
	y = vY;
	z = vZ;
}

IntVector3::IntVector3(VECTOR v)
{
	x = Utility::Round(v.x);
	y = Utility::Round(v.y);
	z = Utility::Round(v.z);
}

IntVector3::~IntVector3(void)
{
}

bool IntVector3::operator<(const IntVector3& value) const
{
	return std::tie(x, y, z) < std::tie(value.x, value.y, value.z);
}

void IntVector3::Add(int v)
{
	x += v;
	y += v;
	z += v;
}

void IntVector3::Sub(int v)
{
	x -= v;
	y -= v;
	z -= v;
}

void IntVector3::Scale(int v)
{
	x *= v;
	y *= v;
	z *= v;
}
