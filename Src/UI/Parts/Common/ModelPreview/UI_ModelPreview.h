#pragma once
#include "../../../UIBase.h"
#include "../../Image/UI_Image.h"
#include "../../Text/UI_Text.h"
#include "../../../../Common/Vector2.h"
#include "../../../../Object/ObjectBase.h"
#include "../../../../Common/Handle/Model/Model.h"

class UI_ModelPreview : public UIBase {
public:
	// 表示するモデルの種類
	enum class OBJECT_TYPE {
		PLAYER = 0,
		GUN_1,
		GUN_2,
		GUN_3,
		GUN_4,
		GRENADE_1,
		GRENADE_2,
	};

	UI_ModelPreview(void) = default;
	UI_ModelPreview(Weak_Graphic graphic, Weak_Graphic graphic_Mask, Weak_PixelShader pixelShader, std::vector<OBJECT_TYPE> objectIDs);
	~UI_ModelPreview(void) = default;

	void Init_UI(void) override;
	bool Update_UI(void) override;
	void Draw_UI(void) override;
	void Release_UI(void) override;
private:
	// 画像
	Weak_Graphic graphic_;
	Weak_Graphic graphic_Mask_;

	// 描画するオブジェクト
	std::vector<std::shared_ptr<ObjectBase>> object_;

	// 描画するモデル
	std::vector<Weak_Model> model_;

};
