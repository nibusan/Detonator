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


	// 追従対象
	Transform* follow_;
	// 追従フレーム番号
	int followFrameNo_;


	// 銃を対象に向けるかつ追従
	void Follow(void);

	void Init_Object(void) override;
	void Update_Object(void) override;
	void Draw_Object(void) override;
	void Release_Object(void) override;
};

