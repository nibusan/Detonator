#pragma once
#include "SceneBase.h"
#include "../Common/Handle/Graphic/Graphic.h"
#include "../Object/Common/Transform.h"

class AssembleUIBase;
class Model;
class MenuScene final : public SceneBase
{

public:

	// コンストラクタ
	MenuScene(void) = default;

	// デストラクタ
	~MenuScene(void) = default;

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

private:
	
	int back_;
	Transform gun_;
	std::unique_ptr<Model> model_;
	int colorGrHandle_;
	int gunDeg_;
};

