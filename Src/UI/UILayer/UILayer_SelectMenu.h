#pragma once
#include "UILayerBase.h"

class UILayer_SelectMenu : public UILayerBase {
public:
	UILayer_SelectMenu(void) = default;
	~UILayer_SelectMenu(void) override = default;

private:

	void SetDefaultUI(void) override;
};

