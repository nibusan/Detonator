#pragma once
#include "../../ObjectBase.h"

class Shield;

class Armour : public ObjectBase
{
public:

	static constexpr VECTOR SHIELD_RELATIVE_POS = { 0.0f,0.0f,100.0f };
	static constexpr int SHIELD_NUM = (6);

	Armour();
	~Armour();

	
	void SetShield(VECTOR followPos, int num);

	void UpdateShield(VECTOR followPos);

	int GetAliveShieldNum(void);

private:


	std::shared_ptr<ObjectBase> shields_[SHIELD_NUM];
	int aliveShieldNum_;

	void Init_Object(void) override;
	void Update_Object(void)override;
	void Draw_Object(void) override;
	void Release_Object(void) override;
};

