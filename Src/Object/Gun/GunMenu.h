#pragma once
#include "../../Manager/GameManager.h"
#include "../Common/Transform.h"
#include "../ObjectBase.h"

class GunMenu : public ObjectBase
{
public:
	GunMenu(ID id, Transform* follow, int frameNo);
	GunMenu(ID id, VECTOR pos);
	~GunMenu(void);
	void Update(void);
	void Draw(void);
	
	void ChangeType(GUN_TYPE type);

	void SetID(ID id);

private:

	GameManager& gameManager_;

	GUN_TYPE type_;
	ID ID_;

	std::unique_ptr<Model> models_[(int)GUN_TYPE::MAX];

	Transform gunTransform_[(int)GUN_TYPE::MAX];
	VECTOR relativePos_[(int)GUN_TYPE::MAX];


	// �Ǐ]�Ώ�
	Transform* follow_;
	// �Ǐ]�t���[���ԍ�
	int followFrameNo_;


	// �e��ΏۂɌ����邩�Ǐ]
	void Follow(void);

	void Init_Object(void) override;
	void Update_Object(void) override;
	void Draw_Object(void) override;
	void Release_Object(void) override;
};

