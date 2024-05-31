#pragma once
#include <concepts>
#include <cmath>
//#include <assert.h>

//�񎟌��x�N�g��(int,float,double�̂�)
template <typename T>
concept ValidType = std::is_same_v<T, int> || std::is_same_v<T, float> || std::is_same_v<T, double>;
template <ValidType T>
struct Vector2
{

	T x, y;

	Vector2(void) = default;
	constexpr Vector2(T ax, T ay)
	{
		x = ax;
		y = ay;
	};
	~Vector2(void) = default;

	Vector2 operator+(const Vector2& rval) const 
	{
		return { x + rval.x, y + rval.y };
	};

	Vector2 operator-(const Vector2& rval) const 
	{
		return { x - rval.x, y - rval.y };
	};

	Vector2 operator*(const float scale) const 
	{
		return { x * scale, y * scale };
	};

	Vector2 operator*(const Vector2& scale) const 
	{
		return { x * scale.x, y * scale.y };
	};

	Vector2 operator/(const float div) const 
	{
		//assert(div != 0.0f);
		return { x / div, y / div };
	};

	Vector2 operator/(const Vector2& div) const
	{
		//assert(div.x != 0.0f);
		//assert(div.y != 0.0f);
		return { x / div.x, y / div.y };
	};

	void operator+=(const Vector2& rval)
	{
		x += rval.x;
		y += rval.y;
	};

	void operator-=(const Vector2& rval) 
	{
		x -= rval.x;
		y -= rval.y;
	};

	void operator*=(const float scale) 
	{
		x *= scale;
		y *= scale;
	};

	void operator/=(const float div)
	{
		//assert(div != 0.0f);
		x /= div;
		y /= div;
	};

	/// <summary>
	/// �x�N�g���̓��ς�Ԃ�
	/// </summary>
	/// <param name="rval"> �x�N�g�� </param>
	/// <returns> ���� </returns>
	T Dot(const Vector2& rval) const 
	{
		return x * rval.x + y * rval.y;
	};

	/// <summary>
	/// �x�N�g���̊O�ς�Ԃ�
	/// </summary>
	/// <param name="rval"> �x�N�g�� </param>
	/// <returns> �O�� </returns>
	T Cross(const Vector2& rval) const
	{
		return x * rval.y - y * rval.x;
	};

	/// <summary>
	/// �x�N�g���̑傫����Ԃ�
	/// </summary>
	/// <param name=""> �x�N�g�� </param>
	/// <returns> �x�N�g���̑傫�� </returns>
	T Length(void) const 
	{
		return hypotf(x, y);
	};

	/// <summary>
	/// ���K�����ꂽ�x�N�g����Ԃ�
	/// </summary>
	/// <param name=""> �x�N�g�� </param>
	/// <returns> ���K���ς݂̃x�N�g�� </returns>
	Vector2 Normalized(void) const 
	{
		T length = Length();
		return { x / length, y / length };
	};

	/// <summary>
	/// Vector2<int>�ɕϊ�
	/// </summary>
	/// <param name=""> Vector2 </param>
	/// <returns> Vector2<int> </returns>
	Vector2<int> ToVector2i(void) const
	{
		return Vector2<int>(static_cast<int>(x), static_cast<int>(y));
	}

	/// <summary>
	/// Vector2<float>�ɕϊ�
	/// </summary>
	/// <param name=""> Vector2 </param>
	/// <returns> Vector2<float> </returns>
	Vector2<float> ToVector2f(void) const 
	{
		return Vector2<float>(static_cast<float>(x), static_cast<float>(y));
	}

	/// <summary>
	/// Vector2<double>�ɕϊ�
	/// </summary>
	/// <param name=""> Vector2 </param>
	/// <returns> Vector2<double> </returns>
	Vector2<double> ToVector2d(void) const
	{
		return Vector2<double>(static_cast<double>(x), static_cast<double>(y));
	}
};