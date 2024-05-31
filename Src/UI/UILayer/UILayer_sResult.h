#pragma once
#include "UILayerBase.h"

class UILayer_sResult : public UILayerBase {
public:
	UILayer_sResult(void) = default;
	~UILayer_sResult(void) override = default;

private:

	void SetDefaultUI(void) override;
};

