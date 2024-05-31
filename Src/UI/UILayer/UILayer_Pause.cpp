#include "../../Manager/ResourceManager.h"
#include "../../Data/UI/UILayerList.h"
#include "UILayer_Pause.h"

void UILayer_Pause::SetDefaultUI(void) {
	auto& uiLayerList = UILayerList::GetInstance();
	auto uiDatas = uiLayerList.GetUILayerData(UILayerList::TYPE::PAUSE_MENU);

	for (auto& [id, uiData] : uiDatas) {
		AddUI(CreateUI(uiData));
	}
}
