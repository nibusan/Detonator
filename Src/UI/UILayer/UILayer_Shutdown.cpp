#include "../../Manager/ResourceManager.h"
#include "../../Data/UI/UILayerList.h"
#include "UILayer_Shutdown.h"

void UILayer_Shutdown::SetDefaultUI(void) {
	auto& uiLayerList = UILayerList::GetInstance();
	auto uiDatas = uiLayerList.GetUILayerData(UILayerList::TYPE::SHUTDOWN);

	for (auto& [id, uiData] : uiDatas) {
		AddUI(CreateUI(uiData));
	}
}
