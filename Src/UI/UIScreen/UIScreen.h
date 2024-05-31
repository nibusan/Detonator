#pragma once
#include <memory>
#include <vector>
#include "../UILayer/UILayerBase.h"

class UIScreen {
public:
	UIScreen(void) = default;
	~UIScreen(void) = default;

	void Init(void);
	void Update(void);
	void Draw(void);
	void Release(void);

	void PushUILayer(std::unique_ptr<UILayerBase> uiLayer);
	void PopUILayer(void);
	int GetUILayerCount(void) const;
	void SetIsActive_FrontUILayer(bool flag);
	void DeleteUILayer(void);
private:
	std::vector<std::unique_ptr<UILayerBase>> uiLayers_;
	std::vector<int> deleteUILayerID_;
	int deleteCount_;
};