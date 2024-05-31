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
		// •\¦—p
		Transform transform_;

		// Õ“Ë”»’è—p
		std::shared_ptr<Collider> collider_;

		std::shared_ptr<Model> models_[2];
	};

	MapBase(void) = default;
	~MapBase(void) = default;

	void Init_Object(void) override;
	void Update_Object(void) override;
	void Draw_Object(void) override;
	void Release_Object(void) override;

	// Õ“Ë‚µ‚½‚Æ‚«‚Ìˆ—
	void OnHit(int no) override;

	int GetMapModel(TYPE type);
	
protected:

	MAP map_[(int)TYPE::MAX];

};

