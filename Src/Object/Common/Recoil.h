#pragma once
#include <DxLib.h>
#include "../../Common/Quaternion.h"

class Recoil
{

public:

	// 大きさ1ごとに回転させる量
	static constexpr float ROT_POW = (1.0f);

	// パワーを減衰させる量(1秒)
	static constexpr float DECAY_POW = (30.0f);

	// 最大パワー
	static constexpr float MAX_POW = (10.0f);

	enum class RECOIL_TYPE
	{
		UP,
		LEFT,
		RIGHT,
		NONE
	};

	Recoil(void);
	~Recoil(void);

	void Update(void);

	void ResetPower(void);

	void AddPower(float pow);

	float GetRecoilPowX(void);


private:

	float pow_;
	float goalPow_;

	bool isEnd_;
};

