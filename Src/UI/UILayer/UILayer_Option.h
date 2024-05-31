#pragma once
#include "UILayerBase.h"

class UILayer_Option : public UILayerBase {
public:
	UILayer_Option(void) = default;
	~UILayer_Option(void) override = default;

private:

	void SetDefaultUI(void) override;
};