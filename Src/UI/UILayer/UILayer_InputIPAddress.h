#pragma once
#include "UILayerBase.h"

class UILayer_InputIPAddress : public UILayerBase {
public:
	UILayer_InputIPAddress(void) = default;
	~UILayer_InputIPAddress(void) override = default;

private:

	void SetDefaultUI(void) override;
};

