#pragma once
#include "UILayerBase.h"

class UILayer_Title : public UILayerBase {
public:
	UILayer_Title(void) = default;
	~UILayer_Title(void) override = default;

private:

	void SetDefaultUI(void) override;
};

