#pragma once
#include "UILayerBase.h"

class UILayer_mResult : public UILayerBase {
public:
	UILayer_mResult(void) = default;
	~UILayer_mResult(void) override = default;

private:

	void SetDefaultUI(void) override;
};