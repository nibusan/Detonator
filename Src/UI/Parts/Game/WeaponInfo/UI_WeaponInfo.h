#pragma once
#include "../../../UIBase.h"
#include "../../Image/UI_Image.h"
#include "../../Text/UI_Text.h"
#include "../../../../Object/Gun/GunType.h"

class UI_WeaponInfo : public UIBase {
public:
	UI_WeaponInfo(void) = default;
	UI_WeaponInfo(
		Weak_Graphic graphic,
		const std::vector<Weak_Graphic>& graphic_Grenade,
		const std::vector<Weak_Graphic>& graphic_Gun,
		GUN_TYPE gunType,
		const std::string& font_Name_Magazine,
		const std::string& font_Name_GunName
	);
	~UI_WeaponInfo(void) = default;

	void Init_UI(void) override;
	bool Update_UI(void) override;
	void Draw_UI(void) override;
	void Release_UI(void) override;

private:
	// 各アイコンの描画位置
	static constexpr Vector2<float> ICON_GRENADE_POS = { 25.0f, 41.0f };
	static constexpr Vector2<float> ICON_GUN_POS = { 150.0f, 26.0f };

	// 画像
	Weak_Graphic graphic_;
	std::vector<Weak_Graphic> graphic_Grenade_;
	std::vector<Weak_Graphic> graphic_Gun_;

	// 銃の種類
	GUN_TYPE gunType_;

	// 銃の名前
	std::string gunName_;

	// 弾数
	int magazineCount_;

	Shared_Font usingFont_Magazine_;
	Shared_Font usingFont_GunName_;
	unsigned int color_;
	std::function<int(void)> getMagazineFunction_;
};
