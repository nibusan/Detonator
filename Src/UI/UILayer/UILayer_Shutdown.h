#pragma once
#include "UILayerBase.h"

class UILayer_Shutdown : public UILayerBase {
public:
	UILayer_Shutdown(void) = default;
	~UILayer_Shutdown(void) override = default;

private:

	void SetDefaultUI(void) override;
};