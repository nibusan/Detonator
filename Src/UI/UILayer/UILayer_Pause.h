#pragma once
#include "UILayerBase.h"

class UILayer_Pause : public UILayerBase {
public:
	UILayer_Pause(void) = default;
	~UILayer_Pause(void) override = default;

private:

	void SetDefaultUI(void) override;
};