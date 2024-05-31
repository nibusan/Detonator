#pragma once
#include "UILayerBase.h"

class UILayer_sGame : public UILayerBase {
public:
	UILayer_sGame(void) = default;
	~UILayer_sGame(void) override = default;

private:

	void SetDefaultUI(void) override;
};

