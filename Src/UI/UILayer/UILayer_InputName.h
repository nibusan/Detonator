#pragma once
#include "UILayerBase.h"

class UILayer_InputName : public UILayerBase {
public:
	UILayer_InputName(void) = default;
	~UILayer_InputName(void) override = default;

private:

	void SetDefaultUI(void) override;
};
