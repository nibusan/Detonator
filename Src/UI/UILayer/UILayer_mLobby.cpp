#include "../../Manager/ResourceManager.h"
#include "../../Data/UI/UILayerList.h"
#include "UILayer_mLobby.h"

void UILayer_mLobby::SetDefaultUI(void) {
	auto& uiLayerList = UILayerList::GetInstance();
	auto uiDatas = uiLayerList.GetUILayerData(UILayerList::TYPE::MLOBBY);

	for (auto& [id, uiData] : uiDatas) {
		AddUI(CreateUI(uiData));
	}
}
