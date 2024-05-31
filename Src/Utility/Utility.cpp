#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>
#include <DxLib.h>
#include "Utility.h"

int Utility::Round(float v)
{
    return static_cast<int>(roundf(v));
}

std::vector<std::string> Utility::Split(std::string& line, char delimiter)
{

    std::istringstream stream(line);
    std::string field;
    std::vector<std::string> result;
    
    while (getline(stream, field, delimiter)) {
        result.push_back(field);
    }

    return result;

}

float Utility::Rad2DegF(float rad)
{
    return rad * (180.0f / DX_PI_F);
}

int Utility::Rad2DegI(int rad)
{
    return rad * Round(180.0f / DX_PI_F);
}

double Utility::Deg2RadD(double deg)
{
    return deg * (DX_PI / 180.0);
}

float Utility::Deg2RadF(float deg)
{
    return deg * (DX_PI_F / 180.0f);
}

int Utility::Deg2RadI(int deg)
{
    return deg * Round(DX_PI_F / 180.0f);
}

double Utility::DegIn360(double deg)
{
    deg = fmod(deg, 360.0);
    if (deg < 0.0f)
    {
        deg += 360.0;
    }
    return deg;
}

double Utility::RadIn2PI(double rad)
{
    rad = fmod(rad, DX_TWO_PI);
    if (rad < 0.0)
    {
        rad += DX_TWO_PI;
    }
    return rad;
}

int Utility::DirNearAroundRad(float from, float to)
{

    float ret = 1.0f;

    float diff = to - from;

    if (diff >= 0.0f)
    {

        // ��r���������v���Ɉʒu����

        if (diff > DX_PI_F)
        {
            // �ł��A180�x�ȏ㗣��Ă���̂ŁA�����v���̕����߂�
            ret = -1.0f;
        }
        else
        {
            // ���v���
            ret = 1.0f;
        }

    }
    else
    {

        // ��r�����������v���Ɉʒu����

        if (diff < -DX_PI_F)
        {
            // �ł��A180�x�ȏ㗣��Ă���̂ŁA���v���̕����߂�
            ret = 1.0f;
        }
        else
        {
            // �����v���
            ret = -1.0f;
        }

    }

    return static_cast<int>(ret);

}

int Utility::DirNearAroundDeg(float from, float to)
{

    float ret = 1.0f;

    float diff = to - from;

    if (diff >= 0.0f)
    {

        // ��r���������v���Ɉʒu����

        if (diff > 180.0f)
        {
            // �ł��A180�x�ȏ㗣��Ă���̂ŁA�����v���̕����߂�
            ret = -1.0f;
        }
        else
        {
            // ���v���
            ret = 1.0f;
        }

    }
    else
    {

        // ��r�����������v���Ɉʒu����

        if (diff < -180.0f)
        {
            // �ł��A180�x�ȏ㗣��Ă���̂ŁA���v���̕����߂�
            ret = 1.0f;
        }
        else
        {
            // �����v���
            ret = -1.0f;
        }

    }

    return static_cast<int>(ret);

}

int Utility::Lerp(int start, int end, float t)
{
    // ���`���
    if (t >= 1.0f)
    {
        return end;
    }

    int ret = start;
    ret += Round(t * static_cast<float>(end - start));
    return ret;
}

float Utility::Lerp(float start, float end, float t)
{
    // ���`���
    if (t >= 1.0f)
    {
        return end;
    }

    float ret = start;
    ret += t * (end - start);
    return ret;
}

double Utility::Lerp(double start, double end, double t)
{
    // ���`���
    if (t >= 1.0)
    {
        return end;
    }

    double ret = start;
    ret += t * (end - start);
    return ret;
}

Vector2<int> Utility::Lerp(const Vector2<int>& start, const Vector2<int>& end, float t)
{
    // ���`���
    if (t >= 1.0f)
    {
        return end;
    }

    Vector2 ret = start;
    ret.x += Round(t * static_cast<float>((end.x - start.x)));
    ret.y += Round(t * static_cast<float>((end.y - start.y)));
    return ret;
}

VECTOR Utility::Lerp(const VECTOR& start, const VECTOR& end, float t)
{
    // ���`���
    if (t >= 1.0f)
    {
        return end;
    }

    VECTOR ret = start;
    ret.x += t * (end.x - start.x);
    ret.y += t * (end.y - start.y);
    ret.z += t * (end.z - start.z);

    return ret;
}

