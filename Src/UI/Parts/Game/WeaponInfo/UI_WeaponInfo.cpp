#include "../../../../Net/NetManager.h"
#include "../../../../Manager/GameManager.h"
#include "UI_WeaponInfo.h"

UI_WeaponInfo::UI_WeaponInfo(
	Weak_Graphic graphic, 
	const std::vector<Weak_Graphic>& graphic_Grenade,
	const std::vector<Weak_Graphic>& graphic_Gun,
	GUN_TYPE gunType, 
	const std::string& font_Name_Magazine,
	const std::string& font_Name_GunName
) :
graphic_(graphic),
graphic_Grenade_(graphic_Grenade),
graphic_Gun_(graphic_Gun),
gunType_(gunType),
magazineCount_(0) {
	usingFont_Magazine_ = std::make_shared<Font>(font_Name_Magazine, 50);
	usingFont_Magazine_->SetIsAutoDeleteHandle(true);
	usingFont_GunName_ = std::make_shared<Font>(font_Name_GunName, 24);
	usingFont_GunName_->SetIsAutoDeleteHandle(true);
}

void UI_WeaponInfo::Init_UI(void) {

}

bool UI_WeaponInfo::Update_UI(void) {
	auto& gameManager = GameManager::GetInstance();
	int no = (int)NetManager::GetInstance().GetSelf().playerType;
	auto loadOut = gameManager.GetLoadOut(no);
	switch (loadOut.gunType_) {
	case GUN_TYPE::AK47:
		gunName_ = "AK-47";
		break;
	case GUN_TYPE::AWM:
		gunName_ = "AWM";
		break;
	case GUN_TYPE::M4:
		gunName_ = "M4";
		break;
	case GUN_TYPE::M24R:
		gunName_ = "M24R";
		break;
	default:
		break;
	}

	magazineCount_ = gameManager.GetMagazine(no);

	return false;
}

void UI_WeaponInfo::Draw_UI(void) {
	auto& gameManager = GameManager::GetInstance();
	int no = (int)NetManager::GetInstance().GetSelf().playerType;
	auto loadOut = gameManager.GetLoadOut(no);
	int grenadeID = (int)(gameManager.GetNowGreType(no));
	int gunID = (int)(loadOut.gunType_);
	//int magazineCount = gameManager.GetPlayerInfo(0).
	graphic_.lock()->Draw({ 0.0f, 0.0f }, false, nullptr);

	// 紛らわしい定数が大量に増えることを避けるために
	// ここでしか使わない座標などはあえて定数化せずに使用してます
	graphic_Grenade_[gunID].lock()->Draw({ 130.0f, 26.0f }, false, nullptr);
	graphic_Gun_[grenadeID].lock()->Draw(ICON_GRENADE_POS, false, nullptr);
	DrawStringToHandle(365, 35, std::to_string(magazineCount_).c_str(), 0xFFFFFF, usingFont_Magazine_->GetHandle());
	DrawStringToHandle(244, 92, gunName_.c_str(), 0xFFFFFF, usingFont_GunName_->GetHandle());
	DrawStringToHandle(244, 92, gunName_.c_str(), 0xFFFFFF, usingFont_GunName_->GetHandle());
	DrawStringToHandle(20, 40, std::to_string(gameManager.GetGrenadeNum(no)).c_str(), 0xFFFFFF, usingFont_GunName_->GetHandle());
}

void UI_WeaponInfo::Release_UI(void) {

}
