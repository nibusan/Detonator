#include "../../Manager/SceneManager.h"
#include "../../Manager/InputManager.h"
#include "../../Utility/Utility.h"
#include "Recoil.h"


Recoil::Recoil(void)
{
	pow_ = 0.0f;
	goalPow_ = 0.0f;
	isEnd_ = true;
}

Recoil::~Recoil(void)
{

}

void Recoil::Update(void)
{
	if (isEnd_) return;

	if (pow_ > 0.0f)
	{
		pow_ -= SceneManager::GetInstance().GetDeltaMult(DECAY_POW);
		if (pow_ < 0.0f)pow_ = 0.0f;
	}
	else
	{
		//pow_ = Utility::Lerp(pow_, goalPow_, 0.8f);
		//if (fabsf(pow_ - goalPow_) <= 0.1f)
		//{
			pow_ = 0.0f;
			goalPow_ = 0.0f;
			isEnd_ = true;
		//}
	}
}

void Recoil::ResetPower(void)
{
	pow_ = 0.0f;
	goalPow_ = 0.0f;
	isEnd_ = true;
}

void Recoil::AddPower(float pow)
{
	pow_ += pow;
	if (pow_ >= MAX_POW)pow_ = MAX_POW;
	goalPow_ = -pow_;
	isEnd_ = false;
}

float Recoil::GetRecoilPowX(void)
{
	//switch (type)
	//{
	//case Recoil::RECOIL_TYPE::UP:

	//	return SceneManager::GetInstance().GetDeltaMult(pow * ROT_POW);

	//	break;
	//case Recoil::RECOIL_TYPE::LEFT:
	//	break;
	//case Recoil::RECOIL_TYPE::RIGHT:
	//	break;
	//}
	//return 0.0f;

	return SceneManager::GetInstance().GetDeltaMult(Utility::Deg2RadF(pow_));

	//Utility::Lerp();
}

