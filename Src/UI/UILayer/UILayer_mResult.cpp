#include "../../Manager/ResourceManager.h"
#include "../../Data/UI/UILayerList.h"
#include "UILayer_mResult.h"

void UILayer_mResult::SetDefaultUI(void) {
	auto& uiLayerList = UILayerList::GetInstance();
	auto uiDatas = uiLayerList.GetUILayerData(UILayerList::TYPE::MRESULT);

	for (auto& [id, uiData] : uiDatas) {
		AddUI(CreateUI(uiData));
	}
}
