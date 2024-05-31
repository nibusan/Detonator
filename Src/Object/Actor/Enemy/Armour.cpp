#include "../../../Utility/Utility.h"
#include "Shield.h"
#include "Armour.h"

Armour::Armour()
{
	aliveShieldNum_ = SHIELD_NUM;
}

Armour::~Armour()
{
}

void Armour::Init_Object(void)
{

}

void Armour::Update_Object(void)
{
}

void Armour::Draw_Object(void)
{
	for (auto& shield : shields_) {
		shield->Draw();
	}
}

void Armour::Release_Object(void)
{
	for (auto& shield : shields_) {
		shield->Release();
	}
}

void Armour::SetShield(VECTOR followPos, int num)
{
	transform_.pos = followPos;
	float angle = 0.0f;
	while (angle < 360.0f) {

		VECTOR sendRot = { 0.0f,Utility::Deg2RadF(angle + (num * (360.0f / SHIELD_NUM / 2.0f))),0.0f };
		VECTOR sendPos = VAdd(transform_.pos, Quaternion::Euler(
			sendRot).PosAxis(SHIELD_RELATIVE_POS));

		int num = (int)(angle / (360.0f / SHIELD_NUM));
		shields_[num]
			= std::make_shared<Shield>(std::dynamic_pointer_cast<Armour>(GetPtr()),sendPos, sendRot);
		shields_[num]->Init();

		angle += 360.0f / SHIELD_NUM;
	}
}

void Armour::UpdateShield(VECTOR followPos)
{
	transform_.pos = followPos;
	aliveShieldNum_ = 0;
	for (auto& shield : shields_) {
		auto s = std::dynamic_pointer_cast<Shield>(shield);

		auto nowRot = s->GetTransform().rot;
		nowRot.y += Utility::Deg2RadF(1.0f);

		VECTOR sendPos = VAdd(transform_.pos, Quaternion::Euler(
			nowRot).PosAxis(SHIELD_RELATIVE_POS));

		s->SetPos(sendPos);
		s->SetRot(nowRot);
		s->Update();

		if (!shield->GetTransform().isHide)aliveShieldNum_++;
	}

}

int Armour::GetAliveShieldNum(void)
{
	return aliveShieldNum_;
}
