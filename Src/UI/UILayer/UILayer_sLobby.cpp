#include "../../Manager/ResourceManager.h"
#include "../../Data/UI/UILayerList.h"
#include "UILayer_sLobby.h"

void UILayer_sLobby::SetDefaultUI(void) {
	auto& uiLayerList = UILayerList::GetInstance();
	auto uiDatas = uiLayerList.GetUILayerData(UILayerList::TYPE::SLOBBY);

	for (auto& [id, uiData] : uiDatas) {
		AddUI(CreateUI(uiData));
	}
}
