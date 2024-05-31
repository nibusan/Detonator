#include "../../Manager/ResourceManager.h"
#include "../../Data/UI/UILayerList.h"
#include "UILayer_Title.h"

void UILayer_Title::SetDefaultUI(void) {
	auto& uiLayerList = UILayerList::GetInstance();
	auto uiDatas = uiLayerList.GetUILayerData(UILayerList::TYPE::TITLE);
	
	for (auto& [id, uiData] : uiDatas) {
		AddUI(CreateUI(uiData));
	}
}