double Utility::LerpDeg(double start, double end, double t)
{

    double ret;

    double diff = end - start;
    if (diff < -180.0)
    {
        end += 360.0;
        ret = Lerp(start, end, t);
        if (ret >= 360.0)
        {
            ret -= 360.0;
        }
    }
    else if (diff > 180.0)
    {
        end -= 360.0;
        ret = Lerp(start, end, t);
        if (ret < 0.0)
        {
            ret += 360.0;
        }
    }
    else
    {
        ret = Lerp(start, end, t);
    }

    return ret;

}

COLOR_F Utility::Lerp(const COLOR_F& start, const COLOR_F& end, float t)
{
    // ���`���
    if (t >= 1.0f)
    {
        return end;
    }

    COLOR_F ret = start;
    ret.r += t * (end.r - start.r);
    ret.g += t * (end.g - start.g);
    ret.b += t * (end.b - start.b);
    ret.a += t * (end.a - start.a);
    return ret;
}

Vector2<int> Utility::Bezier(const Vector2<int>& p1, const Vector2<int>& p2, const Vector2<int>& p3, float t)
{
    Vector2<int> a = Lerp(p1, p2, t);
    Vector2<int> b = Lerp(p2, p3, t);
    return Lerp(a, b, t);
}

VECTOR Utility::Bezier(const VECTOR& p1, const VECTOR& p2, const VECTOR& p3, float t)
{
    VECTOR a = Lerp(p1, p2, t);
    VECTOR b = Lerp(p2, p3, t);
    return Lerp(a, b, t);
}

VECTOR Utility::RotXZPos(const VECTOR& centerPos, const VECTOR& radiusPos, float rad)
{
    float x = ((radiusPos.x - centerPos.x) * cosf(rad)) - ((radiusPos.z - centerPos.z) * sinf(rad));
    float z = ((radiusPos.x - centerPos.x) * sinf(rad)) + ((radiusPos.z - centerPos.z) * cosf(rad));
    return VGet(centerPos.x + x, radiusPos.y, centerPos.z + z);
}

double Utility::Magnitude(const Vector2<int>& v)
{
    return sqrt((v.x * v.x) + (v.y * v.y));
}

