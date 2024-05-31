#pragma once
#include "../../Manager/GameManager.h"
#include "../Common/Transform.h"
#include "../ObjectBase.h"

class GrenadeMenu : public ObjectBase
{
public:
	GrenadeMenu(ID id, int slotNo, VECTOR pos);
	~GrenadeMenu(void);
	void Update(void);
	void Draw(void);

	int GetSlotNo(void) const;

	void ChangeType(GRENADE_TYPE type);

	void SetID(ID id);

private:

	GameManager& gameManager_;

	int slotNo_;
	ID ID_;
	GRENADE_TYPE type_;

	std::unique_ptr<Model> models_[(int)GRENADE_TYPE::MAX];

	Transform greTransform_[(int)GRENADE_TYPE::MAX];

	void Init_Object(void) override;
	void Update_Object(void) override;
	void Draw_Object(void) override;
	void Release_Object(void) override;
};
