#pragma once
#include "../ObjectBase.h"

class Model;

class MapBase : public ObjectBase
{
public:

	enum class TYPE
	{
		NORMAL,
		UNBREAK,
		MAX
	};

	struct MAP
	{
		// 表示用
		Transform transform_;

		// 衝突判定用
		std::shared_ptr<Collider> collider_;

		std::shared_ptr<Model> models_[2];
	};

	MapBase(void) = default;
	~MapBase(void) = default;

	void Init_Object(void) override;
	void Update_Object(void) override;
	void Draw_Object(void) override;
	void Release_Object(void) override;

	// 衝突したときの処理
	void OnHit(int no) override;

	int GetMapModel(TYPE type);
	
protected:

	MAP map_[(int)TYPE::MAX];

};