double Utility::Magnitude(const VECTOR& v)
{
    return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

float Utility::MagnitudeF(const VECTOR& v)
{
    return sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

int Utility::SqrMagnitude(const Vector2<int>& v)
{
    return v.x * v.x + v.y * v.y;
}

float Utility::SqrMagnitudeF(const VECTOR& v)
{
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

double Utility::SqrMagnitude(const VECTOR& v)
{
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

double Utility::SqrMagnitude(const VECTOR& v1, const VECTOR& v2)
{
    return pow(v2.x - v1.x, 2) + pow(v2.y - v1.y, 2) + pow(v2.z - v1.z, 2);
}

double Utility::Distance(const Vector2<int>& v1, const Vector2<int>& v2)
{
    return sqrt(pow(v2.x - v1.x, 2) + pow(v2.y - v1.y, 2));
}

double Utility::Distance(const VECTOR& v1, const VECTOR& v2)
{
    return sqrt(pow(v2.x - v1.x, 2) + pow(v2.y - v1.y, 2) + pow(v2.z - v1.z, 2));
}

bool Utility::IsHitSpheres(const VECTOR& pos1, float radius1, const VECTOR& pos2, float radius2)
{
    // ���̓��m�̏Փ˔���
    bool ret = false;

    // ���݂��̔��a�̍��v
    float radius = radius1 + radius2;

    // ���W�̍����炨�݂��̋��������
    VECTOR diff = VSub(pos2, pos1);

    // �O�����̒藝�Ŕ�r(SqrMagnitude�Ɠ���)
    float dis = (diff.x * diff.x) + (diff.y * diff.y) + (diff.z * diff.z);
    if (dis < (radius * radius))
    {
        ret = true;
    }

    return ret;
}

bool Utility::IsHitSphereCapsule(
    const VECTOR& sphPos, float sphRadius, 
    const VECTOR& capPos1, const VECTOR& capPos2, float capRadius)
{

    bool ret = false;

    // �J�v�Z�����̂̒��S���q���x�N�g��
    VECTOR cap1to2 = VSub(capPos2, capPos1);

    // �x�N�g���𐳋K��
    VECTOR cap1to2ENor = VNorm(cap1to2);

    // �J�v�Z���q���̒P�ʃx�N�g���ƁA
    // ���̃x�N�g�������狅�̂ւ̃x�N�g���̓��ς����
    float dot = VDot(cap1to2ENor, VSub(sphPos, capPos1));

    // ���ςŋ��߂��ˉe�������g���āA�J�v�Z���q����̍��W�����
    VECTOR capRidePos = VAdd(capPos1, VScale(cap1to2ENor, dot));

    // �J�v�Z���q���̃x�N�g���̒��������
    float len = Utility::MagnitudeF(cap1to2);

    // ���̂��J�v�Z���q����ɂ��邩���ʂ��邽�߁A�䗦�����
    float rate = dot / len;

    VECTOR centerPos;

    // ���̂̈ʒu���R�G���A�ɕ������ꂽ�J�v�Z���`��̂ǂ��ɂ��邩����
    if (rate > 0.0f && rate <= 1.0f)
    {
        // �@���̂��J�v�Z���q����ɂ���
        centerPos = VAdd(capPos1, VScale(cap1to2ENor, dot));
    }
    else if (rate > 1.0f)
    {
        // �A���̂��J�v�Z���̏I�_���ɂ���
        centerPos = capPos2;
    }
    else if (rate < 0.0f)
    {
        // �B���̂��J�v�Z���̎n�_���ɂ���
        centerPos = capPos1;
    }
    else
    {
        // �����ɂ��Ă͂����Ȃ�
    }

    // ���̓��m�̓����蔻��
    if (Utility::IsHitSpheres(centerPos, capRadius, sphPos, sphRadius))
    {
        ret = true;
    }
    else
    {
        ret = false;
    }

    return ret;

}

bool Utility::Equals(const VECTOR& v1, const VECTOR& v2)
{
    if (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z)
    {
        return true;
    }
    return false;
}

bool Utility::EqualsVZero(const VECTOR& v1)
{
    const VECTOR& v2 = VECTOR_ZERO;
    if (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z)
    {
        return true;
    }
    return false;
}

VECTOR Utility::Normalize(const Vector2<int>& v)
{
    VECTOR ret = VGet(
        static_cast<float>(v.x),
        static_cast<float>(v.y),
        0.0f
    );
    float len = static_cast<float>(Magnitude(v));
    ret.x /= len;
    ret.y /= len;
    ret.z /= len;
    return ret;
}

VECTOR Utility::VNormalize(const VECTOR& v)
{
    if (Utility::EqualsVZero(v))
    {
        // Quaternion�v�Z�Ń[����n���āA
        // �G���[(-1, -1, -1)���Ԃ��Ă���ƍ���
        return v;
    }
    return VNorm(v);
}

double Utility::AngleDeg(const VECTOR& from, const VECTOR& to)
{
    // sqrt(a) * sqrt(b) = sqrt(a * b) -- valid for real numbers
    auto fLen = SqrMagnitude(from);
    auto tLen = SqrMagnitude(to);
    auto denominator = sqrt(fLen * tLen);
    if (denominator < kEpsilonNormalSqrt)
    {
        return 0.0f;
    }

    //float dot = std::clamp(Dot(from, to) / denominator, -1.0f, 1.0f);
    //auto dot = Dot(from, to) / denominator;
    auto dot = VDot(from, to) / denominator;
    if (dot < -1.0f)
    {
        dot = -1.0f;
    }
    if (dot > 1.0f)
    {
        dot = 1.0f;
    }

    return acos(dot) * (180.0 / DX_PI);
}

void Utility::DrawLineDir(const VECTOR& pos, const VECTOR& dir, int color, float len)
{
    auto nDir = Utility::VNormalize(dir);
    auto sPos = VAdd(pos, VScale(nDir, -len));
    auto ePos = VAdd(pos, VScale(nDir, len));
    DrawLine3D(sPos, ePos, color);
    DrawSphere3D(ePos, 5.0f, 5, color, color, true);
}

void Utility::DrawLineXYZ(const VECTOR& pos, const MATRIX& rot, float len)
{

    VECTOR dir;

    // X
    dir = VTransform(Utility::DIR_R, rot);
    DrawLineDir(pos, dir, 0xff0000, len);

    // Y
    dir = VTransform(Utility::DIR_U, rot);
    DrawLineDir(pos, dir, 0x00ff00, len);

    // Z
    dir = VTransform(Utility::DIR_F, rot);
    DrawLineDir(pos, dir, 0x0000ff, len);

}

void Utility::DrawLineXYZ(const VECTOR& pos, const Quaternion& rot, float len)
{

    VECTOR dir;

    // X
    dir = rot.GetRight();
    DrawLineDir(pos, dir, 0xff0000, len);

    // Y
    dir = rot.GetUp();
    DrawLineDir(pos, dir, 0x00ff00, len);

    // Z
    dir = rot.GetForward();
    DrawLineDir(pos, dir, 0x0000ff, len);

}

Vector2<float> Utility::WorldPos2ScreenPos(const VECTOR& wPos, float width, float height)
{

    //�r���[�|�[�g�s��̍쐬
    float w = width / 2.0f;
    float h = height / 2.0f;
    MATRIX viewport = {
    	w , 0 , 0 , 0 ,
    	0 ,-h , 0 , 0 ,
    	0 , 0 , 1 , 0 ,
    	w , h , 0 , 1
    };
    
    //�r���[�ϊ��ƃv���W�F�N�V�����ϊ�
    VECTOR calcPos = wPos;
    
    calcPos = VTransform(calcPos, GetCameraViewMatrix());
    calcPos = VTransform(calcPos, GetCameraProjectionMatrix());
    
    //z�Ŋ�����-1~1�͈̔͂Ɏ��߂�
    calcPos.x /= calcPos.z; 
    calcPos.y /= calcPos.z;
    calcPos.z /= calcPos.z;
    
    //�X�N���[���ϊ�
    calcPos = VTransform(calcPos, viewport);
    
    return { calcPos.x,calcPos.y };
}

VECTOR Utility::GetClosestPosTriangle(
    const VECTOR& tPos1, const VECTOR& tPos2, const VECTOR& tPos3,
    const VECTOR& sPos1)
{
    VECTOR ab = VSub(tPos2, tPos1);
    VECTOR ac = VSub(tPos3, tPos1);
    VECTOR ba = VSub(tPos1, tPos2);
    VECTOR bc = VSub(tPos3, tPos2);
    VECTOR ca = VSub(tPos1, tPos3);
    VECTOR cb = VSub(tPos2, tPos3);
    // �ŋߐړ_�@���_�`
    VECTOR ap = VSub(sPos1, tPos1);
    float dotABAP = VDot(ab, ap);
    float dotACAP = VDot(ac, ap);
    if (dotABAP <= 0.0f && dotACAP <= 0.0f)
    {
        return tPos1;
    }
    // �ŋߐړ_�@���_�a
    VECTOR bp = VSub(sPos1, tPos2);
    float dotBABP = VDot(ba, bp);
    float dotBCBP = VDot(bc, bp);
    if (dotBABP <= 0.0f && dotBCBP <= 0.0f)
    {
        return tPos2;
    }
    // �ŋߐړ_�@���_�b
    VECTOR cp = VSub(sPos1, tPos3);
    float dotCACP = VDot(ca, cp);
    float dotCBCP = VDot(cb, cp);
    if (dotCACP <= 0.0f && dotCBCP <= 0.0f)
    {
        return tPos3;
    }
    // �ŋߐړ_�@�ӂ`�a
    VECTOR crossABCA = VCross(ab, ca);
    VECTOR crossAPBP = VCross(ap, bp);
    float dotAP = VDot(crossABCA, crossAPBP);
    if (dotAP >= 0.0f)
    {
        // ���_A�Ɠ_P�̎ˉe���W
        float dotAPAB = VDot(ap, ab);
        float disAB = Distance(tPos1, tPos2);
        return VAdd(tPos1, VScale(VNorm(ab), dotAPAB / disAB));
    }
    // �ŋߐړ_�@�ӂa�b
    VECTOR crossBCAB = VCross(bc, ab);
    VECTOR crossBPCP = VCross(bp, cp);
    float dotBP = VDot(crossBCAB, crossBPCP);
    if (dotBP >= 0.0f)
    {
        // ���_B�Ɠ_P�̎ˉe���W
        float dotBPBC = VDot(bp, bc);
        float disBC = Distance(tPos2, tPos3);
        return VAdd(tPos2, VScale(VNorm(bc), dotBPBC / disBC));
    }
    // �ŋߐړ_�@�ӂb�`
    VECTOR crossCABC = VCross(ca, bc);
    VECTOR crossCPAP = VCross(cp, ap);
    float dotCP = VDot(crossCABC, crossCPAP);
    if (dotCP >= 0.0f)
    {
        // ���_C�Ɠ_P�̎ˉe���W
        float dotCPCA = VDot(cp, ca);
        float disCA = Distance(tPos3, tPos1);
        return VAdd(tPos3, VScale(VNorm(ca), dotCPCA / disCA));
    }
    // �ŋߐړ_�@�ʂ`�a�b
    VECTOR tCrossN = VNorm(crossABCA);
    if (tCrossN.x == -1.0f && tCrossN.y == -1.0f && tCrossN.z == -1.0f)
    {
    }
    // �꒼����ɒ��_�����сA�@���x�N�g�������Ȃ�
    // ���̏ꍇ�́A�R���_�̂����A�ł��߂����_��Ԃ�
    float aPow = SqrMagnitudeF(ap);
    float bPow = SqrMagnitudeF(bp);
    float cPow = SqrMagnitudeF(cp);
    if (aPow <= bPow)
    {
        if (aPow <= cPow) { return tPos1; }
        else { return tPos3; }
    }
    else
    {
        if (bPow <= cPow) { return tPos2; }
        else { return tPos3; }
    }

    // �O�p�`�̖@���ƃx�N�g��AP���g�p���Ėʏ�̎ˉe�𗎂Ƃ�
    float d = VDot(tCrossN, ap);
    // ���̂̒��S���W����ˉe�̒������A�O�p�`�̖@�������Ƀx�N�g����L�΂��ƁA
    // �O�p�`�ʏ�̍ŋߐړ_�����߂���
    return VAdd(sPos1, VScale(tCrossN, -d));

}

VECTOR Utility::RefrectVector(const VECTOR& inVec, const VECTOR& N)
{
    // R��I-2(I�EN)N

    float dot = Dot(inVec, N);

    return VSub(inVec, VScale(VScale(N, 2.0f),dot));

}

float Utility::Dot(const VECTOR& va, const VECTOR& vb)
{
    return va.x * vb.x + va.y * vb.y + va.z * vb.z;
}

std::string Utility::WStringToString(std::wstring oWString) 
{
	// wstring �� SJIS
	int iBufferSize = WideCharToMultiByte(CP_OEMCP, 0, oWString.c_str()
		, -1, (char*)NULL, 0, NULL, NULL);

	// �o�b�t�@�̎擾
	CHAR* cpMultiByte = new CHAR[iBufferSize];

	// wstring �� SJIS
	WideCharToMultiByte(CP_OEMCP, 0, oWString.c_str(), -1, cpMultiByte
		, iBufferSize, NULL, NULL);

	// string�̐���
	std::string oRet(cpMultiByte, cpMultiByte + iBufferSize - 1);

	// �o�b�t�@�̔j��
	delete[] cpMultiByte;

	// �ϊ����ʂ�Ԃ�
	return(oRet);
}

std::wstring Utility::StringToWString(std::string oString)
{
	// SJIS �� wstring
	int iBufferSize = MultiByteToWideChar(CP_ACP, 0, oString.c_str()
		, -1, (wchar_t*)NULL, 0);

	// �o�b�t�@�̎擾
	wchar_t* cpUCS2 = new wchar_t[iBufferSize];

	// SJIS �� wstring
	MultiByteToWideChar(CP_ACP, 0, oString.c_str(), -1, cpUCS2
		, iBufferSize);

	// string�̐���
	std::wstring oRet(cpUCS2, cpUCS2 + iBufferSize - 1);

	// �o�b�t�@�̔j��
	delete[] cpUCS2;

	// �ϊ����ʂ�Ԃ�
	return(oRet);
}
