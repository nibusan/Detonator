#include "../UILayer/UILayerBase.h"
#include "UIScreen.h"

void UIScreen::Init(void) {
	deleteCount_ = 0;
}

void UIScreen::Update(void) {
	for (const auto& uiLayer : uiLayers_) {
		//if (!uiLayer->()) continue;
		uiLayer->Update();
	}
}

void UIScreen::Draw(void) {
	for (const auto& uiLayer : uiLayers_) {
		uiLayer->Draw();
	}
}

void UIScreen::Release(void) {
	for (const auto& uiLayer : uiLayers_) {
		uiLayer->Release();
	}
}

void UIScreen::PushUILayer(std::unique_ptr<UILayerBase> uiLayer) {
	uiLayers_.emplace_back(std::move(uiLayer));
}

void UIScreen::PopUILayer(void) {
	deleteUILayerID_.push_back(uiLayers_.size() - 1 - deleteCount_);
	deleteCount_++;
}

int UIScreen::GetUILayerCount(void) const {
	return uiLayers_.size();
}

void UIScreen::SetIsActive_FrontUILayer(bool flag) {
	uiLayers_[uiLayers_.size() - 1]->SetIsActive(flag);
}

void UIScreen::DeleteUILayer(void) {
	for (int i = 0; i < deleteUILayerID_.size(); i++) {
		uiLayers_[deleteUILayerID_[i]]->Release();
		uiLayers_.erase(uiLayers_.begin() + deleteUILayerID_[i]);
	}
	if (uiLayers_.size() > 0) {
		// 新しく先頭になったUILayerをアクティブにする
		uiLayers_[uiLayers_.size() - 1]->SetIsActive(true);
	}
	deleteCount_ = 0;
	deleteUILayerID_.clear();
}
