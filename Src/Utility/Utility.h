#pragma once
#include <string>
#include <vector>
#include <DxLib.h>
#include "../Common/Vector2.h"
#include "../Common/Quaternion.h"


class Utility
{

public:

	// ���W�A��(rad)�E�x(deg)�ϊ��p
	static constexpr float RAD2DEG = (180.0f / DX_PI_F);
	static constexpr float DEG2RAD = (DX_PI_F / 180.0f);

	static constexpr VECTOR VECTOR_ZERO = { 0.0f, 0.0f, 0.0f };
	static constexpr VECTOR VECTOR_ONE = { 1.0f, 1.0f, 1.0f };

	// ��]��
	static constexpr VECTOR AXIS_X = { 1.0f, 0.0f, 0.0f };
	static constexpr VECTOR AXIS_Y = { 0.0f, 1.0f, 0.0f };
	static constexpr VECTOR AXIS_Z = { 0.0f, 0.0f, 1.0f };

	// ����
	static constexpr VECTOR DIR_F = { 0.0f, 0.0f, 1.0f };
	static constexpr VECTOR DIR_B = { 0.0f, 0.0f, -1.0f };
	static constexpr VECTOR DIR_R = { 1.0f, 0.0f, 0.0f };
	static constexpr VECTOR DIR_L = { -1.0f, 0.0f, 0.0f };
	static constexpr VECTOR DIR_U = { 0.0f, 1.0f, 0.0f };
	static constexpr VECTOR DIR_D = { 0.0f, -1.0f, 0.0f };

	static constexpr float kEpsilonNormalSqrt = 1e-15F;

	// �l�̌ܓ�
	static int Round(float v);

	// ������̕���
	static std::vector <std::string> Split(std::string& line, char delimiter);

	// ���W�A��(rad)����x(deg)
	static double Rad2DegD(double rad);
	static float Rad2DegF(float rad);
	static int Rad2DegI(int rad);

	// �x(deg)���烉�W�A��(rad)
	static double Deg2RadD(double deg);
	static float Deg2RadF(float deg);
	static int Deg2RadI(int deg);

	// 0�`360�x�͈̔͂Ɏ��߂�
	static double DegIn360(double deg);

	// 0(0)�`2��(360�x)�͈̔͂Ɏ��߂�
	static double RadIn2PI(double rad);

	// ��]�����Ȃ����̉�]�������擾����(���v���:1�A�����v���:-1)
	static int DirNearAroundRad(float from, float to);

	// ��]�����Ȃ����̉�]�������擾����(���v���:1�A�����v���:-1)
	static int DirNearAroundDeg(float from, float to);

	// ���`���
	static int Lerp(int start, int end, float t);
	static float Lerp(float start, float end, float t);
	static double Lerp(double start, double end, double t);
	static Vector2<int> Lerp(const Vector2<int>& start, const Vector2<int>& end, float t);
	static VECTOR Lerp(const VECTOR& start, const VECTOR& end, float t);

	// �p�x�̐��`���
	static double LerpDeg(double start, double end, double t);

	// �F�̐��`���
	static COLOR_F Lerp(const COLOR_F& start, const COLOR_F& end, float t);

	// �x�W�F�Ȑ�
	static Vector2<int> Bezier(const Vector2<int>& p1, const Vector2<int>& p2, const Vector2<int>& p3, float t);
	static VECTOR Bezier(const VECTOR& p1, const VECTOR& p2, const VECTOR& p3, float t);

	// Y����]
	static VECTOR RotXZPos(const VECTOR& centerPos, const VECTOR& radiusPos, float rad);

	// �x�N�g���̒���
	static double Magnitude(const Vector2<int>& v);
	static double Magnitude(const VECTOR& v);
	static float MagnitudeF(const VECTOR& v);
	static int SqrMagnitude(const Vector2<int>& v);
	static float SqrMagnitudeF(const VECTOR& v);
	static double SqrMagnitude(const VECTOR& v);
	static double SqrMagnitude(const VECTOR& v1, const VECTOR& v2);
	static double Distance(const Vector2<int>& v1, const Vector2<int>& v2);
	static double Distance(const VECTOR& v1, const VECTOR& v2);

	// ���̓��m�̏Փ˔���
	static bool IsHitSpheres(
		const VECTOR& pos1, float radius1, const VECTOR& pos2, float radius2);

	// ���̂ƃJ�v�Z���̏Փ˔���
	static bool IsHitSphereCapsule(
		const VECTOR& sphPos, float sphRadius,
		const VECTOR& capPos1, const VECTOR& capPos2, float capRadius);

	// ��r
	static bool Equals(const VECTOR& v1, const VECTOR& v2);
	static bool EqualsVZero(const VECTOR& v1);

	// ���K��
	static VECTOR Normalize(const Vector2<int>& v);
	static VECTOR VNormalize(const VECTOR& v);

	// 2�̃x�N�g���̊Ԃ̊p�x
	static double AngleDeg(const VECTOR& from, const VECTOR& to);

	// �`��n
	static void DrawLineDir(const VECTOR& pos, const VECTOR& dir, int color, float len = 50.0f);
	static void DrawLineXYZ(const VECTOR& pos, const MATRIX& rot, float len = 50.0f);
	static void DrawLineXYZ(const VECTOR& pos, const Quaternion& rot, float len = 50.0f);

	// ���[���h���W���X�N���[�����W�ɕϊ�
	static Vector2<float> WorldPos2ScreenPos(const VECTOR& wPos, float width, float height);

	static VECTOR GetClosestPosTriangle(
		const VECTOR& tPos1, const VECTOR& tPos2, const VECTOR& tPos3,
		const VECTOR& sPos1);


	static VECTOR RefrectVector(const VECTOR& inVec, const VECTOR& N);

	///���ς�Ԃ�
	static float Dot(const VECTOR& va, const VECTOR& vb);

	// ������ϊ�

	//wstring��string�֕ϊ�
	static std::string WStringToString(std::wstring oWString);

	//string��wstring�֕ϊ�
	static std::wstring StringToWString(std::string oString);
};

// inline�W�J������
inline double Utility::Rad2DegD(double rad)
{
	return rad * (180.0 / DX_PI);
}

